#include "DicomRawConverterAPI.h"
#include "../Types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    printf("Create Dicom Raw Converter\n");
    ConvHandle handle = CreateDicomRawConverter("./sample_dicom", ".+dcm");
    printf("%lu\n", handle);

    unsigned int dicomCounts = GetDicomCounts(handle);
    printf("Dicom counts: %u\n", dicomCounts);

    printf("Sorting Dicom files...\n");
    SortDicomFile(handle, "[0-9]+", 1);

    char *buff = NULL;
    unsigned int buffSize = 0;
    GetDicomBufferSequential(handle, 5, &buff, &buffSize);
    printf("Layer 5's size: %u\n", buffSize);
    ReleaseDicomBuffer(&buff);

    char *layerFilename = NULL;
    GetDicomNameSequential(handle, 5, &layerFilename);
    printf("Layer 5's name: %s\n", layerFilename);
    ReleaseDicomBuffer(&layerFilename);

    printf("Start to build raw file...\n");
    int isBuildSuccess = Build(handle, 1);
    if (!isBuildSuccess)
    {
        printf("Build failed\n");
        ReleaseDicomRawConverter(handle);
        return 1;
    }
    printf("Build success\n");

    printf("Show Layer 5 image and wait for your keys\n");
    ShowDicomSequential(handle, 5);

    Dimension dimension;
    GetRawDimension(handle, &dimension);
    printf("Dimension: %ux%ux%u\n", dimension.width, dimension.height, dimension.depth);

    char rawFilename[100] = {'\0'};
    sprintf(rawFilename, "ABC_%u_%u_%u.raw", dimension.width, dimension.height, dimension.depth);
    printf("Write to %s\n", rawFilename);
    WriteToRawFile(handle, rawFilename);

    char *rawData = NULL;
    unsigned int rawDataSize = 0;
    GetRawData(handle, &rawData, &rawDataSize);
    printf("Get raw data buffer with size: %u\n", rawDataSize);
    printf("Release raw data buffer\n");
    ReleaseDicomBuffer(&rawData);

    unsigned int *brokenLayer = NULL;
    unsigned int brodenLayerCount = 0;
    GetBrokenLayer(handle, &brokenLayer, &brodenLayerCount);
    printf("Get broken layer with count: %u\n", brodenLayerCount);
    for (unsigned int i = 0; i < brodenLayerCount; ++i)
    {
        printf("Layer: %u is broken\n", brokenLayer + i);
    }
    printf("Release broken layer\n");
    ReleaseDicomLayer(&brokenLayer);

    printf("Release Dicom Raw Converter\n");
    ReleaseDicomRawConverter(handle);

    printf("End\n");
    return 0;
}