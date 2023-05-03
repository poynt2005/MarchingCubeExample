#ifndef __DICOM_RAW_CONVERTER_H__
#define __DICOM_RAW_CONVERTER_H__

#include "../Types.h"

#include <string>
#include <vector>
#include <memory>
#include <utility>

class DicomRawConverter
{
public:
    DicomRawConverter();
    DicomRawConverter(const std::string &, const std::string &);
    DicomRawConverter(const std::vector<std::string> &);
    ~DicomRawConverter();

    void SortFile(const std::string &, const unsigned int resultOrder);
    bool Build();
    bool Build(const bool);

    void GetDicomSequential(const unsigned int, std::vector<uint8_t> &) const;
    void GetDicomSequential(const unsigned int, std::string &) const;
    void ShowDicomSequential(const unsigned int) const;
    void GetRawDimension(Dimension &) const;

    const unsigned int GetDicomCounts() const;

    void WriteToRawFile(const std::string &) const;
    void GetRawData(std::vector<uint8_t> &) const;
    void GetBrokenLayer(std::vector<unsigned int> &) const;

private:
    Dimension dimension{0, 0, 0};
    std::vector<uint8_t> rawBuffer;
    std::vector<unsigned int> brokenLayer;
    std::vector<std::pair<std::string, unsigned int>> dicomSequentialNames;
    std::vector<std::vector<uint8_t>> dicomSequential;
    void DecompressDicom(const unsigned int, const bool);
};

#endif