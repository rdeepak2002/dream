precision highp float;
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture_diffuse1;
uniform vec4 diffuse_color;

void main()
{
//    FragColor = diffuse_color;
    vec4 texColor = texture(texture_diffuse1, TexCoord) * diffuse_color;
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}