//
// Created by Deepak Ramalingam on 11/10/22.
//

#include "dream/renderer/OpenGLTexture.h"

#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Dream
{
    OpenGLTexture::OpenGLTexture(std::string texturePath)
    {
//        path = texturePath;
        glGenTextures(1, &id);
        this->bind();
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            int format;
            if (nrChannels == 1) {
                format = GL_RED;
            }
            else if (nrChannels == 3) {
                format = GL_RGB;
            }
            else if (nrChannels == 4) {
                format = GL_RGBA;
            } else {
                std::cout << "Unable to parse texture with " << nrChannels << " channels" << std::endl;
                exit(EXIT_FAILURE);
            }
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
            exit(EXIT_FAILURE);
        }
        stbi_image_free(data);
    }

    OpenGLTexture::~OpenGLTexture()
    {

    }

    void OpenGLTexture::bind(int unit)
    {
        if(unit >= 0)
            glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, id);
    }

    void OpenGLTexture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}