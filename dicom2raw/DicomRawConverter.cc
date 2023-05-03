#include "DicomRawConverter.h"

#include <filesystem>
#include <regex>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>

#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmImage.h>
#include <gdcmAttribute.h>
#include <gdcmImageWriter.h>
#include <gdcmImageChangeTransferSyntax.h>
#include <gdcmTransferSyntax.h>
#include <gdcmImage.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmImageChangePlanarConfiguration.h>
#include <gdcmPixelFormat.h>

#include <opencv2/opencv.hpp>

DicomRawConverter::DicomRawConverter() {}
DicomRawConverter::DicomRawConverter(const std::string &directory, const std::string &filePatternString)
{
    if (std::filesystem::exists(directory))
    {
        std::regex filePattern(filePatternString);

        for (const auto &entry : std::filesystem::directory_iterator(directory))
        {
            auto pathStr = entry.path().string();
            if (std::regex_search(pathStr, filePattern))
            {
                dicomSequentialNames.emplace_back(
                    std::pair<std::string, unsigned int>(
                        std::move(std::filesystem::absolute(pathStr).string()),
                        0));
            }
        }
    }

    dicomSequential.resize(dicomSequentialNames.size());
    dimension.depth = static_cast<unsigned int>(dicomSequentialNames.size());
}
DicomRawConverter::DicomRawConverter(const std::vector<std::string> &inputPaths)
{
    for (const auto &p : inputPaths)
    {
        if (!std::filesystem::exists(p))
        {
            continue;
        }

        dicomSequentialNames.emplace_back(
            std::pair<std::string, unsigned int>(
                std::move(std::filesystem::absolute(p).string()),
                0));
    }

    dicomSequential.resize(dicomSequentialNames.size());
    dimension.depth = static_cast<unsigned int>(dicomSequentialNames.size());
}

DicomRawConverter::~DicomRawConverter() {}

void DicomRawConverter::SortFile(const std::string &sortPatternNumberString, const unsigned int resultOrder)
{
    std::regex sortPatternNumber(sortPatternNumberString);

    for (auto &&filenamePair : dicomSequentialNames)
    {
        const auto filename = std::filesystem::path(filenamePair.first).stem().string();
        std::smatch m;
        if (std::regex_search(filename, m, sortPatternNumber))
        {
            if (resultOrder >= m.size())
            {
                continue;
            }

            try
            {
                const auto order = static_cast<unsigned int>(std::stoul(m[resultOrder].str()));
                filenamePair.second = order;
            }
            catch (std::exception &ex)
            {
            }
        }
    }

    std::sort(
        dicomSequentialNames.begin(),
        dicomSequentialNames.end(),
        [](const auto &pairA, const auto &pairB)
        {
            return static_cast<int>(pairA.second) < static_cast<int>(pairB.second);
        });
}

void DicomRawConverter::DecompressDicom(const unsigned int order, const bool isCV)
{
    std::ifstream inFile(dicomSequentialNames[order].first, std::ios::binary);
    gdcm::ImageReader reader;
    reader.SetStream(inFile);

    if (!reader.Read())
    {
        brokenLayer.emplace_back(order);
        return;
    }

    auto &ds = reader.GetFile().GetDataSet();
    auto &header = reader.GetFile().GetHeader();

    const auto pixelDataTag = gdcm::Tag(0x7fe0, 0x0010);
    const auto dataTransferTag = gdcm::Tag(0x0002, 0x0010);

    gdcm::Attribute<0x0028, 0x0010> rowsAttr;
    gdcm::Attribute<0x0028, 0x0011> colsAttr;
    gdcm::Attribute<0x0028, 0x0100> bitsAllocatedAttr;
    gdcm::Attribute<0x0028, 0x0101> bitsStoredAttr;

    rowsAttr.SetFromDataElement(ds.GetDataElement(rowsAttr.GetTag()));
    colsAttr.SetFromDataElement(ds.GetDataElement(colsAttr.GetTag()));
    bitsAllocatedAttr.SetFromDataElement(ds.GetDataElement(bitsAllocatedAttr.GetTag()));

    const unsigned int rows = rowsAttr.GetValue();
    const unsigned int cols = colsAttr.GetValue();
    const unsigned int bitsAllocated = bitsAllocatedAttr.GetValue();

    if (bitsAllocated != 8 && bitsAllocated != 16)
    {
        brokenLayer.emplace_back(order);
        return;
    }

    if (order == 0)
    {
        dimension.width = rows;
        dimension.height = cols;
    }

    auto &image = reader.GetImage();
    unsigned long imgBufferLength = image.GetBufferLength();

    auto imgBuffer = std::make_unique<uint8_t[]>(imgBufferLength);
    const bool isBufferGet = image.GetBuffer(reinterpret_cast<char *>(imgBuffer.get()));

    if (!isBufferGet)
    {
        brokenLayer.emplace_back(order);
        return;
    }

    if (bitsAllocated == 16)
    {
        auto shortBuffer = std::make_unique<unsigned short[]>(imgBufferLength / 2);
        memcpy(shortBuffer.get(), reinterpret_cast<unsigned short *>(imgBuffer.get()), imgBufferLength);

        auto maxElement = *std::max_element(shortBuffer.get(), shortBuffer.get() + imgBufferLength / 2);
        auto minElement = *std::min_element(shortBuffer.get(), shortBuffer.get() + imgBufferLength / 2);

        auto convertedBuffer = std::make_unique<uint8_t[]>(imgBufferLength / 2);

        std::transform(
            shortBuffer.get(),
            shortBuffer.get() + imgBufferLength / 2,
            convertedBuffer.get(),
            [&maxElement, &minElement](unsigned short u16) -> uint8_t
            {
                unsigned short resU16 = static_cast<float>((u16 - minElement) * 255) / static_cast<float>(maxElement - minElement);
                return static_cast<uint8_t>(resU16);
            });
        imgBufferLength /= 2;
        imgBuffer = std::move(convertedBuffer);
    }

    dicomSequential[order].clear();
    dicomSequential[order].resize(imgBufferLength);
    memcpy(dicomSequential[order].data(), imgBuffer.get(), imgBufferLength);

    if (isCV)
    {
        cv::Mat matImg(dimension.width, dimension.height, CV_8UC1, dicomSequential[order].data());
        cv::Mat dstImg = matImg, filterDst;

        auto morphKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, 1));
        cv::Mat laplacianKernel = (cv::Mat_<float>(3, 3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);

        cv::GaussianBlur(dstImg, dstImg, cv::Size(3, 3), 0);
        cv::filter2D(dstImg, filterDst, -1, laplacianKernel);
        cv::addWeighted(dstImg, 1.5, filterDst, -0.5, 0, dstImg);

        cv::erode(dstImg, dstImg, morphKernel);

        // cv::equalizeHist(matImg, dstImg);

        memcpy(dicomSequential[order].data(), dstImg.data, dstImg.total());
        matImg.release();
        dstImg.release();
        morphKernel.release();
        laplacianKernel.release();
    }

    inFile.close();
}

