precision highp float;

layout(location = 0) out vec4 FragColor;

in vec4 Color;
//in vec3 Normal;

void main()
{
    FragColor = Color;
//    FragColor = vec4(Normal, 1.0);
}