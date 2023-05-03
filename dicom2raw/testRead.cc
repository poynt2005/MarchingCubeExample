#include <gdcmReader.h>
#include <gdcmImageReader.h>
#include <gdcmImage.h>
#include <gdcmAttribute.h>
#include <gdcmImageWriter.h>
#include <gdcmImageChangeTransferSyntax.h>
#include <gdcmImageChangePlanarConfiguration.h>
#include <gdcmPixelFormat.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <memory>
#include <string.h>
#include <math.h>

int main()
{
    const auto dicomTestFile = std::filesystem::absolute("sample_dicom/I0").string().data();
    std::cout << dicomTestFile << '\n';

    std::ifstream inFile(dicomTestFile, std::ios::binary);

    gdcm::ImageReader reader;
    reader.SetStream(inFile);

    if (!reader.Read())
    {
        std::cout << "Read file failed\n";
        exit(-1);
    }

    auto &ds = reader.GetFile().GetDataSet();

    const auto pixelDataTag = gdcm::Tag(0x7fe0, 0x0010);

    gdcm::Attribute<0x0028, 0x0010> rowsAttr;
    gdcm::Attribute<0x0028, 0x0011> colsAttr;
    gdcm::Attribute<0x0028, 0x0100> bitsAllocatedAttr;
    gdcm::Attribute<0x0028, 0x0101> bitsStoredAttr;

    rowsAttr.SetFromDataElement(ds.GetDataElement(rowsAttr.GetTag()));
    colsAttr.SetFromDataElement(ds.GetDataElement(colsAttr.GetTag()));
    bitsAllocatedAttr.SetFromDataElement(ds.GetDataElement(bitsAllocatedAttr.GetTag()));
    bitsStoredAttr.SetFromDataElement(ds.GetDataElement(bitsStoredAttr.GetTag()));

    const unsigned int rows = rowsAttr.GetValue();
    const unsigned int cols = colsAttr.GetValue();
    const unsigned int bitsAllocated = bitsAllocatedAttr.GetValue();
    const unsigned int bitsStored = bitsStoredAttr.GetValue();

    if (bitsAllocated != 8 && bitsAllocated != 16)
    {
        std::cout << "Cannot process bitsAllocated: " << bitsAllocated << '\n';
        exit(-1);
    }

    std::cout << "rows: " << rows << " cols: " << cols << " bitsAllocated: " << bitsAllocated << " bitsStored: " << bitsStored << '\n';

    auto &image = reader.GetImage();

    const unsigned long imgBufferLength = image.GetBufferLength();
    auto rawImgBuffer = std::make_unique<char[]>(imgBufferLength);

    const bool isBufferGet = image.GetBuffer(rawImgBuffer.get());

    // std::ofstream outFile("test.bin", std::ios::binary);
    // outFile.write(rawImgBuffer.get(), imgBufferLength);
    // outFile.close();

    auto imgBuffer = std::make_unique<uint8_t[]>(imgBufferLength);
    memcpy(imgBuffer.get(), reinterpret_cast<uint8_t *>(rawImgBuffer.get()), imgBufferLength);

    if (bitsAllocated == 16)
    {
        auto shortBuffer = std::make_unique<unsigned short[]>(imgBufferLength / 2);
        memcpy(shortBuffer.get(), reinterpret_cast<unsigned short *>(imgBuffer.get()), imgBufferLength);

        auto transformedBuffer = std::make_unique<uint8_t[]>(imgBufferLength / 2);

        std::transform(
            shortBuffer.get(),
            shortBuffer.get() + imgBufferLength / 2,
            transformedBuffer.get(),
            [](unsigned short u16) -> uint8_t
            {
                unsigned short st = static_cast<unsigned short>(std::floor(static_cast<float>(u16) / 65535.f * 255.f));
                return static_cast<uint8_t>(st);
            });
    }

    inFile.close();

    std::cout << "end\n";

    return 0;
}