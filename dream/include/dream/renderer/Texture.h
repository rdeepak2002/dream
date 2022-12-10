//
// Created by Deepak Ramalingam on 11/14/22.
//

#ifndef DREAM_TEXTURE_H
#define DREAM_TEXTURE_H

#include <iostream>

namespace Dream {
    class Texture {
    public:
        virtual unsigned int ID() {
            return 0;
        }
    };
}

#endif //DREAM_TEXTURE_H
