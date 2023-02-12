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
in mat3 TBN;

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
uniform vec3 shadowDirectionalLightDir;
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
float ShadowCalculation(int cascadeIndex, vec3 normal);
vec3 CalcDirLight(DirLight light);
vec3 CalcPointLight(PointLight light);
vec3 CalcSpotLight(SpotLight light);

void main()
{
    vec4 tc = texture(texture_diffuse1, TexCoord) * diffuse_color;
    if(tc.a < 0.1)
        discard;
    vec3 result = vec3(0.0, 0.0, 0.0);
    for(int i = 0; i < numberOfDirLights; i++) {
        result += CalcDirLight(dirLights[i]);
    }
    for(int i = 0; i < numberOfPointLights; i++) {
        result += CalcPointLight(pointLights[i]);
    }
    for(int i = 0; i < numberOfSpotLights; i++) {
        result += CalcSpotLight(spotLights[i]);
    }

    result.rgb = pow(result.rgb, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}

float ShadowCalculation(int cascadeIndex, vec3 normal)
{
//    vec3 normal = normalize(Normal);
    vec4 fragPosLightSpace = lightSpaceMatrices[cascadeIndex] * vec4(FragPos, 1.0);

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
    float bias = max(0.05f * (1.0 - dot(normal, normalize(shadowDirectionalLightDir))), 0.005);
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

    float shadow = 0.0;
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
vec3 CalcDirLight(DirLight light) {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-light.direction);

    bool enableNormalMapping = true;
    if (enableNormalMapping) {
        vec3 TangentViewPos  = TBN * viewPos;
        vec3 TangentFragPos  = TBN * FragPos;
        normal = texture(texture_normal, TexCoord).rgb;
        if (length(normal) <= 0.0f) {
            // handle case where no normal map is associatd with the model, so we end up using a black texture for the normal map
            normal = vec3(0.5, 0.5, 1);
        }
        normal = normalize(normal * 2.0 - 1.0);
        viewDir = normalize(TangentViewPos - TangentFragPos);
        vec3 lightPos = FragPos + normalize(-light.direction);
        vec3 TangentLightPos = TBN * lightPos;
        lightDir = normalize(TangentLightPos - TangentFragPos);
    }

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 ambient = light.ambient * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
    vec3 specular = light.specular * spec * vec3(specular_color) * vec3(texture(texture_specular, TexCoord));

    float shadow = 0.0f;
    vec4 fragPosViewSpace = view * vec4(FragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    int layer = -1;
    vec3 shadowCascadeDebugColor = vec3(0, 0, 0);
    for (int i = 0; i < NUM_CASCADES; ++i) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }
    if (layer == -1) {
        layer = NUM_CASCADES - 1;
    }

    int cascadeIndex = layer;
    float shadowFactor = ShadowCalculation(cascadeIndex, normal);
    shadow += shadowFactor;

    vec3 lighting = ambientColor * ambient + ((1.0 - shadow) * (diffuse + specular)) * vec3(diffuse_color) + shadowCascadeDebugColor;
    return lighting;
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light) {
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);

    // normal mapping
    bool enableNormalMapping = true;
    if (enableNormalMapping) {
        vec3 TangentViewPos  = TBN * viewPos;
        vec3 TangentFragPos  = TBN * FragPos;
        normal = texture(texture_normal, TexCoord).rgb;
        if (length(normal) <= 0.0f) {
            // handle case where no normal map is associatd with the model, so we end up using a black texture for the normal map
            normal = vec3(0.5, 0.5, 1);
        }
        normal = normalize(normal * 2.0 - 1.0);
        viewDir = normalize(TangentViewPos - TangentFragPos);
        vec3 TangentLightPos = TBN * light.position;
        lightDir = normalize(TangentLightPos - TangentFragPos);
    }

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    vec3 ambient = light.ambient * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));                   // TODO: use ambient texture
    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
    vec3 specular = light.specular * spec * vec3(specular_color) * vec3(texture(texture_specular, TexCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light) {
    vec3 normal = normalize(Normal);
    vec3 fragPos = FragPos;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    vec3 ambient = light.ambient * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));           // TODO: use ambient texture
    vec3 diffuse = light.diffuse * diff * vec3(diffuse_color) * vec3(pow(texture(texture_diffuse1, TexCoord).rgb, vec3(gamma)));
    vec3 specular = light.specular * spec * vec3(specular_color) * vec3(texture(texture_specular, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}