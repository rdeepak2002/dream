//layout(triangles, invocations = 5) in;
//layout(triangle_strip, max_vertices = 3) out;
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout (std140) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

//#define ID gl_InvocationID

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = lightSpaceMatrices[0] * gl_in[i].gl_Position;
        gl_Layer = 0;
        EmitVertex();
    }
    EndPrimitive();
}

//layout (points) in;
//layout (points, max_vertices = 1) out;
//
//void main() {
//    gl_Position = gl_in[0].gl_Position;
//    EmitVertex();
//    EndPrimitive();
//}