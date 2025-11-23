#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;
in vec4 vertexColor;

// Output vertex attributes (to fragment shader)
out vec3 fragNormal;
out vec2 fragTexCoord;
out vec4 fragColor;

// Uniforms (Raylib defaults)
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    fragNormal = normalize((matNormal * vec4(vertexNormal, 0.0)).xyz);
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}