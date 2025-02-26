#include <string>
#include <filesystem>
#include <glm/glm.hpp>

#include "../../delegates.h"

#include "scripting_functional.h"

namespace fs = std::filesystem;

class Engine;
class Content;
struct ContentPack;
class ContentIndices;
class Level;
class Block;
class Player;
class ItemDef;
class Inventory;
class UiDocument;
struct block_funcs_set;
struct item_funcs_set;
struct uidocscript;
class BlocksController;

namespace scripting {
    extern Engine* engine;
    extern const Content* content;
    extern const ContentIndices* indices;
    extern Level* level;
    extern BlocksController* blocks;

    /* Lua environment wrapper for automatic deletion */
    class Environment {
        int env;
    public:
        Environment(int env);
        ~Environment();

        int getId() const;
    };

    void initialize(Engine* engine);

    extern bool register_event(int env, const std::string& name, const std::string& id);

    std::unique_ptr<Environment> create_environment(int parent=0);
    std::unique_ptr<Environment> create_pack_environment(const ContentPack& pack);
    std::unique_ptr<Environment> create_doc_environment(int parent, const std::string& name);

    void on_world_load(Level* level, BlocksController* blocks);
    void on_world_save();
    void on_world_quit();
    void on_blocks_tick(const Block* block, int tps);
    void update_block(const Block* block, int x, int y, int z);
    void random_update_block(const Block* block, int x, int y, int z);
    void on_block_placed(Player* player, const Block* block, int x, int y, int z);
    void on_block_broken(Player* player, const Block* block, int x, int y, int z);
    bool on_block_interact(Player* player, const Block* block, int x, int y, int z);

    /** Called on RMB click on block with the item selected 
       @return true if prevents default action */
    bool on_item_use_on_block(Player* player, const ItemDef* item, int x, int y, int z);
    /** Called on LMB click on block with the item selected 
       @return true if prevents default action */
    bool on_item_break_block(Player* player, const ItemDef* item, int x, int y, int z);

    /** Called on UI view show */
    void on_ui_open(UiDocument* layout, Inventory* inventory, glm::ivec3 blockcoord);
    /** Called on UI view close*/
    void on_ui_close(UiDocument* layout, Inventory* inventory);

    /** Load script associated with a Block */
    void load_block_script(int env, std::string prefix, fs::path file, block_funcs_set& funcsset);
    /** Load script associated with an Item */
    void load_item_script(int env, std::string prefix, fs::path file, item_funcs_set& funcsset);

    /** 
     * Load package-specific world script 
     * @param env environment id
     * @param packid content-pack id
     * @param file script file path
     */
    void load_world_script(int env, std::string packid, fs::path file);

    /** Load script associated with an UiDocument */
    void load_layout_script(int env, std::string prefix, fs::path file, uidocscript& script);

    /** Finalize lua state. Using scripting after will lead to Lua panic */
    void close();
}
