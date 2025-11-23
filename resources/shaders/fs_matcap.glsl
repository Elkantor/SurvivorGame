#version 330

// Input from vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;  // diffuse
uniform sampler2D texture1;  // matcap (used from material.maps[MATERIAL_MAP_METALNESS])

uniform float contrast;
uniform float brightness;

void main()
{
    vec4 diffuseColor = texture(texture0, fragTexCoord);
    vec2 uv = fragNormal.xy * 0.5 + 0.5;
    vec4 matcapColor = texture(texture1, uv);

    /*float contrast = 0.99;
    float brightness = 0.6;*/
    matcapColor = (matcapColor - 0.5) * contrast + 0.5 + brightness;
    matcapColor = clamp(matcapColor, 0.0, 1.0);

    finalColor = diffuseColor * matcapColor * fragColor;
}