#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Custom variables
uniform vec3 color;

void main()
{
    finalColor = vec4(color, 1.f);
}
