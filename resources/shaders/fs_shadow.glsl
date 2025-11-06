#version 330

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

uniform sampler2D texture0;  // Texture diffuse du modèle (si présente)
uniform sampler2D shadowMap;  // Shadow map
uniform vec3 lightPos;  // Position lumière

out vec4 fragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Projette en coordonnées normalisées [0,1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    if (projCoords.z > 1.0) return 0.0;  // Évite ombres au-delà du far plane

    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normalize(Normal), normalize(lightPos - FragPos))), 0.005);  // Bias dynamique
    float shadow = (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec3 color = texture(texture0, TexCoords).rgb;  // Ou vec3(1.0) si pas de texture
    vec3 normal = normalize(Normal);
    vec3 lightColor = vec3(1.0);  // Couleur lumière
    vec3 ambient = 0.3 * lightColor;  // Ambiant basique

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * color;

    fragColor = vec4(color, 1.0);
}