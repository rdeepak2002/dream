precision highp float;

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D tex;
uniform vec4 color;

void main()
{
    vec4 texColor = texture(tex, TexCoord) * color;
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}