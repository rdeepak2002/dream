//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIEDITORRENDERERVIEW_H
#define DREAM_IMGUIEDITORRENDERERVIEW_H

namespace Dream {
    class ImGuiEditorRendererView {
    public:
        void update(int &rendererViewportWidth, int &rendererViewportHeight, unsigned int frameBufferTexture);
    };
}

#endif //DREAM_IMGUIEDITORRENDERERVIEW_H