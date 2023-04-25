#ifndef __MARCHING_CUBE_API_H__
#define __MARCHING_CUBE_API_H__

#include "Types.h"

typedef unsigned long long MCHandle;

#ifdef BUILDMCAPI
#define EXPORTMCAPI __declspec(dllexport)
#else
#define EXPORTMCAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /** MarchingCubes API*/
    EXPORTMCAPI MCHandle CreateMarchingCubeInstance(const char *, const Dimension *);
    EXPORTMCAPI MCHandle CreateMarchingCubeInstanceFromBuffer(const char *, const int, const Dimension *);
    EXPORTMCAPI void ReleaseMarchingCubeInstance(const MCHandle);
    EXPORTMCAPI int CheckIsMCInstanceExists(const MCHandle);

    EXPORTMCAPI void March(const MCHandle, const unsigned int);
    EXPORTMCAPI void DefaultMarch(const MCHandle);

    EXPORTMCAPI void GetCurrentMesh(const MCHandle, Triangle **, unsigned int *);
    EXPORTMCAPI void GetCurrentMeshNormalized(const MCHandle, Triangle **, unsigned int *);
    EXPORTMCAPI void GetMeshNormal(const Triangle *, const unsigned, fPoint **, unsigned int *);

    EXPORTMCAPI void ReleaseCurrentMesh(Triangle **);
    EXPORTMCAPI void ReleaseCurrentPoint(fPoint **);

    EXPORTMCAPI void WriteCurrentMeshToObj(const MCHandle, const char *);
    EXPORTMCAPI int ParseFileName(const char *, Dimension *);
#ifdef __cplusplus
}
#endif

#endif