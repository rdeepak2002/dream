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

// information from vertex shader
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 FragPosLightSpace;

// material information
// TODO: encapsulate in struct
uniform float shininess;
uniform sampler2D texture_diffuse1;
uniform vec4 diffuse_color;
uniform sampler2D texture_specular;
uniform vec4 specular_color;
uniform sampler2D texture_ambient;
uniform vec4 ambient_color;
uniform sampler2D texture_normal;
uniform sampler2D texture_height;
uniform sampler2D shadowMap;

// light information
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform DirLight dirLights[MAX_NR_DIR_LIGHTS];
uniform PointLight pointLights[MAX_NR_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_NR_SPOT_LIGHTS];
uniform int numberOfDirLights;
uniform int numberOfPointLights;
uniform int numberOfSpotLights;

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

float ShadowCalculation(vec4 fragPosLightSpace)
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
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
//    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    vec2 texelSize = vec2(float(1.0) / float(textureSize(shadowMap, 0).x), float(1.0) / float(textureSize(shadowMap, 0).y));


    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
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

    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * vec3(diffuse_color);
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