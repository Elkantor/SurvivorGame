#pragma once

#include "../types.c"
#include "../rlgl.h"
#include "../raylib.h"
#include "../raymath.h"
#include "./utils3D.c"

Image BlobShadowGenImage(const i32 _size, const f32 _hardness)
{
    Image img = GenImageColor(_size, _size, BLANK);
    i32 center = _size / 2;

    for (i32 y = 0; y < _size; ++y)
    {
        for (i32 x = 0; x < _size; ++x) 
        {
            const f32 dx = (f32)(x - center);
            const f32 dy = (f32)(y - center);
            const f32 dist = sqrtf(dx * dx + dy * dy) / (float)center;
            const f32 alpha = 1.0f - powf(Utils3DSmoothstepf32(0.0f, _hardness, dist), 3.0f); // pow for blurring it
            const Color pix = { 0, 0, 0, (u8)(alpha * 255.0f) };
            ImageDrawPixel(&img, x, y, pix);
        }
    }
    return img;
}

Vector3 BlobShadowComputePos(Vector3 _objPos, Vector3 _lightDir, f32 _groundLevel)
{
    /*const Vector3 dir = Vector3Normalize(_lightDir);
    const f32 t = (_groundLevel - _objPos.y) / dir.y;

    if (dir.y >= 0.0f || t < 0.0f) 
    {
        return _objPos;
    }

    return Vector3Add(_objPos, Vector3Scale(dir, t));*/

    return _objPos;
}

void BlobShadowRender(
    Vector3 _objPositions, f32 _objSize,
    const Camera3D _camera, Vector3 _lightDir,
    Texture2D _blobTexture, f32 _groundLevel,
    f32 _liftHeight  // to avoid z fighting
) 
{
    rlDisableDepthMask();
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    {
        Vector3 shadowPos = BlobShadowComputePos(_objPositions, _lightDir, _groundLevel);
        //shadowPos.y += _liftHeight;

        const f32 shadowScale = _objSize * 2.5f;
        const Color shadowColor = (Color){ 0, 0, 0, 150 };

        DrawBillboard(_camera, _blobTexture, shadowPos, shadowScale, shadowColor);
    }
    EndBlendMode();
    rlEnableDepthMask();
}