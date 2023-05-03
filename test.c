#include <stdio.h>
#include "MarchingCubeAPI.h"

int main()
{
    const char *rawFileName = "ABC_512_512_51.raw";
    const char *objFileName = "result.obj";

    Dimension fileDimension;
    ParseFileName(rawFileName, &fileDimension);

    printf("Parse filename: \n");
    printf("w: %d, h: %d, d: %d\n", fileDimension.width, fileDimension.height, fileDimension.depth);

    MCHandle handle = CreateMarchingCubeInstance(rawFileName, &fileDimension);

    printf("Start marching...\n");
    March(handle, 10);

    printf("Write File...\n");
    WriteCurrentMeshToObj(handle, objFileName);

    printf("Release instance\n");
    ReleaseMarchingCubeInstance(handle);

    printf("End\n");

    return 0;
}