void DicomRawConverter::GetDicomSequential(const unsigned int order, std::string &outName) const
{
    outName = dicomSequentialNames[order].first;
}

void DicomRawConverter::GetDicomSequential(const unsigned int order, std::vector<uint8_t> &outBuffer) const
{
    outBuffer = std::vector<uint8_t>(dicomSequential[order].begin(), dicomSequential[order].end());
}

void DicomRawConverter::ShowDicomSequential(const unsigned int order) const
{
    if (!dicomSequential.empty() && !dicomSequential[order].empty())
    {
        std::vector<uint8_t> cvBuffer(dicomSequential[order].begin(), dicomSequential[order].end());

        cv::Mat matImg(dimension.width, dimension.height, CV_8UC1, cvBuffer.data());

        cv::namedWindow(dicomSequentialNames[order].first, cv::WINDOW_NORMAL);
        cv::resizeWindow(dicomSequentialNames[order].first, dimension.width, dimension.height);
        cv::imshow(dicomSequentialNames[order].first, matImg);

        cv::waitKey(0);

        matImg.release();
    }
}

void DicomRawConverter::GetRawData(std::vector<uint8_t> &outBuffer) const
{
    outBuffer = std::vector<uint8_t>(rawBuffer.begin(), rawBuffer.end());
}

void DicomRawConverter::GetBrokenLayer(std::vector<unsigned int> &outBrokenLayer) const
{
    outBrokenLayer = std::vector<unsigned int>(brokenLayer.begin(), brokenLayer.end());
}

bool DicomRawConverter::Build()
{
    return Build(false);
}

bool DicomRawConverter::Build(const bool isCV)
{
    rawBuffer.clear();
    for (unsigned int i = 0; i < dimension.depth; i++)
    {
        DecompressDicom(i, isCV);
    }

    if (!brokenLayer.empty())
    {
        return false;
    }

    rawBuffer.resize(dimension.width * dimension.height * dimension.depth);

    for (unsigned int d = 0; d < dimension.depth; ++d)
    {
        auto &image = dicomSequential[d];

        for (unsigned int h = 0; h < dimension.height; ++h)
        {
            memcpy(
                rawBuffer.data() + d * dimension.height * dimension.width + h * dimension.width,
                image.data() + h * dimension.width,
                dimension.width);
        }
    }

    return true;
}

void DicomRawConverter::WriteToRawFile(const std::string &outFilename) const
{
    if (rawBuffer.empty())
    {
        return;
    }

    std::ofstream outFile(outFilename, std::ios::binary);
    outFile.write(reinterpret_cast<const char *>(rawBuffer.data()), rawBuffer.size());
    outFile.close();
}

const unsigned int DicomRawConverter::GetDicomCounts() const
{
    return static_cast<unsigned int>(dicomSequentialNames.size());
}

void DicomRawConverter::GetRawDimension(Dimension &outDimension) const
{
    outDimension = dimension;
}