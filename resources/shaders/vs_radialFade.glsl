#version 330

in vec3 vertexPosition;
in vec4 vertexColor;

uniform mat4 mvp;

out vec4 fragColor;
out vec3 fragPosition;

void main() 
{
    fragColor = vertexColor;
    fragPosition = vertexPosition;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}