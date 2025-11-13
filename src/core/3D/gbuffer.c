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
    target.m_id = rlLoadFramebuffer();

    rlEnableFramebuffer(target.m_id);

    target.m_color.id = rlLoadTexture(NULL, _width, _height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    target.m_color.width = _width;
    target.m_color.height = _height;
    target.m_color.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    target.m_color.mipmaps = 1;
    rlFramebufferAttach(target.m_id, target.m_color.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

    target.m_normal.id = rlLoadTexture(NULL, _width, _height, PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1);
    target.m_normal.width = _width;
    target.m_normal.height = _height;
    target.m_normal.format = PIXELFORMAT_UNCOMPRESSED_R16G16B16A16;
    target.m_normal.mipmaps = 1;
    rlFramebufferAttach(target.m_id, target.m_normal.id, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);

    target.m_depth.id = rlLoadTextureDepth(_width, _height, false);
    target.m_depth.width = _width;
    target.m_depth.height = _height;
    target.m_depth.format = 19;       //DEPTH_COMPONENT_24BIT?
    target.m_depth.mipmaps = 1;
    rlFramebufferAttach(target.m_id, target.m_depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_TEXTURE2D, 0);

    rlDisableFramebuffer();

    return target;
}

void UnloadGBuffer(GBuffer _target)
{
    if (_target.m_id > 0)
    {
        rlUnloadFramebuffer(_target.m_id);
    }
}

void BeginGBuffer(GBuffer _target, Camera3D _camera)
{
    rlDrawRenderBatchActive();      // Update and draw internal render batch

    rlEnableFramebuffer(_target.m_id); // Enable render target
    rlActiveDrawBuffers(2);

    // Set viewport and RLGL internal framebuffer size
    rlViewport(0, 0, _target.m_color.width, _target.m_color.height);
    rlSetFramebufferWidth(_target.m_color.width);
    rlSetFramebufferHeight(_target.m_color.height);

    ClearBackground(BLACK);

    rlMatrixMode(RL_PROJECTION);    // Switch to projection matrix
    rlPushMatrix();                 // Save previous matrix, which contains the settings for the 2d ortho projection
    rlLoadIdentity();               // Reset current matrix (projection)

    f32 aspect = (f32)_target.m_color.width / (f32)_target.m_color.height;

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