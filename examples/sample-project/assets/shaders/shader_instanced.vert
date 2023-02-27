layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;
layout (location = 7) in mat4 aInstanceMatrix;

// variables to send to fragment shader
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out mat3 TBN;

uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0);

    // TODO: generalize by checking all id's are -1 using MAX_BONE_INFLUENCE and for loop
    if (boneIds[0] == -1 && boneIds[1] == -1 && boneIds[2] == -1 && boneIds[3] == -1) {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal = aNormal;
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
            vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
            totalNormal += localNormal;
        }
    }

    FragPos = vec3(aInstanceMatrix * totalPosition);
    // FragPos = vec3(projection * view * aInstanceMatrix * totalPosition);
    //    Normal = mat3(transpose(inverse(aInstanceMatrix))) * aNormal;
    Normal = normalize(mat3(transpose(inverse(aInstanceMatrix))) * totalNormal);
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);

    mat3 normalMatrix = transpose(inverse(mat3(aInstanceMatrix)));
    vec3 T = normalize(normalMatrix * aTangent);
    //    vec3 N = normalize(normalMatrix * aNormal);
    vec3 N = normalize(mat3(transpose(inverse(aInstanceMatrix))) * totalNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    TBN = transpose(mat3(T, B, N));

    gl_Position = projection * view * aInstanceMatrix * totalPosition;
}
