#pragma once

#include "../raymath.h"
#include "../raylib.h"
#include "../rlgl.h"
#include <assert.h>

typedef struct ShadowLight
{
    Vector3 target;
    Vector3 position;
    Vector3 up;
    float width;
    float height;
    float near;
    float far;

} ShadowLight;

void ShadowLightInit(ShadowLight* _shadowLight)
{
    Vector3 lightDir = Vector3Normalize((Vector3) { 0.35f, -1.0f, -0.35f });

    ShadowLight shadowLight = (ShadowLight){ 0 };
    shadowLight.target = Vector3Zero();
    shadowLight.position = Vector3Scale(lightDir, -5.0f);
    shadowLight.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    shadowLight.width = 5.0f;
    shadowLight.height = 5.0f;
    shadowLight.near = 0.01f;
    shadowLight.far = 10.0f;

    shadowLight.target = (Vector3){ 0.f, 0.0f, 0.f };
    shadowLight.position = Vector3Add(shadowLight.target, Vector3Scale(lightDir, -5.0f));
}

RenderTexture2D LoadShadowMap(const i32 _width, const i32 _height)
{
    RenderTexture2D target = { 0 };
    target.id = rlLoadFramebuffer();
    target.texture.width = _width;
    target.texture.height = _height;
    assert(target.id);

    rlEnableFramebuffer(target.id);

    target.depth.id = rlLoadTextureDepth(_width, _height, false);
    target.depth.width = _width;
    target.depth.height = _height;
    target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
    target.depth.mipmaps = 1;
    rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);
    assert(rlFramebufferComplete(target.id));

    rlDisableFramebuffer();

    return target;
}

void UnloadShadowMap(RenderTexture2D target)
{
    if (target.id > 0)
    {
        rlUnloadFramebuffer(target.id);
    }
}

void BeginShadowMap(RenderTexture2D target, ShadowLight shadowLight)
{
    BeginTextureMode(target);
    ClearBackground(WHITE);

    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();               // Reset current matrix (projection)

    rlOrtho(
        -shadowLight.width / 2, shadowLight.width / 2,
        -shadowLight.height / 2, shadowLight.height / 2,
        shadowLight.near, shadowLight.far);

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    // Setup Camera view
    Matrix matView = MatrixLookAt(shadowLight.position, shadowLight.target, shadowLight.up);
    rlMultMatrixf(MatrixToFloat(matView));      // Multiply modelview matrix by view matrix (camera)

    rlEnableDepthTest();            // Enable DEPTH_TEST for 3D    
}

void EndShadowMap()
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPopMatrix();                  // Restore previous matrix (projection) from matrix stack

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    rlDisableDepthTest();           // Disable DEPTH_TEST for 2D

    EndTextureMode();
}

void SetShaderValueShadowMap(Shader shader, int locIndex, RenderTexture2D target)
{
    if (locIndex <= -1)
        return;

    
    rlEnableShader(shader.id);
    int slot = 15; // Can be anything 0 to 15, but 0 will probably be taken up
    rlActiveTextureSlot(slot);
    rlEnableTexture(target.depth.id);
    rlSetUniform(locIndex, &slot, SHADER_UNIFORM_INT, 1);
}