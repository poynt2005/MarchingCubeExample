#ifndef __MARCHING_CUBE_H__
#define __MARCHING_CUBE_H__
#include "Types.h"

#include <vector>
#include <string>

class MarchingCube
{
public:
    MarchingCube(const std::string &, const Dimension &);
    MarchingCube(const std::vector<uint8_t> &, const Dimension &);

    ~MarchingCube();

    void March(const unsigned int);
    void March();

    void GetCurrentMesh(std::vector<Triangle> &) const;
    void GetCurrentMeshNormalized(std::vector<Triangle> &) const;
    void GetCurrentBoundingBox(fPoint &, fPoint &) const;
    void WriteCurrentMeshToObj(const std::string &);

    static bool ParseFileName(const std::string &, Dimension &);
    static void GetMeshNormal(const std::vector<Triangle> &, std::vector<fPoint> &);

private:
    /** rawBuffer => Raw file's buffer */
    std::vector<uint8_t> rawBuffer;

    /** Mesh => Mesh calculated by current isosurface */
    std::vector<Triangle> currentMesh;

    /** Current mesh bounding box => 0->max, 1->min */
    std::vector<fPoint> currentBoundingBox;

    unsigned int currentIsoSurface;

    Dimension rawDimension;

    /** Calculate mesh by cube*/
    void CalculateMesh(const unsigned int, const unsigned int, const unsigned int);

    /** Get data of a given point*/
    inline unsigned int GetPointData(const unsigned int, const unsigned int, const unsigned int) const;

    /** Interpolate the cross point over the surface*/
    void VertexInterpolate(const uPoint &, const uPoint &, const unsigned int, const unsigned int, fPoint &) const;

    /** Calculate the bounding box */
    inline void CalculBounding(const fPoint &);
};

#endif