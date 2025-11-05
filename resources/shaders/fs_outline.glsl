#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;       // Color buffer
uniform sampler2D depthTex;       // Depth buffer
uniform vec2 screenSize;
uniform vec4 outlineColor;
uniform float edgeThreshold;

float GetDepth(vec2 uv)
{
    return texture(depthTex, uv).r;
}

void main()
{
    vec2 texel = 1.0 / screenSize;

    float depth = GetDepth(fragTexCoord);
    float d1 = GetDepth(fragTexCoord + vec2(texel.x, 0.0));
    float d2 = GetDepth(fragTexCoord + vec2(-texel.x, 0.0));
    float d3 = GetDepth(fragTexCoord + vec2(0.0, texel.y));
    float d4 = GetDepth(fragTexCoord + vec2(0.0, -texel.y));

    float diff = 0.0;
    diff += abs(depth - d1);
    diff += abs(depth - d2);
    diff += abs(depth - d3);
    diff += abs(depth - d4);

    if (diff > edgeThreshold)
        finalColor = outlineColor;
    else
        finalColor = texture(texture0, fragTexCoord);
}