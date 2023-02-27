layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;
layout (location = 7) in mat4 aInstanceMatrix;

out vec2 TexCoord;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

uniform bool instanced;

void main()
{
    vec4 totalPosition = vec4(0.0f);

    // TODO: generalize by checking all id's are -1 using MAX_BONE_INFLUENCE and for loop
    if (boneIds[0] == -1 && boneIds[1] == -1 && boneIds[2] == -1 && boneIds[3] == -1) {
        totalPosition = vec4(aPos, 1.0f);
    } else {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
            if(boneIds[i] == -1) {
                continue;
            }

            if(boneIds[i] >= MAX_BONES) {
                totalPosition = vec4(aPos, 1.0f);
                break;
            }

            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0f);
            totalPosition += localPosition * weights[i];
        }
    }

    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    if (instanced) {
        gl_Position = lightSpaceMatrix * aInstanceMatrix * totalPosition;
    } else {
        gl_Position = lightSpaceMatrix * model * totalPosition;
    }
}