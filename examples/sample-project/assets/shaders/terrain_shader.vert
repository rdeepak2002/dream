layout (location = 0) in vec3 Position;

uniform mat4 view;
uniform mat4 projection;

out vec4 Color;

void main()
{
    gl_Position =  projection * view * vec4(Position, 1.0);

    Color = vec4(Position.y / 200.0);
}