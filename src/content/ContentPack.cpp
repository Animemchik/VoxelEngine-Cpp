#include "ContentPack.h"

#include <iostream>
#include <algorithm>

#include "../coders/json.h"
#include "../files/files.h"
#include "../files/engine_paths.h"
#include "../data/dynamic.h"
#include "../logic/scripting/scripting.h"

namespace fs = std::filesystem;

const std::string ContentPack::PACKAGE_FILENAME = "package.json";
const std::string ContentPack::CONTENT_FILENAME = "content.json";
const fs::path ContentPack::BLOCKS_FOLDER = "blocks";
const fs::path ContentPack::ITEMS_FOLDER = "items";
const std::vector<std::string> ContentPack::RESERVED_NAMES = {
    "res", "abs", "local", "core", "user", "world", "none", "null"
};

contentpack_error::contentpack_error(
    std::string packId, 
    fs::path folder, 
    std::string message)
    : std::runtime_error(message), packId(packId), folder(folder) {
}

std::string contentpack_error::getPackId() const {
    return packId;
}
fs::path contentpack_error::getFolder() const {
    return folder;
}

fs::path ContentPack::getContentFile() const {
    return folder/fs::path(CONTENT_FILENAME);
}

bool ContentPack::is_pack(fs::path folder) {
    return fs::is_regular_file(folder/fs::path(PACKAGE_FILENAME));
}

static void checkContentPackId(const std::string& id, const fs::path& folder) {
    if (id.length() < 2 || id.length() > 24)
        throw contentpack_error(id, folder, 
            "content-pack id length is out of range [2, 24]");
    if (isdigit(id[0])) 
        throw contentpack_error(id, folder, 
            "content-pack id must not start with a digit");
    for (char c : id) {
        if (!isalnum(c) && c != '_') {
            throw contentpack_error(id, folder, 
                "illegal character in content-pack id");
        }
    }
    if (std::find(ContentPack::RESERVED_NAMES.begin(), 
                  ContentPack::RESERVED_NAMES.end(), id)
        != ContentPack::RESERVED_NAMES.end()) {
        throw contentpack_error(id, folder, 
                "this content-pack id is reserved");
    }
}

ContentPack ContentPack::read(fs::path folder) {
    auto root = files::read_json(folder/fs::path(PACKAGE_FILENAME));
    ContentPack pack;
    root->str("id", pack.id);
    root->str("title", pack.title);
    root->str("version", pack.version);
    root->str("creator", pack.creator);
    root->str("description", pack.description);
    pack.folder = folder;

    auto dependencies = root->list("dependencies");
    if (dependencies) {
        for (size_t i = 0; i < dependencies->size(); i++) {
            pack.dependencies.push_back(dependencies->str(i));
        }
    }

    if (pack.id == "none")
        throw contentpack_error(pack.id, folder, 
            "content-pack id is not specified");
    checkContentPackId(pack.id, folder);

    return pack;
}

void ContentPack::scanFolder(
    fs::path folder,
    std::vector<ContentPack>& packs
) {
    if (!fs::is_directory(folder)) {
        return;
    }
    for (auto entry : fs::directory_iterator(folder)) {
        fs::path folder = entry.path();
        if (!fs::is_directory(folder))
            continue;
        if (!is_pack(folder))
            continue;
        try {
            packs.push_back(read(folder));
        } catch (const contentpack_error& err) {
            std::cerr << "package.json error at " << err.getFolder().u8string();
            std::cerr << ": " << err.what() << std::endl;
        } catch (const std::runtime_error& err) {
            std::cerr << err.what() << std::endl;
        }
    }
}

void ContentPack::scan(
    fs::path rootfolder,
    EnginePaths* paths,
    std::vector<ContentPack>& packs
) {
    scanFolder(paths->getResources()/fs::path("content"), packs);
    scanFolder(paths->getUserfiles()/fs::path("content"), packs);
    scanFolder(rootfolder, packs);
}

void ContentPack::scan(EnginePaths* paths,
                       std::vector<ContentPack>& packs) {
    scan(paths->getWorldFolder()/fs::path("content"), paths, packs);
}

std::vector<std::string> ContentPack::worldPacksList(fs::path folder) {
    fs::path listfile = folder / fs::path("packs.list");
    if (!fs::is_regular_file(listfile)) {
        std::cerr << "warning: packs.list not found (will be created)";
        std::cerr << std::endl;
        files::write_string(listfile, "# autogenerated, do not modify\nbase\n");
    }
    return files::read_list(listfile);
}

fs::path ContentPack::findPack(const EnginePaths* paths, fs::path worldDir, std::string name) {
    fs::path folder = worldDir / fs::path("content") / fs::path(name);
    if (fs::is_directory(folder)) {
        return folder;
    }
    folder = paths->getUserfiles() / fs::path("content") / fs::path(name);
    if (fs::is_directory(folder)) {
        return folder;
    }
    folder = paths->getResources() / fs::path("content") / fs::path(name);
    if (fs::is_directory(folder)) {
        return folder;
    }
    return folder;
}

void ContentPack::readPacks(const EnginePaths* paths,
                            std::vector<ContentPack>& packs, 
                            const std::vector<std::string>& packnames,
                            fs::path worldDir) {
    for (const auto& name : packnames) {
        fs::path packfolder = ContentPack::findPack(paths, worldDir, name);
        if (!fs::is_directory(packfolder)) {
            throw contentpack_error(name, packfolder, 
                                    "could not to find pack '"+name+"'");
        }
        packs.push_back(ContentPack::read(packfolder));
    }
}

ContentPackRuntime::ContentPackRuntime(
    ContentPack info, 
    std::unique_ptr<scripting::Environment> env
) : info(info), env(std::move(env))
{
}
