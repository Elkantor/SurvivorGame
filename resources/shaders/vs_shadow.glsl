#version 330

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 mvp;  // Automatique en Raylib (model-view-proj)
uniform mat4 model;  // Matrice modèle
uniform mat4 lightSpaceMatrix;  // View * Proj de la lumière
uniform mat3 normalMatrix;  // Pour normals (transpose(inverse(model)))

void main()
{
    FragPos = vec3(model * vec4(vertexPosition, 1.0));
    Normal = normalMatrix * vertexNormal;
    TexCoords = vertexTexCoord;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = mvp * vec4(vertexPosition, 1.0);  // Ou utilisez FragPos si vous voulez
}