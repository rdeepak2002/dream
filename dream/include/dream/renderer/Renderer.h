//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_RENDERER_H
#define DREAM_RENDERER_H

namespace Dream {
    class Renderer {
    public:
        virtual unsigned int render(int viewportWidth, int viewportHeight, bool fullscreen);
        virtual void initFrameBuffer(int viewportWidth, int viewportHeight);
        virtual void resizeFrameBuffer();
    protected:
        Renderer();
    };
}

#endif //DREAM_RENDERER_H
