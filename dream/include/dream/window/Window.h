//
// Created by Deepak Ramalingam on 11/3/22.
//

#ifndef DREAM_WINDOW_H
#define DREAM_WINDOW_H

#include <iostream>

namespace Dream {
    class Window {
    public:
        virtual void update(float dt);
        virtual void pollEvents();
        virtual void swapBuffers();
        [[nodiscard]] virtual bool shouldClose() const;
        virtual std::pair<int, int> getWindowDimensions();
        void setIsLoading(bool isLoading);
    protected:
        Window();
        bool shouldCloseFlag;
        bool isLoading;
    };
}

#endif //DREAM_WINDOW_H
