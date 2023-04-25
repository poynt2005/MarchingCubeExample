#include "MarchingCubeAPI.h"
#include <filesystem>
#include <memory>
#include <vector>
#include <string.h>
#include "MarchingCube.h"

#include <unordered_map>

std::unordered_map<MCHandle, std::unique_ptr<MarchingCube>> instanceMapping;

MCHandle CreateMarchingCubeInstance(const char *filename, const Dimension *dimension)
{
    if (!std::filesystem::exists(filename))
    {
        return 0;
    }

    auto instance = std::make_unique<MarchingCube>(filename, *dimension);
    auto handle = reinterpret_cast<MCHandle>(instance.get());

    instanceMapping.insert(std::pair<MCHandle, std::unique_ptr<MarchingCube>>(
        handle,
        std::move(instance)));

    return handle;
}

MCHandle CreateMarchingCubeInstanceFromBuffer(const char *inputBuf, const int bufSize, const Dimension *dimension)
{
    std::vector<uint8_t> buf(dimension->width * dimension->height * dimension->depth);
    memcpy(buf.data(), inputBuf, bufSize);

    auto instance = std::make_unique<MarchingCube>(buf, *dimension);
    auto handle = reinterpret_cast<MCHandle>(instance.get());

    instanceMapping.insert(std::pair<MCHandle, std::unique_ptr<MarchingCube>>(
        handle,
        std::move(instance)));

    return handle;
}

int CheckIsMCInstanceExists(const MCHandle handle)
{
    return instanceMapping.count(handle);
}

void ReleaseMarchingCubeInstance(const MCHandle handle)
{
    instanceMapping.erase(handle);
}

void March(const MCHandle handle, const unsigned int isoSurface)
{
    instanceMapping[handle]->March(static_cast<uint8_t>(isoSurface));
}

void DefaultMarch(const MCHandle handle)
{
    instanceMapping[handle]->March();
}

void GetCurrentMesh(const MCHandle handle, Triangle **triangleArr, unsigned int *faces)
{
    std::vector<Triangle> triangeVec;
    instanceMapping[handle]->GetCurrentMesh(triangeVec);
    *triangleArr = new Triangle[triangeVec.size()];
    memcpy(*triangleArr, triangeVec.data(), sizeof(Triangle) * triangeVec.size());
    *faces = static_cast<unsigned int>(triangeVec.size());
}

void GetCurrentMeshNormalized(const MCHandle handle, Triangle **triangleArr, unsigned int *faces)
{
    std::vector<Triangle> triangeVec;
    instanceMapping[handle]->GetCurrentMeshNormalized(triangeVec);
    *triangleArr = new Triangle[triangeVec.size()];
    memcpy(*triangleArr, triangeVec.data(), sizeof(Triangle) * triangeVec.size());
    *faces = static_cast<unsigned int>(triangeVec.size());
}

void GetMeshNormal(const Triangle *inTri, const unsigned facesCount, fPoint **outNorm, unsigned int *normCount)
{
    std::vector<Triangle> triangeVec(facesCount);
    memcpy(triangeVec.data(), inTri, facesCount * sizeof(Triangle));

    std::vector<fPoint> norms;
    MarchingCube::GetMeshNormal(triangeVec, norms);

    *outNorm = new fPoint[norms.size()];
    memcpy(*outNorm, norms.data(), norms.size() * sizeof(fPoint));
    *normCount = static_cast<unsigned int>(norms.size());
}

void ReleaseCurrentPoint(fPoint **inPoi)
{
    delete[] (*inPoi);
    *inPoi = nullptr;
}

void ReleaseCurrentMesh(Triangle **triangleArr)
{
    delete[] (*triangleArr);
    *triangleArr = nullptr;
}

void WriteCurrentMeshToObj(const MCHandle handle, const char *filename)
{
    instanceMapping[handle]->WriteCurrentMeshToObj(filename);
}

int ParseFileName(const char *filename, Dimension *dimension)
{
    return static_cast<int>(MarchingCube::ParseFileName(filename, *dimension));
}