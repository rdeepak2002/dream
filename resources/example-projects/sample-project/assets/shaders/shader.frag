precision highp float;
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_diffuse1;

void main()
{
    vec4 texColor = texture(texture_diffuse1, TexCoord);
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}