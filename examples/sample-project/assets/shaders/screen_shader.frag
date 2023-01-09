precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;

//    float gamma = 2.2;
//    col.rgb = pow(col.rgb, vec3(1.0 / gamma));

    FragColor = vec4(col, 1.0);
}