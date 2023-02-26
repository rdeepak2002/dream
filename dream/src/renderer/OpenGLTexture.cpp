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

#include "dream/renderer/OpenGLTexture.h"
#include "dream/util/Logger.h"

#include <cassert>

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

namespace Dream {
    OpenGLTexture::OpenGLTexture(stbi_uc const *buffer, int len, bool flipTexture) {
        glGenTextures(1, &id);
        this->bind();
        stbi_set_flip_vertically_on_load(flipTexture);
        unsigned char *data = stbi_load_from_memory(buffer, len, &width, &height, &nrChannels, 0);
        if (data) {
            int format;
            if (nrChannels == 1) {
                format = GL_RED;
#ifdef EMSCRIPTEN
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif

            } else if (nrChannels == 3) {
                format = GL_RGB;

#ifdef EMSCRIPTEN
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif
            } else if (nrChannels == 4) {
                format = GL_RGBA;

#ifdef EMSCRIPTEN
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif
            } else {
                Logger::fatal("Unable to parse texture with " + std::to_string(nrChannels) + " channels");
            }
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            Logger::fatal("(1) Failed to load texture from data");
        }
        stbi_image_free(data);
    }

    OpenGLTexture::OpenGLTexture(std::string texturePath, bool flipTexture) {
        glGenTextures(1, &id);
        this->bind();
        stbi_set_flip_vertically_on_load(flipTexture);
        unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (texturePath.empty()) {
            Logger::fatal("Empty texture path provided");
        }
        if (data) {
            int format;
            if (nrChannels == 1) {
                format = GL_RED;
#ifdef EMSCRIPTEN
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif
            } else if (nrChannels == 3) {
                format = GL_RGB;
#ifdef EMSCRIPTEN
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif
            } else if (nrChannels == 4) {
                format = GL_RGBA;

#ifdef EMSCRIPTEN
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#else
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
#endif
            }
            // TODO: figure this out...
//            else if (nrChannels == 2) {
//                format = GL_RG;
//                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//            }
            else {
                Logger::fatal("Unable to parse texture with " + std::to_string(nrChannels) + " channels for " + texturePath);
            }

            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            Logger::fatal("(2) Failed to load texture " + texturePath);
        }
        stbi_image_free(data);
    }

    OpenGLTexture::~OpenGLTexture() {
//        bind(0);
//        glDeleteTextures(0, &id);
    }

    void OpenGLTexture::bind(int unit) {
        if (unit >= 0)
            glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void OpenGLTexture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned int OpenGLTexture::ID() {
        return id;
    }
}