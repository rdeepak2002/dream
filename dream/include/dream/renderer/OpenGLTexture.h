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
