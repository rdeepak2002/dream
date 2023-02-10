precision highp float;
out vec4 FragColor;

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_NR_DIR_LIGHTS 32
#define MAX_NR_POINT_LIGHTS 32
#define MAX_NR_SPOT_LIGHTS 32
#define NUM_CASCADES 4

uniform vec3 ambientColor;

// information from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// material information
uniform float shininess;
uniform sampler2D texture_diffuse1;
uniform vec4 diffuse_color;
uniform sampler2D texture_specular;
uniform vec4 specular_color;
uniform sampler2D texture_ambient;
uniform vec4 ambient_color;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;
uniform sampler2D shadowMaps[NUM_CASCADES];
//uniform mat4 lightSpaceMatrix;
uniform mat4 lightSpaceMatrices[NUM_CASCADES];
uniform float cascadePlaneDistances[16];

// light information
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform float farPlane;
uniform DirLight dirLights[MAX_NR_DIR_LIGHTS];
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfDirLights;
uniform int numberOfPointLights;
uniform int numberOfSpotLights;

uniform mat4 view;

float gamma = 2.2;

// light function prototypes
float ShadowCalculation(vec4 fragPosLightSpace);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec4 tc = texture(texture_diffuse1, TexCoord) * diffuse_color;
    if(tc.a < 0.1)
        discard;
//    vec4 texColor = texture(texture_diffuse1, TexCoord) * diffuse_color;
//    if(texColor.a < 0.1)
//        discard;
//    FragColor = texColor;

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // properties
    // =======================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // =======================================================
    vec3 result = vec3(0.0, 0.0, 0.0);
    // phase 1: directional lighting
    for(int i = 0; i < numberOfDirLights; i++)
    result += CalcDirLight(dirLights[i], norm, viewDir);
    // phase 2: point lights
    for(int i = 0; i < numberOfPointLights; i++)
    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    // phase 3: spot light
    for(int i = 0; i < numberOfSpotLights; i++)
    result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);

    // calculate shadow
//    result = result * (1.0 - shadow);
//    result = vec3(shadow, shadow, shadow);
//    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    result.rgb = pow(result.rgb, vec3(1.0 / gamma));

    FragColor = vec4(result, 1.0);
}

float ShadowCalculation(int cascadeIndex, vec4 fragPosLightSpace)
{
//    // perform perspective divide
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//    float closestDepth = texture(shadowMap, projCoords.xy).r;
//    // get depth of current fragment from light's perspective
//    float currentDepth = projCoords.z;
//    // check whether current frag pos is in shadow
//    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
//
//    return shadow;

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float a;
    if (cascadeIndex == 0) {
        a = float(texture(shadowMaps[0], projCoords.xy).r);
    } else if (cascadeIndex == 1) {
        a = float(texture(shadowMaps[1], projCoords.xy).r);
    } else if (cascadeIndex == 2) {
        a = float(texture(shadowMaps[2], projCoords.xy).r);
    } else if (cascadeIndex == 3) {
        a = float(texture(shadowMaps[3], projCoords.xy).r);
    }
    float closestDepth = a;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
//    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05f * (1.0 - dot(normal, normalize(lightDir))), 0.005);
    if (cascadeIndex == 3)
    {
        bias = 0.00003f;
    }
    else if (cascadeIndex == 2) {
        bias = 0.0003f;
    }
    else if (cascadeIndex == 1) {
        bias = 0.0003f;
    }
    else if (cascadeIndex == 0) {
        bias = 0.0003f;
    }
//    bias = 0.0f;
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
//    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    float b;
    float c;

    if (cascadeIndex == 0) {
        b = float(textureSize(shadowMaps[0], 0).x);
        c = float(textureSize(shadowMaps[0], 0).y);
    } else if (cascadeIndex == 1) {
        b = float(textureSize(shadowMaps[1], 0).x);
        c = float(textureSize(shadowMaps[1], 0).y);
    } else if (cascadeIndex == 2) {
        b = float(textureSize(shadowMaps[2], 0).x);
        c = float(textureSize(shadowMaps[2], 0).y);
    } else if (cascadeIndex == 3) {
        b = float(textureSize(shadowMaps[3], 0).x);
        c = float(textureSize(shadowMaps[3], 0).y);
    }

    vec2 texelSize = vec2(float(1.0) / float(b), float(1.0) / float(c));


    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float d;

            if (cascadeIndex == 0) {
                d = float(texture(shadowMaps[0], projCoords.xy + vec2(x, y) * texelSize).r);
            } else if (cascadeIndex == 1) {
                d = float(texture(shadowMaps[1], projCoords.xy + vec2(x, y) * texelSize).r);
            } else if (cascadeIndex == 2) {
                d = float(texture(shadowMaps[2], projCoords.xy + vec2(x, y) * texelSize).r);
            } else if (cascadeIndex == 3) {
                d = float(texture(shadowMaps[3], projCoords.xy + vec2(x, y) * texelSize).r);
            }

            float pcfDepth = d;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
    shadow = 0.0;

    return shadow;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // TODO: multiply by material's diffuse and specular color

    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    //    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));           // TODO: use ambient texture

    // gamma
    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
//    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(texture(texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(specular_color) * vec3(texture(texture_specular, TexCoord));
//    return (ambient + diffuse + specular);

    // TODO: iterate through light space matrices
    float shadow = 0.0f;

    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(FragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;

    vec3 shadowCascadeDebugColor = vec3(0, 0, 0);

    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            if (i == 0) {
//                shadowCascadeDebugColor = vec3(1.0, 0.0, 0.0);
            }
            if (i == 1) {
//                shadowCascadeDebugColor = vec3(0.0, 1.0, 0.0);
            }
            if (i == 2) {
//                shadowCascadeDebugColor = vec3(0.0, 0.0, 1.0);
            }
            if (i == 3) {
//                shadowCascadeDebugColor = vec3(1.0, 1.0, 0.0);
            }
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = NUM_CASCADES - 1;
    }

    int cascadeIndex = layer;
    vec4 FragPosLightSpace = lightSpaceMatrices[cascadeIndex] * vec4(FragPos, 1.0);
    float shadowFactor = ShadowCalculation(cascadeIndex, FragPosLightSpace);
    shadow += shadowFactor;

//    for (int cascadeIndex = 0; cascadeIndex < NUM_CASCADES; cascadeIndex++) {
//        vec4 FragPosLightSpace = lightSpaceMatrices[cascadeIndex] * vec4(FragPos, 1.0);
//        float shadowFactor = ShadowCalculation(cascadeIndex, FragPosLightSpace);
//        shadow += shadowFactor;
//    }
    vec3 lighting = ambientColor * ambient + ((1.0 - shadow) * (diffuse + specular)) * vec3(diffuse_color) + shadowCascadeDebugColor;
    return lighting;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // TODO: multiply by material's diffuse and specular color

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
//    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));                   // TODO: use ambient texture
    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
    vec3 specular = light.specular * spec * vec3(specular_color) * vec3(texture(texture_specular, TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // TODO: multiply by material's diffuse and specular color

    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
//    vec3 reflectDir = reflect(-lightDir, normal);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));           // TODO: use ambient texture
    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
    vec3 specular = light.specular * spec * vec3(specular_color) * vec3(texture(texture_specular, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}