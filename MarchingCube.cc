#include "MarchingCube.h"
#include "Table.h"

#include <limits>
#include <filesystem>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <regex>
#include <sstream>

MarchingCube::MarchingCube(const std::string &filename, const Dimension &dimension)
    : rawDimension(dimension)
{
    std::ifstream inFile(std::filesystem::absolute(filename).string(), std::ios::binary);
    inFile.seekg(0, std::ios::end);
    const auto filesize = inFile.tellg();
    inFile.seekg(std::ios::beg);
    rawBuffer.resize(dimension.width * dimension.height * dimension.depth);
    inFile.read(reinterpret_cast<char *>(rawBuffer.data()), filesize);
    inFile.close();
}

MarchingCube::MarchingCube(const std::vector<uint8_t> &buf, const Dimension &dimension)
    : rawBuffer(buf), rawDimension(dimension)
{
}

MarchingCube::~MarchingCube() {}

void MarchingCube::March(const unsigned int inputIsoSurface)
{
    currentIsoSurface = inputIsoSurface;
    currentMesh.clear();
    currentBoundingBox =
        {
            fPoint{
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min(),
                std::numeric_limits<float>::min()},
            fPoint{
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()}};

    for (unsigned int i = 0; i < rawDimension.width - 1; ++i)
    {
        for (unsigned int j = 0; j < rawDimension.height - 1; ++j)
        {
            for (unsigned int k = 0; k < rawDimension.depth - 1; ++k)
            {
                CalculateMesh(i, j, k);
            }
        }
    }
}

/** Default isosurface*/
void MarchingCube::March()
{
    March(DEFAULT_ISOSURFACE);
}

void MarchingCube::CalculateMesh(const unsigned int x, const unsigned int y, const unsigned int z)
{
    /**
     * Iterate over 8 points of a cube
     * and compare with the isosurface,
     * check whether a surface is outside or inside the points of a cube
     *
     * cubeVertices => The Value of each 8 vertices
     */
    std::vector<unsigned int> cubeVerticesValue;
    cubeVerticesValue.resize(8);

    for (int i = 0; i < 8; ++i)
    {
        cubeVerticesValue[i] = GetPointData(
            /** The x offset from Raw file to cube width*/
            x + Table::cubeVertices[i][0],

            /** The y offset from Raw file to cube height */
            y + Table::cubeVertices[i][1],

            /** The z offset from Raw file to cube depth */
            z + Table::cubeVertices[i][2]);
    }

    unsigned int cubeIndex = 0;
    for (int i = 0; i < 8; ++i)
    {
        /**
         * If a point is inside the mesh, set the bit of this point to 1
         * otherwise, set to 0
         * same to
         *  int cubeIndex = 0;
            if (cubeValues[0] < isolevel) cubeIndex |= 1;
            if (cubeValues[1] < isolevel) cubeIndex |= 2;
            if (cubeValues[2] < isolevel) cubeIndex |= 4;
            if (cubeValues[3] < isolevel) cubeIndex |= 8;
            if (cubeValues[4] < isolevel) cubeIndex |= 16;
            if (cubeValues[5] < isolevel) cubeIndex |= 32;
            if (cubeValues[6] < isolevel) cubeIndex |= 64;
            if (cubeValues[7] < isolevel) cubeIndex |= 128;
         * */
        if (cubeVerticesValue[i] < currentIsoSurface)
        {
            cubeIndex |= (1 << (i));
        }
    }
    /** Find which edges of the cube that mesh cut*/
    unsigned int edges = Table::edgeTable[cubeIndex];

    /** store the corss point of the edge of a cube*/
    std::vector<fPoint> edgeCrossVerteces(12);

    /** Iterate over this 12 bits*/
    for (int i = 0; i < 12; i++)
    {
        /** Bitwise "AND" these 12 bits*/
        if (edges & (1 << i))
        {
            /** Calculate p1 (from raw data point to cube point)*/
            uPoint p1{
                Table::cubeVertices[Table::cubeEdges[i][0]][0] + x,
                Table::cubeVertices[Table::cubeEdges[i][0]][1] + y,
                Table::cubeVertices[Table::cubeEdges[i][0]][2] + z,
            };

            uPoint p2{
                Table::cubeVertices[Table::cubeEdges[i][1]][0] + x,
                Table::cubeVertices[Table::cubeEdges[i][1]][1] + y,
                Table::cubeVertices[Table::cubeEdges[i][1]][2] + z,
            };

            fPoint interpResult;
            /** Interpolate
             * p1 = vertex 1 from raw data to cube index 1
             * p2 = vertex 2 from raw data to cube index 2
             * p1Val = value of p1
             * p2Val = value of p2
             */
            VertexInterpolate(
                p1,
                p2,
                cubeVerticesValue[Table::cubeEdges[i][0]],
                cubeVerticesValue[Table::cubeEdges[i][1]],
                interpResult);
            CalculBounding(interpResult);
            edgeCrossVerteces[i] = std::move(interpResult);
        }
    }
    /**
     * find connected triangle from given connectaion table
     * iterate til the value is equal to -1
     * The point is in times of 3 and will stop when it reaches -1.
     * The number of this edge will be stored in the triTable.
     * So
     *  i   => v0 of the triangle
     *  i+1 => v1 of the triangle
     *  i+2 => v2 of the triangle
     *
     * The array of triangles repersenting Mesh
     * */
    for (int i = 0; Table::triTable[cubeIndex][i] != -1; i += 3)
    {
        Triangle tri{
            /** Triangle v0 vertex*/
            fPoint{
                edgeCrossVerteces[Table::triTable[cubeIndex][i]].x,
                edgeCrossVerteces[Table::triTable[cubeIndex][i]].y,
                edgeCrossVerteces[Table::triTable[cubeIndex][i]].z},
            /** Triangle v1 vertex*/
            fPoint{
                edgeCrossVerteces[Table::triTable[cubeIndex][i + 1]].x,
                edgeCrossVerteces[Table::triTable[cubeIndex][i + 1]].y,
                edgeCrossVerteces[Table::triTable[cubeIndex][i + 1]].z},
            /** Triangle v2 vertex*/
            fPoint{
                edgeCrossVerteces[Table::triTable[cubeIndex][i + 2]].x,
                edgeCrossVerteces[Table::triTable[cubeIndex][i + 2]].y,
                edgeCrossVerteces[Table::triTable[cubeIndex][i + 2]].z}};

        currentMesh.emplace_back(std::move(tri));
    }
}

