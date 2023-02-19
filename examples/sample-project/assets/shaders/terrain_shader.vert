layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoord;
//out vec3 Normal;

void main()
{
    gl_Position =  projection * view * vec4(aPos, 1.0);
//    Normal = mat3(transpose(inverse(model))) * aNormal;

//    FragPos = vec3(model * totalPosition);
    FragPos = aPos;

    TexCoord = aTexCoord;
}