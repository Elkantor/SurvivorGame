#pragma once

#include "../raymath.h"
#include "../raylib.h"
#include "../rlgl.h"
#include "../types.c"

typedef struct GBuffer
{
    u32 m_id;           // OpenGL framebuffer object id
    Texture m_color;    // Color buffer attachment texture
    Texture m_normal;   // Normal buffer attachment texture
    Texture m_depth;    // Depth buffer attachment texture
} GBuffer;

GBuffer LoadGBuffer(const i32 _width, const i32 _height)
{
    GBuffer target = { 0 };
    target.id = rlLoadFramebuffer();

    rlEnableFramebuffer(target.id);

    target.color.id = rlLoadTexture(NULL, _width, _height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    target.color.width = width;
    target.color.height = height;
    target.color.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.color.mipmaps = 1;
    rlFramebufferAttach(target.id, target.color.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

    target.normal.id = rlLoadTexture(NULL, _width, _height, PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1);
    target.normal.width = width;
    target.normal.height = height;
    target.normal.format = PIXELFORMAT_UNCOMPRESSED_R16G16B16A16;
    target.normal.mipmaps = 1;
    rlFramebufferAttach(target.id, target.normal.id, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);

    target.depth.id = rlLoadTextureDepth(_width, _height, false);
    target.depth.width = _width;
    target.depth.height = height;
    target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
    target.depth.mipmaps = 1;
    rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    rlDisableFramebuffer();

    return target;
}

void UnloadGBuffer(GBuffer _target)
{
    if (_target.id > 0)
    {
        rlUnloadFramebuffer(_target.id);
    }
}

void BeginGBuffer(GBuffer _target, Camera3D _camera)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlEnableFramebuffer(_target.id); // Enable render target
    rlActiveDrawBuffers(2);

    // Set viewport and RLGL internal framebuffer size
    rlViewport(0, 0, _target.color.width, _target.color.height);
    rlSetFramebufferWidth(_target.color.width);
    rlSetFramebufferHeight(_target.color.height);

    ClearBackground(BLACK);

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();               // Reset current matrix (projection)

    f32 aspect = (f32)_target.color.width / (f32)_target.color.height;

    // NOTE: zNear and zFar values are important when computing depth buffer values
    if (_camera.projection == CAMERA_PERSPECTIVE)
    {
        // Setup perspective projection
        f64 top = rlGetCullDistanceNear() * tan(_camera.fovy * 0.5 * DEG2RAD);
        f64 right = top * aspect;

        rlFrustum(-right, right, -top, top, rlGetCullDistanceNear(), rlGetCullDistanceFar());
    }
    else if (_camera.projection == CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic projection
        f64 top = _camera.fovy / 2.0;
        f64 right = top * aspect;

        rlOrtho(-right, right, -top, top, rlGetCullDistanceNear(), rlGetCullDistanceFar());
    }

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    // Setup Camera view
    Matrix matView = MatrixLookAt(_camera.position, _camera.target, _camera.up);
    rlMultMatrixf(MatrixToFloat(matView));      // Multiply modelview matrix by view matrix (camera)

    rlEnableDepthTest();            // Enable DEPTH_TEST for 3D
}

void EndGBuffer(const i32 _windowWidth, const i32 _windowHeight)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlDisableDepthTest();           // Disable DEPTH_TEST for 2D
    rlActiveDrawBuffers(1);
    rlDisableFramebuffer();         // Disable render target (fbo)

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPopMatrix();                  // Restore previous matrix (projection) from matrix stack
    rlLoadIdentity();               // Reset current matrix (projection)
    rlOrtho(0, _windowWidth, _windowHeight, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);         // Switch back to modelview matrix
    rlLoadIdentity();                   // Reset current matrix (modelview)
}