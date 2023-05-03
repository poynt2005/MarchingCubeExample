#include "Drawler.h"

#include <GLFW/glfw3.h>
#include <GL/freeglut.h>
#include <math.h>

Drawler::Drawler()
{
    Initialize();
}

Drawler::Drawler(const std::vector<Triangle> &inputTri)
    : tri(inputTri)
{
    Initialize();
}
Drawler::~Drawler()
{
    glfwDestroyWindow(window);
    glfwTerminate();
    window = nullptr;
}

void Drawler::Initialize()
{
    glfwInit();
    window = glfwCreateWindow(800, 800, "OpenGL Marching Cube Example", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *_win, int _width, int _height) -> void
                                   { glViewport(0, 0, _width, _height); });
    SetColor(0.2, 0.5, 0.3);

    FlatternVertex(tri, triNdc);
}

void Drawler::CalculateNorm()
{
    norm.clear();

    for (const auto &m : tri)
    {
        fPoint vector0{
            m.v1.x - m.v0.x,
            m.v1.y - m.v0.y,
            m.v1.z - m.v0.z,
        };

        fPoint vector1{
            m.v2.x - m.v0.x,
            m.v2.y - m.v0.y,
            m.v2.z - m.v0.z,
        };

        fPoint outerProduct{
            vector0.y * vector1.z - vector0.z * vector1.y,
            vector0.z * vector1.x - vector0.x * vector1.z,
            vector0.x * vector1.y - vector0.y * vector1.x};

        float distance = std::sqrt(outerProduct.x * outerProduct.x + outerProduct.y * outerProduct.y + outerProduct.z * outerProduct.z);

        fPoint normVec{
            outerProduct.x / distance,
            outerProduct.y / distance,
            outerProduct.z / distance};

        // v0 norm
        norm.emplace_back(normVec.x);
        norm.emplace_back(normVec.y);
        norm.emplace_back(normVec.z);

        // v1 norm
        norm.emplace_back(normVec.x);
        norm.emplace_back(normVec.y);
        norm.emplace_back(normVec.z);

        // v2 norm
        norm.emplace_back(normVec.x);
        norm.emplace_back(normVec.y);
        norm.emplace_back(normVec.z);
    }
}

bool Drawler::CheckIsClose() const
{
    return static_cast<bool>(glfwWindowShouldClose(window));
}

void Drawler::CalculateColor()
{
    triColor.clear();

    /** Set Vertex Color*/
    for (const auto &v : tri)
    {
        // v0 point color
        triColor.emplace_back(vertexColor.r);
        triColor.emplace_back(vertexColor.g);
        triColor.emplace_back(vertexColor.b);

        // v1 point color
        triColor.emplace_back(vertexColor.r);
        triColor.emplace_back(vertexColor.g);
        triColor.emplace_back(vertexColor.b);

        // v2 point color
        triColor.emplace_back(vertexColor.r);
        triColor.emplace_back(vertexColor.g);
        triColor.emplace_back(vertexColor.b);
    }
}

void Drawler::FlatternVertex(const std::vector<Triangle> &tris, std::vector<float> &flatterned)
{
    flatterned.clear();

    for (const auto &v : tris)
    {
        // v0 point
        flatterned.emplace_back(v.v0.x);
        flatterned.emplace_back(v.v0.y);
        flatterned.emplace_back(v.v0.z);

        // v1 point
        flatterned.emplace_back(v.v1.x);
        flatterned.emplace_back(v.v1.y);
        flatterned.emplace_back(v.v1.z);

        // v2 point
        flatterned.emplace_back(v.v2.x);
        flatterned.emplace_back(v.v2.y);
        flatterned.emplace_back(v.v2.z);
    }
}

/** Setters */

void Drawler::SetColor(const float r, const float g, const float b)
{
    vertexColor = Color3{r, g, b};
    CalculateColor();
}

void Drawler::SetMesh(const std::vector<Triangle> &inputTri)
{
    tri.clear();
    norm.clear();
    tri = std::vector<Triangle>(inputTri.begin(), inputTri.end());

    CalculateColor();
    FlatternVertex(tri, triNdc);
}

void Drawler::SetRotate(const float angle, const char axis)
{
    rotate.angle = angle;
    rotate.axis = axis;
}

void Drawler::SetTranslate(const float x, const float y, const float z)
{
    translate.x = x;
    translate.y = y;
    translate.z = z;
}

void Drawler::SetLightAmbiet(const Color3 &inputColor)
{
    lightAmbietColor = Color3{
        inputColor.r,
        inputColor.g,
        inputColor.b};
}
void Drawler::SetLightDiffuse(const Color3 &inputColor)
{
    lightDiffuseColor = Color3{
        inputColor.r,
        inputColor.g,
        inputColor.b};
}
void Drawler::SetLightPosition(const fPoint &inputPos)
{
    lightPosition = fPoint{
        inputPos.x,
        inputPos.y,
        inputPos.z};
}

void Drawler::SetMaterialAmbiet(const Color3 &inputColor)
{
    materialAmbietColor = Color3{
        inputColor.r,
        inputColor.g,
        inputColor.b};
}
void Drawler::SetMaterialDiffuse(const Color3 &inputColor)
{
    materialDiffuseColor = Color3{
        inputColor.r,
        inputColor.g,
        inputColor.b};
}

/** Renderers */
void Drawler::GLDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightPositionArr[] = {lightPosition.x, lightPosition.y, lightPosition.z, 0.0};
    GLfloat lightDiffuseArr[] = {lightDiffuseColor.r, lightDiffuseColor.g, lightDiffuseColor.b, 1.0};
    GLfloat lightAmbietArr[] = {lightDiffuseColor.r, lightDiffuseColor.g, lightDiffuseColor.b, 1.0};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPositionArr);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuseArr);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbietArr);

    GLfloat materialAmbietArr[] = {materialAmbietColor.r, materialAmbietColor.g, materialAmbietColor.b, 1.0};
    GLfloat materialDiffuseArr[] = {materialDiffuseColor.r, materialDiffuseColor.g, materialDiffuseColor.b, 1.0};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuseArr);
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbietArr);

    glTranslatef(translate.x, translate.y, translate.z);

    if (rotate.angle != 0)
    {
        if (rotate.axis == 'x')
        {
            glRotatef(rotate.angle, 1, 0, 0);
        }
        else if (rotate.axis == 'y')
        {
            glRotatef(rotate.angle, 0, 1, 0);
        }
        else if (rotate.axis == 'z')
        {
            glRotatef(rotate.angle, 0, 0, 1);
        }
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, 0, triNdc.data());

    if (!norm.empty())
    {

        glNormalPointer(GL_FLOAT, 0, norm.data());
    }

    glColorPointer(3, GL_FLOAT, 0, triColor.data());

    glDrawArrays(GL_TRIANGLES, 0, triNdc.size() / 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void Drawler::GLReshape()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const GLfloat aspectRatio = width / height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspectRatio, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
}

void Drawler::RenderFrame()
{
    GLReshape();
    GLDisplay();
    glfwSwapBuffers(window);
    glfwPollEvents();
}