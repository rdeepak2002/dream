precision highp float;

layout(location = 0) out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
//in vec3 Normal;

uniform sampler2D textureDiffuse0;
uniform sampler2D textureNormal0;

uniform sampler2D textureDiffuse1;
uniform sampler2D textureNormal1;

uniform sampler2D textureDiffuse2;
uniform sampler2D textureNormal2;

uniform float gMinHeight;
uniform float gMaxHeight;

uniform float gHeight0;
uniform float gHeight1;
uniform float gHeight2;

void main()
{
    vec4 texColor;
    float height = FragPos.y;

    if (height < gHeight0) {
        texColor = texture(textureDiffuse0, TexCoord);
    } else if (height < gHeight1) {
        float Delta = gHeight1 - gHeight0;
        float Factor = (height - gHeight0) / Delta;
        vec4 Color0 = texture(textureDiffuse0, TexCoord);
        vec4 Color1 = texture(textureDiffuse1, TexCoord);
        texColor = mix(Color0, Color1, Factor);
    } else if (height < gHeight2) {
        float Delta = gHeight2 - gHeight1;
        float Factor = (height - gHeight1) / Delta;
        vec4 Color0 = texture(textureDiffuse1, TexCoord);
        vec4 Color1 = texture(textureDiffuse2, TexCoord);
        texColor = mix(Color0, Color1, Factor);
    } else {
        vec4 Color1 = texture(textureDiffuse2, TexCoord);
        texColor = Color1;
    }

    FragColor = texColor;
}