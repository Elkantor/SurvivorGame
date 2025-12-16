#version 330

// Input from vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

// Output fragment color
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;  // diffuse
uniform sampler2D texture1;  // matcap (used from material.maps[MATERIAL_MAP_METALNESS])
uniform float contrast;
uniform float brightness;
uniform vec3 viewEye;

// For Rim Light
uniform float rimPower;
uniform float rimIntensity;
uniform vec3 rimColor;

void main()
{
    vec4 diffuseColor = texture(texture0, fragTexCoord);
    vec2 uv = fragNormal.xy * 0.5 + 0.5;
    vec4 matcapColor = texture(texture1, uv);
    matcapColor = (matcapColor - 0.5) * contrast + 0.5 + brightness;
    matcapColor = clamp(matcapColor, 0.0, 1.0);

    // Rim Light
    vec3 viewDir = normalize(viewEye - fragPosition);
    vec3 normal = normalize(fragNormal);
    float rim = max(0.0, 1.0 - dot(normal, viewDir));
    rim = pow(rim, rimPower);
    vec3 rimEffect = rimColor * rim * rimIntensity;

    //finalColor = diffuseColor * matcapColor * fragColor + vec4(rimEffect, 0.0);
    // sharpen on edges with a mix to make models more "visible"
    vec4 baseColor = diffuseColor * matcapColor * fragColor;
    finalColor = mix(baseColor, vec4(rimEffect, 0.0) + baseColor, rim);
}