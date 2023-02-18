precision highp float;

layout(location = 0) out vec4 FragColor;

in vec4 Color;

void main()
{
    FragColor = Color;
}