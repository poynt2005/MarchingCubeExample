#ifndef __DICOM_RAW_CONVERTER_API_H__
#define __DICOM_RAW_CONVERTER_API_H__

#include "../Types.h"

#ifdef BUILDD2RAPI
#define EXPORTD2RAPI __declspec(dllexport)
#else
#define EXPORTD2RAPI __declspec(dllimport)
#endif

typedef unsigned long long ConvHandle;

#ifdef __cplusplus
extern "C"
{
#endif

    /** Directory name, search pattern*/
    EXPORTD2RAPI ConvHandle CreateDicomRawConverter(const char *, const char *);
    EXPORTD2RAPI void ReleaseDicomRawConverter(const ConvHandle);
    EXPORTD2RAPI int CheckDicomRawConverterExists(const ConvHandle);

    /** sort number pattern, group order*/
    EXPORTD2RAPI void SortDicomFile(const ConvHandle, const char *, const unsigned int);
    EXPORTD2RAPI int Build(const ConvHandle, const int);
    /** order number, outBuffer*/
    EXPORTD2RAPI void GetDicomBufferSequential(const ConvHandle, const unsigned int, char **, unsigned int *);
    /** order number, outName*/
    EXPORTD2RAPI void GetDicomNameSequential(const ConvHandle, const unsigned int, char **);
    EXPORTD2RAPI void ShowDicomSequential(const ConvHandle, const unsigned int);
    EXPORTD2RAPI void GetRawDimension(const ConvHandle, Dimension *);
    EXPORTD2RAPI unsigned int GetDicomCounts(const ConvHandle);

    EXPORTD2RAPI void WriteToRawFile(const ConvHandle, const char *);
    EXPORTD2RAPI void GetRawData(const ConvHandle, char **, unsigned int *);
    EXPORTD2RAPI void GetBrokenLayer(const ConvHandle, unsigned int **, unsigned int *);

    EXPORTD2RAPI void ReleaseDicomBuffer(char **);
    EXPORTD2RAPI void ReleaseDicomLayer(unsigned int **);

#ifdef __cplusplus
}
#endif

#endif