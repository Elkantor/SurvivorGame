#version 330

// Input from vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

// For shadows
in vec4 fragPosLightSpace;

// Output fragment color
out vec4 finalColor;

// Uniforms
uniform sampler2D texture0;  // diffuse
uniform sampler2D texture1;  // matcap (used from material.maps[MATERIAL_MAP_METALNESS])
uniform float contrast;
uniform float brightness;
uniform vec3 viewEye;

// For rim light
uniform float rimPower;
uniform float rimIntensity;
uniform vec3 rimColor;

// For shadows
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform int shadowMapResolution;
uniform mat4 lightVP; // Light source view projection

float ShadowCompute()
{
    // Shadow calculations
    vec4 fragPosLightSpace = lightVP * vec4(fragPosition, 1);
    fragPosLightSpace.xyz /= fragPosLightSpace.w; // Perform the perspective division
    fragPosLightSpace.xyz = (fragPosLightSpace.xyz + 1.0)/2.0; // Transform from [-1, 1] range to [0, 1] range
    vec2 sampleCoords = fragPosLightSpace.xy;
    float curDepth = fragPosLightSpace.z;

    // Slope-scale depth bias: depth biasing reduces "shadow acne" artifacts, where dark stripes appear all over the scene
    // The solution is adding a small bias to the depth
    // In this case, the bias is proportional to the slope of the surface, relative to the light
    vec3 normal = normalize(fragNormal);
    vec3 l = -lightDir;
    float bias = max(0.000002*(1.01 - dot(normal, l)), 0.00002) + 0.00001;
    int shadowCounter = 0;
    const int numSamples = 120;

    // PCF (percentage-closer filtering) algorithm:
    // Instead of testing if just one point is closer to the current point,
    // we test the surrounding points as well
    // This blurs shadow edges, hiding aliasing artifacts
    vec2 texelSize = vec2(1.0/float(shadowMapResolution));
    for (int x = -5; x <= 5; x++)
    {
        for (int y = -5; y <= 5; y++)
        {
            float sampleDepth = texture(shadowMap, sampleCoords + texelSize*vec2(x, y)).r;
            if (curDepth - bias > sampleDepth) shadowCounter++;
        }
    }

    float shadow = float(shadowCounter) / float(numSamples);
    return shadow;
}

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

    // Rim light modulation based on the angle between the view and the direction of the light
    // If the view is in the same direction as the light (dot > 0), reduce the rim; if opposite (dot < 0), full rim
    vec3 normLightDir = normalize(lightDir);
    float rimMod = max(0.0, -dot(viewDir, normLightDir));
    rimEffect *= rimMod;  // Applique la modulation à l'effet rim

    // sharpen on edges with a mix to make models more "visible"
    vec4 baseColor = diffuseColor * matcapColor * fragColor;

    float shadow = ShadowCompute();
    float shadowIntensity = shadow * 0.8f;
    baseColor.rgb *= (0.6 + 0.4 * (1.0 - shadowIntensity));

    finalColor = mix(baseColor, vec4(rimEffect, 0.0) + baseColor, rim);
}