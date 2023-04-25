#ifndef __MARCHING_CUBE_DRAWLER_API_H__
#define __MARCHING_CUBE_DRAWLER_API_H__

#include "Types.h"

typedef unsigned long long DRHandle;

#ifdef BUILDDRAPI
#define EXPORTDRAPI __declspec(dllexport)
#else
#define EXPORTDRAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /** Drawler API*/
    EXPORTDRAPI DRHandle CreateDrawlerInstance(const Triangle *, const unsigned int);
    EXPORTDRAPI void ReleaseDrawlerInstance(const DRHandle);
    EXPORTDRAPI int CheckIsDrawlerInstanceExists(const DRHandle);

    EXPORTDRAPI void SetMesh(const DRHandle, const Triangle *, const unsigned int);
    EXPORTDRAPI void SetRotate(const DRHandle, const float, const char);
    EXPORTDRAPI void SetColor(const DRHandle, const float, const float, const float);

    EXPORTDRAPI void SetLightAmbiet(const DRHandle, const Color3 *);
    EXPORTDRAPI void SetLightDiffuse(const DRHandle, const Color3 *);
    EXPORTDRAPI void SetLightPosition(const DRHandle, const fPoint *);
    EXPORTDRAPI void SetMaterialAmbiet(const DRHandle, const Color3 *);
    EXPORTDRAPI void SetMaterialDiffuse(const DRHandle, const Color3 *);

    EXPORTDRAPI void CalculateNorm(const DRHandle);
    EXPORTDRAPI void RenderFrame(const DRHandle);
    EXPORTDRAPI int CheckIsClose(const DRHandle);

#ifdef __cplusplus
}
#endif

#endif