#ifndef FRONTEND_GUI_GUI_H_
#define FRONTEND_GUI_GUI_H_

#include <memory>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <functional>
#include <unordered_map>

class GfxContext;
class Assets;
class Camera;

/*
 Some info about padding and margin.
    Padding is element inner space, margin is outer
    glm::vec4 usage:
      x - left
      y - top
      z - right
      w - bottom

 Outer element
 +======================================================================+
 |            .           .                    .          .             |
 |            .padding.y  .                    .          .             |
 | padding.x  .           .                    .          .   padding.z |
 |- - - - - - + - - - - - + - - - - - - - - - -+- - - - - + - - - - - - |
 |            .           .                    .          .             |
 |            .           .margin.y            .          .             |
 |            .margin.x   .                    .  margin.z.             |
 |- - - - - - + - - - - - +====================+- - - - - + - - - - - - |
 |            .           |    Inner element   |          .             |
 |- - - - - - + - - - - - +====================+- - - - - + - - - - - - |
 |            .           .                    .          .             |
 |            .           .margin.w            .          .             |
 |            .           .                    .          .             |
 |- - - - - - + - - - - - + - - - - - - - - - -+- - - - - + - - - - - - |
 |            .           .                    .          .             |
 |            .padding.w  .                    .          .             |
 |            .           .                    .          .             |
 +======================================================================+
*/

namespace gui {
    class UINode;
    class Container;
    class PagesControl;

    /** The main UI controller */
    class GUI {
        std::shared_ptr<Container> container;
        std::shared_ptr<UINode> hover = nullptr;
        std::shared_ptr<UINode> pressed = nullptr;
        std::shared_ptr<UINode> focus = nullptr;
        std::unordered_map<std::string, std::shared_ptr<UINode>> storage;

        std::unique_ptr<Camera> uicamera;
        std::shared_ptr<PagesControl> menu;
        void actMouse(float delta);
    public:
        GUI();
        ~GUI();

        /** Get the main menu (PagesControl) node */
        std::shared_ptr<PagesControl> getMenu();

        /** Get current focused node 
          * @return focused node or nullptr */
        std::shared_ptr<UINode> getFocused() const;

        /** Check if all user input is caught by some element like TextBox */
        bool isFocusCaught() const;

        /** Main input handling and logic update method 
         * @param delta delta time */
        void act(float delta);

        /** Draw all visible elements on main container 
         * @param pctx parent graphics context
         * @param assets active assets storage */
        void draw(const GfxContext* pctx, Assets* assets);

        /** Add node to the main container */
        void add(std::shared_ptr<UINode> node);

        /** Remove node from the main container */
        void remove(std::shared_ptr<UINode> node) noexcept;

        /** Store node in the GUI nodes dictionary 
         * (does not add node to the main container)
         * @param name node key
         * @param node target node
         */
        void store(std::string name, std::shared_ptr<UINode> node);

        /** Get node from the GUI nodes dictionary 
         * @param name node key
         * @return stored node or nullptr
        */
        std::shared_ptr<UINode> get(std::string name) noexcept;

        /** Remove node from the GUI nodes dictionary
         * @param name node key 
         */
        void remove(std::string name) noexcept;

        /** Set node as focused 
          * @param node new focused node or nullptr to remove focus
          */
        void setFocus(std::shared_ptr<UINode> node);

        /** Get the main container */
        std::shared_ptr<Container> getContainer() const;
    };
}

#endif // FRONTEND_GUI_GUI_H_
