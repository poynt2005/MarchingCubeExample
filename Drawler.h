#ifndef __MARCHING_CUBE_DRAWLER_H__
#define __MARCHING_CUBE_DRAWLER_H__

#include "Types.h"

#include <vector>

using GLFWwindow = struct GLFWwindow;

class Drawler
{
public:
    Drawler();
    Drawler(const std::vector<Triangle> &);
    ~Drawler();

    void SetRotate(const float, const char);
    void SetTranslate(const float, const float, const float);
    void SetColor(const float, const float, const float);

    void SetLightAmbiet(const Color3 &);
    void SetLightDiffuse(const Color3 &);
    void SetLightPosition(const fPoint &);

    void SetMaterialAmbiet(const Color3 &);
    void SetMaterialDiffuse(const Color3 &);

    void SetMesh(const std::vector<Triangle> &);

    bool CheckIsClose() const;

    void CalculateNorm();
    void RenderFrame();

private:
    using RotateData = struct rotateData
    {
        float angle;
        char axis;
    };

    using TranslateData = struct translateData
    {
        float x;
        float y;
        float z;
    };

    GLFWwindow *window;

    std::vector<Triangle> tri;

    std::vector<float> norm;
    std::vector<float> triColor;
    std::vector<float> triNdc;

    Color3 vertexColor{0.2, 0.5, 0.3};

    fPoint lightPosition{1.0, 1.0, 1.0};
    Color3 lightDiffuseColor{1.0, 1.0, 1.0};
    Color3 lightAmbietColor{1.0, 1.0, 1.0};

    Color3 materialDiffuseColor{1.0, 1.0, 1.0};
    Color3 materialAmbietColor{1.0, 1.0, 1.0};

    RotateData rotate = RotateData{0, 'x'};
    TranslateData translate = TranslateData{0.0, 0.0, 0.0};

    void Initialize();

    void GLDisplay();
    void GLReshape();

    void CalculateColor();
    static inline void FlatternVertex(const std::vector<Triangle> &, std::vector<float> &);
};

#endif