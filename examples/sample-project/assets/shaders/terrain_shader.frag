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

void main()
{
    float height = FragPos.y;

    if (height < 80.0) {
        FragColor = texture(textureDiffuse0, TexCoord);
        return;
    } else if (height < 128.0) {
        FragColor = texture(textureDiffuse1, TexCoord);
        return;
    } else {
        FragColor = texture(textureDiffuse2, TexCoord);
        return;
    }
}