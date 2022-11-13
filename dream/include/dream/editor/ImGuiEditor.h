//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_IMGUIEDITOR_H
#define DREAM_IMGUIEDITOR_H

#include "dream/window/SDL2Window.h"
#include "dream/window/Window.h"
#include "dream/editor/Editor.h"
#include "dream/editor/ImGuiEditorSceneView.h"

namespace Dream {
    class ImGuiEditor : public Editor {
    public:
        ~ImGuiEditor();
        void update(Dream::Window *window, unsigned int frameBufferTexture) override;
        void style();
        std::pair<int, int> getRendererViewportDimensions() override;
    protected:
        explicit ImGuiEditor(Dream::Window *window);
        void newFrame(Dream::Window *window) override;
        void renderDrawData() override;
        int rendererViewportWidth, rendererViewportHeight;
    private:
        ImGuiEditorSceneView* sceneView;
    };
}


#endif //DREAM_IMGUIEDITOR_H
