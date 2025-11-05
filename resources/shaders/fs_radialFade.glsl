#version 330

in vec4 fragColor;
in vec3 fragPosition;

out vec4 finalColor;

uniform vec3 center;
uniform vec3 cameraPos;
uniform float distScale;

float easeInExpo(float x)
{
    return x == 0.0 ? 0.0 : pow(2.0, 10. * x - 10.0);
}

float easeOutQuart(float x)
{
    return 1.0 - pow(1.0 - x, 4.0);
}

float easeInCirc(float x)
{
    return 1.0 - sqrt(1.0 - pow(x, 2.0));
}

void main() 
{
    float distToCenter = distance(cameraPos, center);
    float globalAlpha = 1.0 / (1.0 + easeInExpo(distToCenter / distScale));

    float distFragToCenter = distance(fragPosition, center);
    float alphaToCenter = 1.0 / (1.0 + easeInCirc(distFragToCenter / distScale));

    float alpha = alphaToCenter * globalAlpha * fragColor.a;
    float r = alphaToCenter * fragColor.r;
    float g = alphaToCenter * fragColor.g;
    float b = alphaToCenter * fragColor.b;

    finalColor = vec4(r, g, b, alpha);
}