void MarchingCube::GetCurrentMesh(std::vector<Triangle> &outMesh) const
{
    outMesh = std::vector<Triangle>(currentMesh.begin(), currentMesh.end());
}

void MarchingCube::GetCurrentMeshNormalized(std::vector<Triangle> &outMesh) const
{
    outMesh.resize(currentMesh.size());

    for (int i = 0; i < currentMesh.size(); i++)
    {
        outMesh[i] = {
            fPoint{
                2 * (currentMesh[i].v0.x - currentBoundingBox[1].x) / (currentBoundingBox[0].x - currentBoundingBox[1].x) - 1,
                2 * (currentMesh[i].v0.y - currentBoundingBox[1].y) / (currentBoundingBox[0].y - currentBoundingBox[1].y) - 1,
                2 * (currentMesh[i].v0.z - currentBoundingBox[1].z) / (currentBoundingBox[0].z - currentBoundingBox[1].z) - 1},
            fPoint{
                2 * (currentMesh[i].v1.x - currentBoundingBox[1].x) / (currentBoundingBox[0].x - currentBoundingBox[1].x) - 1,
                2 * (currentMesh[i].v1.y - currentBoundingBox[1].y) / (currentBoundingBox[0].y - currentBoundingBox[1].y) - 1,
                2 * (currentMesh[i].v1.z - currentBoundingBox[1].z) / (currentBoundingBox[0].z - currentBoundingBox[1].z) - 1

            },
            fPoint{
                2 * (currentMesh[i].v2.x - currentBoundingBox[1].x) / (currentBoundingBox[0].x - currentBoundingBox[1].x) - 1,
                2 * (currentMesh[i].v2.y - currentBoundingBox[1].y) / (currentBoundingBox[0].y - currentBoundingBox[1].y) - 1,
                2 * (currentMesh[i].v2.z - currentBoundingBox[1].z) / (currentBoundingBox[0].z - currentBoundingBox[1].z) - 1}};
    }
}

void MarchingCube::GetCurrentBoundingBox(fPoint &max, fPoint &min) const
{
    max = currentBoundingBox[0];
    min = currentBoundingBox[1];
}

unsigned int MarchingCube::GetPointData(const unsigned int width, const unsigned int height, const unsigned int depth) const
{
    const auto index = depth * rawDimension.height * rawDimension.width + height * rawDimension.width + width;
    return static_cast<unsigned int>(rawBuffer[index]);
}

void MarchingCube::VertexInterpolate(const uPoint &p1, const uPoint &p2, const unsigned int p1Val, const unsigned int p2Val, fPoint &outInterp) const
{

    /**
     * P = P1 + (isovalue - V1) (P2 - P1) / (V2 - V1)
     * ratio <- (isosurface - p1Val) / (p2Val - p1Val)
     * interp <- p1 + ratio * (p2 - p1)
     */

    /**
     * If value of p1 and p2 is equal, set the cross point to middle
     * or it will raise divided by 0 exception
     */
    float ratio;
    if (p2Val == p1Val)
    {
        ratio = 0.5;
    }
    else
    {
        ratio = (static_cast<float>(currentIsoSurface) - static_cast<float>(p1Val)) / (static_cast<float>(p2Val) - static_cast<float>(p1Val));
    }

    outInterp = fPoint{
        static_cast<float>(p1.x) + ratio * (static_cast<float>(p2.x) - static_cast<float>(p1.x)),
        static_cast<float>(p1.y) + ratio * (static_cast<float>(p2.y) - static_cast<float>(p1.y)),
        static_cast<float>(p1.z) + ratio * (static_cast<float>(p2.z) - static_cast<float>(p1.z))};
}

