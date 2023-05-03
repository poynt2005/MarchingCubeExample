#include "DicomRawConverter.h"
#include "DicomRawConverterAPI.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>
#include <memory>

template <typename T>
inline void Release2DArray(T **buf)
{
    delete[] (*buf);
    *buf = nullptr;
}

std::unordered_map<ConvHandle, std::unique_ptr<DicomRawConverter>> convInstanceMapping;

ConvHandle CreateDicomRawConverter(const char *directoryName, const char *dicomSearchPattern)
{
    auto converter = std::make_unique<DicomRawConverter>(directoryName, dicomSearchPattern);
    ConvHandle handle = reinterpret_cast<ConvHandle>(converter.get());
    convInstanceMapping.insert(std::pair<ConvHandle, std::unique_ptr<DicomRawConverter>>(handle, std::move(converter)));
    return handle;
}

void ReleaseDicomRawConverter(const ConvHandle handle)
{
    convInstanceMapping.erase(handle);
}

int CheckDicomRawConverterExists(const ConvHandle handle)
{
    return convInstanceMapping.count(handle);
}

/** sort number pattern, group order*/
void SortDicomFile(const ConvHandle handle, const char *sortNumberPattern, const unsigned int groupOrder)
{
    convInstanceMapping[handle]->SortFile(sortNumberPattern, groupOrder);
}

int Build(const ConvHandle handle, const int isCV)
{
    return static_cast<int>(convInstanceMapping[handle]->Build(isCV));
}
/** order number, outBuffer*/
void GetDicomBufferSequential(const ConvHandle handle, const unsigned int order, char **outBuffer, unsigned int *bufSize)
{
    std::vector<uint8_t> outBufferVec;
    convInstanceMapping[handle]->GetDicomSequential(order, outBufferVec);
    *outBuffer = new char[outBufferVec.size()];
    memcpy(*outBuffer, reinterpret_cast<char *>(outBufferVec.data()), outBufferVec.size());
    *bufSize = static_cast<unsigned int>(outBufferVec.size());
}
/** order number, outName*/
void GetDicomNameSequential(const ConvHandle handle, const unsigned int order, char **outName)
{
    std::string outNameStr;
    convInstanceMapping[handle]->GetDicomSequential(order, outNameStr);
    *outName = new char[outNameStr.length() + 1];
    strncpy(*outName, outNameStr.data(), outNameStr.length());
    (*outName)[outNameStr.length()] = '\0';
}

void ShowDicomSequential(const ConvHandle handle, const unsigned int order)
{
    convInstanceMapping[handle]->ShowDicomSequential(order);
}

void GetRawDimension(const ConvHandle handle, Dimension *dimension)
{
    Dimension outDimension;
    convInstanceMapping[handle]->GetRawDimension(outDimension);
    dimension->height = outDimension.height;
    dimension->width = outDimension.width;
    dimension->depth = outDimension.depth;
}

unsigned int GetDicomCounts(const ConvHandle handle)
{
    return convInstanceMapping[handle]->GetDicomCounts();
}

void WriteToRawFile(const ConvHandle handle, const char *outputFilename)
{
    convInstanceMapping[handle]->WriteToRawFile(outputFilename);
}
void GetRawData(const ConvHandle handle, char **outRawBuffer, unsigned int *bufSize)
{
    std::vector<uint8_t> outRawBufferVec;
    convInstanceMapping[handle]->GetRawData(outRawBufferVec);
    *outRawBuffer = new char[outRawBufferVec.size()];
    memcpy(*outRawBuffer, reinterpret_cast<char *>(outRawBufferVec.data()), outRawBufferVec.size());
    *bufSize = static_cast<unsigned int>(outRawBufferVec.size());
}
void GetBrokenLayer(const ConvHandle handle, unsigned int **outLayer, unsigned int *layerCount)
{
    std::vector<unsigned int> outLayerVec;
    convInstanceMapping[handle]->GetBrokenLayer(outLayerVec);
    *outLayer = new unsigned int[outLayerVec.size()];
    memcpy(*outLayer, outLayerVec.data(), outLayerVec.size() * sizeof(unsigned int));
    *layerCount = static_cast<unsigned int>(outLayerVec.size());
}

void ReleaseDicomBuffer(char **buf)
{
    Release2DArray<char>(buf);
}
void ReleaseDicomLayer(unsigned int **layer)
{
    Release2DArray<unsigned int>(layer);
}