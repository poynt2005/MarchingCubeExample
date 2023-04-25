#ifndef __MARCHING_CUBE_TYPES_H__
#define __MARCHING_CUBE_TYPES_H__

#define DEFAULT_ISOSURFACE 100

typedef struct _dimension
{
    unsigned int width;
    unsigned int height;
    unsigned int depth;
} Dimension;

typedef struct _upoint
{
    unsigned int x;
    unsigned int y;
    unsigned int z;
} uPoint;

typedef struct _fpoint
{
    float x;
    float y;
    float z;
} fPoint;

typedef struct _triangle
{
    fPoint v0;
    fPoint v1;
    fPoint v2;
} Triangle;

typedef struct _color3
{
    float r;
    float g;
    float b;
} Color3;

#endif