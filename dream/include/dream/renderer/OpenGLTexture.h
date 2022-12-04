//
// Created by Deepak Ramalingam on 11/10/22.
//

#ifndef DREAM_OPENGLTEXTURE_H
#define DREAM_OPENGLTEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb/stb_image.h>
#include "dream/renderer/Texture.h"

namespace Dream {
    class OpenGLTexture : public Texture {
        private:
//            unsigned int ID;
//            // directly change the texture state where relevant from within the setters.
//            GLenum Target         = GL_TEXTURE_2D;           // what type of texture we're dealing with
//            GLenum InternalFormat = GL_RGBA;                 // number of color components
//            GLenum Format         = GL_RGBA;                 // the format each texel is stored in
//            GLenum Type           = GL_UNSIGNED_BYTE;
//            GLenum FilterMin      = GL_LINEAR_MIPMAP_LINEAR; // what filter method to use during minification
//            GLenum FilterMax      = GL_LINEAR;               // what filter method to use during magnification
//            GLenum WrapS          = GL_REPEAT;               // wrapping method of the S coordinate
//            GLenum WrapT          = GL_REPEAT;               // wrapping method of the T coordinate
//            GLenum WrapR          = GL_REPEAT;               // wrapping method of the R coordinate
//            bool Mipmapping       = true;
//
            unsigned int id = 0;
            int width  = 0;
            int height = 0;
            int nrChannels = 0;
//            unsigned int Depth  = 0;
        private:
        public:
            OpenGLTexture(std::string texturePath, bool flipTexture=true);
            OpenGLTexture(stbi_uc const *buffer, int len, bool flipTexture=true);
            ~OpenGLTexture();
            void bind(int unit = -1);
            void unbind();
            unsigned int ID() override;
    };
}

#endif //DREAM_OPENGLTEXTURE_H
