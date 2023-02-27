layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;
layout (location = 7) in mat4 aInstanceMatrix;

//layout (location = 0) in vec3 aPos;
//layout (location = 2) in vec2 aTexCoord;
//layout (location = 3) in mat4 aInstanceMatrix;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aTexCoord;
    gl_Position = projection * view * aInstanceMatrix * vec4(aPos, 1.0f);
}