//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_IMGUIEDITOR_H
#define DREAM_IMGUIEDITOR_H

#include "dream/window/SDL2Window.h"
#include "dream/window/Window.h"
#include "dream/editor/Editor.h"
#include "dream/editor/ImGuiEditorSceneView.h"
#include "dream/editor/ImGuiEditorInspectorView.h"
#include "dream/editor/ImGuiEditorProjectView.h"
#include "dream/editor/ImGuiEditorConsoleView.h"
#include "dream/editor/ImGuiEditorRendererView.h"
#include "dream/editor/ImGuiEditorMenu.h"
#include "dream/editor/ImGuiTextEditor.h"
#include "dream/editor/ImGuiFileBrowser.h"
#include "dream/editor/ImGuiEditorAnimatorGraph.h"

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
        void setupDockSpace();
        ImGuiEditorSceneView* sceneView;
        ImGuiEditorInspectorView* inspectorView;
        ImGuiEditorProjectView* projectView;
        ImGuiEditorConsoleView* consoleView;
        ImGuiEditorRendererView* rendererView;
        ImGuiEditorMenu* menu;
        ImGuiTextEditor* textEditor;
        ImGuiFileBrowser* fileBrowser;
        ImGuiEditorAnimatorGraph* animatorGraphEditor;
    };
}


#endif //DREAM_IMGUIEDITOR_H