void MarchingCube::WriteCurrentMeshToObj(const std::string &objFilename)
{
    const std::string commentsString =
        "# OBJ file generated by MarchingCube Algorithm\n"
        "# Face Count = " +
        std::to_string(currentMesh.size()) + "\n";

    std::ofstream outFile(std::filesystem::absolute(objFilename).string());

    outFile << commentsString;

    if (!currentMesh.empty())
    {
        for (int i = 0; i < currentMesh.size(); i++)
        {
            const std::string v0 = "v " + std::to_string(currentMesh[i].v0.x) + " " + std::to_string(currentMesh[i].v0.y) + " " + std::to_string(currentMesh[i].v0.z) + "\n";
            const std::string v1 = "v " + std::to_string(currentMesh[i].v1.x) + " " + std::to_string(currentMesh[i].v1.y) + " " + std::to_string(currentMesh[i].v1.z) + "\n";
            const std::string v2 = "v " + std::to_string(currentMesh[i].v2.x) + " " + std::to_string(currentMesh[i].v2.y) + " " + std::to_string(currentMesh[i].v2.z) + "\n";
            const std::string f = "f " + std::to_string(i * 3 + 1) + " " + std::to_string(i * 3 + 2) + " " + std::to_string(i * 3 + 3) + "\n";

            outFile << "v " + std::to_string(currentMesh[i].v0.x) + " " + std::to_string(currentMesh[i].v0.y) + " " + std::to_string(currentMesh[i].v0.z) + "\n"
                    << "v " + std::to_string(currentMesh[i].v1.x) + " " + std::to_string(currentMesh[i].v1.y) + " " + std::to_string(currentMesh[i].v1.z) + "\n"
                    << "v " + std::to_string(currentMesh[i].v2.x) + " " + std::to_string(currentMesh[i].v2.y) + " " + std::to_string(currentMesh[i].v2.z) + "\n";
        }

        for (int i = 0; i < currentMesh.size(); i++)
        {
            outFile << "f " + std::to_string(i * 3 + 1) + " " + std::to_string(i * 3 + 2) + " " + std::to_string(i * 3 + 3) + "\n";
        }
    }

    outFile.close();
}

void MarchingCube::CalculBounding(const fPoint &fCoordinates)
{
    /** Calculate bounding box*/
    if (fCoordinates.x > currentBoundingBox[0].x)
    {
        currentBoundingBox[0].x = fCoordinates.x;
    }
    if (fCoordinates.y > currentBoundingBox[0].y)
    {
        currentBoundingBox[0].y = fCoordinates.y;
    }
    if (fCoordinates.z > currentBoundingBox[0].z)
    {
        currentBoundingBox[0].z = fCoordinates.z;
    }

    if (fCoordinates.x < currentBoundingBox[1].x)
    {
        currentBoundingBox[1].x = fCoordinates.x;
    }
    if (fCoordinates.y < currentBoundingBox[1].y)
    {
        currentBoundingBox[1].y = fCoordinates.y;
    }
    if (fCoordinates.z < currentBoundingBox[1].z)
    {
        currentBoundingBox[1].z = fCoordinates.z;
    }
}

bool MarchingCube::ParseFileName(const std::string &filename, Dimension &dimension)
{

    std::regex pattern(".+_[0-9]+_[0-9]+_[0-9]+");

    if (!std::regex_search(filename, pattern))
    {
        return false;
    }

    std::stringstream ss(filename);
    std::vector<unsigned long> vec;
    std::string rawString;
    std::regex numberPattern("[0-9]+");

    while (std::getline(ss, rawString, '_'))
    {
        if (!std::regex_search(rawString, numberPattern))
        {
            continue;
        }

        vec.emplace_back(std::stoul(rawString));
    }

    if (vec.size() < 3)
    {
        return false;
    }

    dimension = Dimension{
        static_cast<unsigned int>(vec[0]),
        static_cast<unsigned int>(vec[1]),
        static_cast<unsigned int>(vec[2])};
    return true;
}

void MarchingCube::GetMeshNormal(const std::vector<Triangle> &inputTri, std::vector<fPoint> &outTriNormal)
{
    for (const auto &m : inputTri)
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

        outTriNormal.emplace_back<fPoint>(fPoint{
            outerProduct.x / distance,
            outerProduct.y / distance,
            outerProduct.z / distance});
    }
}
