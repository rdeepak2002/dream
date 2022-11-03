//
// Created by Deepak Ramalingam on 10/27/22.
//

#ifndef DREAM_IMGUISDL2OPENGLEDITOR_H
#define DREAM_IMGUISDL2OPENGLEDITOR_H

#include "dream/editor/ImGuiEditor.h"

namespace Dream {
    class ImGuiSDL2OpenGLEditor : public ImGuiEditor {
    public:
        explicit ImGuiSDL2OpenGLEditor(Dream::SDL2OpenGLWindow *window);
        static void pollEvents(SDL_Event Event);
    protected:
        void newFrame(Dream::SDL2OpenGLWindow *window) override;
        void renderDrawData() override;
    };
}

#endif //DREAM_IMGUISDL2OPENGLEDITOR_H
