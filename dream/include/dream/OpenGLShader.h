//
// Created by Deepak Ramalingam on 10/29/22.
//

#ifndef DREAM_OPENGLSHADER_H
#define DREAM_OPENGLSHADER_H

#include <iostream>
#include <glm/glm.hpp>

namespace Dream {
    class OpenGLShader {
    public:
        unsigned int ID;
        OpenGLShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
        void use();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, const glm::vec2 &value) const;
        void setVec3(const std::string &name, const glm::vec3 &value) const;
        void setVec3(const std::string &name, float x, float y, float z) const;
        void setVec4(const std::string &name, const glm::vec4 &value) const;
        void setVec4(const std::string &name, float x, float y, float z, float w);
        void setMat2(const std::string &name, const glm::mat2 &mat) const;
        void setMat3(const std::string &name, const glm::mat3 &mat) const;
        void setMat4(const std::string &name, const glm::mat4 &mat) const;
        static std::string getShaderVersion();
    private:
        void checkCompileErrors(int shader, std::string type);
    };
}

#endif //DREAM_OPENGLSHADER_H
