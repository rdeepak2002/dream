/**********************************************************************************
 *  Dream is a software for developing real-time 3D experiences.
 *  Copyright (C) 2023 Deepak Ramalignam
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **********************************************************************************/

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

        ImGuiEditorSceneView *sceneView;
        ImGuiEditorInspectorView *inspectorView;
        ImGuiEditorProjectView *projectView;
        ImGuiEditorConsoleView *consoleView;
        ImGuiEditorRendererView *rendererView;
        ImGuiEditorMenu *menu;
        ImGuiTextEditor *textEditor;
        ImGuiFileBrowser *fileBrowser;
        ImGuiEditorAnimatorGraph *animatorGraphEditor;
    };
}


#endif //DREAM_IMGUIEDITOR_H
