//
// Created by Deepak Ramalingam on 11/28/22.
//

#ifndef DREAM_IMGUIEDITORRENDERERVIEW_H
#define DREAM_IMGUIEDITORRENDERERVIEW_H

namespace Dream {
    class ImGuiEditorRendererView {
    public:
        ImGuiEditorRendererView();
        void update(int &rendererViewportWidth, int &rendererViewportHeight, unsigned int frameBufferTexture);
    private:
        unsigned int playIcon, stopIcon, expandIcon;
    };
}

#endif //DREAM_IMGUIEDITORRENDERERVIEW_H
