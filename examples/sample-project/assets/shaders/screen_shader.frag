precision highp float;
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
//    vec3 col = texture(screenTexture, TexCoords).rgb;
//
////    float gamma = 2.2;
////    col.rgb = pow(col.rgb, vec3(1.0 / gamma));
//
//    FragColor = vec4(col, 1.0);

    float exposure = 1.4;
//    const float gamma = 2.2;
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;

    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    // gamma correction
//    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}