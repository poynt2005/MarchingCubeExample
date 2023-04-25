#include "Table.h"
#include "DrawlerAPI.h"
#include "Drawler.h"

#include <unordered_map>
#include <memory>
#include <string.h>
#include <vector>

std::unordered_map<DRHandle, std::unique_ptr<Drawler>> instanceMapping;

DRHandle CreateDrawlerInstance(const Triangle *tri, const unsigned int facesCount)
{
    std::vector<Triangle> triVec(facesCount);
    memcpy(triVec.data(), tri, facesCount * sizeof(Triangle));

    auto instance = std::make_unique<Drawler>(triVec);
    const DRHandle handle = reinterpret_cast<DRHandle>(instance.get());

    instanceMapping.insert(std::pair<DRHandle, std::unique_ptr<Drawler>>(handle, std::move(instance)));

    return handle;
}

void ReleaseDrawlerInstance(const DRHandle handle)
{
    instanceMapping.erase(handle);
}

int CheckIsDrawlerInstanceExists(const DRHandle handle)
{
    return instanceMapping.count(handle);
}

void SetMesh(const DRHandle handle, const Triangle *tri, const unsigned int facesCount)
{
    std::vector<Triangle> triVec(facesCount);
    memcpy(triVec.data(), tri, facesCount * sizeof(Triangle));
    instanceMapping[handle]->SetMesh(triVec);
}

void SetRotate(const DRHandle handle, const float angle, const char axis)
{
    instanceMapping[handle]->SetRotate(angle, axis);
}

void SetColor(const DRHandle handle, const float r, const float g, const float b)
{
    instanceMapping[handle]->SetColor(r, g, b);
}
void RenderFrame(const DRHandle handle)
{
    instanceMapping[handle]->RenderFrame();
}

void CalculateNorm(const DRHandle handle)
{
    instanceMapping[handle]->CalculateNorm();
}

void SetLightAmbiet(const DRHandle handle, const Color3 *color)
{
    instanceMapping[handle]->SetLightAmbiet(*color);
}
void SetLightDiffuse(const DRHandle handle, const Color3 *color)
{
    instanceMapping[handle]->SetLightDiffuse(*color);
}
void SetLightPosition(const DRHandle handle, const fPoint *pos)
{
    instanceMapping[handle]->SetLightPosition(*pos);
}
void SetMaterialAmbiet(const DRHandle handle, const Color3 *color)
{
    instanceMapping[handle]->SetMaterialAmbiet(*color);
}
void SetMaterialDiffuse(const DRHandle handle, const Color3 *color)
{
    instanceMapping[handle]->SetMaterialDiffuse(*color);
}

int CheckIsClose(const DRHandle handle)
{
    return static_cast<int>(instanceMapping[handle]->CheckIsClose());
}
