precision highp float;

in vec2 TexCoord;
uniform sampler2D texture_diffuse1;
uniform vec4 diffuse_color;

void main()
{
    vec4 tc = texture(texture_diffuse1, TexCoord) * diffuse_color;
    if(tc.a < 0.1)
        discard;
//    gl_FragDepth = gl_FragCoord.z;
}