#include <iostream>
#include <thread>
#include <chrono>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <def.hpp>
#include <qrcode.hpp>

using namespace DSG;

int main(int argc, char **argv)
{
    QRCode::DumpVersion();
    if (argc != 2)
    {
        DSG_ERROR("Usage: sample_qrcode image_path");
        return -1;
    }

    auto *qrcode = QRCode::Request();

    // load test png
    // std::string filePath = "/Volumes/Data/junbo_prj/star/jbcore/build/qrcode_test_image.png";
    std::string filePath = argv[1];
    int width, height, channels;
    std::unique_ptr<stbi_uc, void (*)(void *)> buffer(stbi_load(filePath.c_str(), &width, &height, &channels, 0), stbi_image_free);
    if (!buffer)
    {
        DSG_ERROR("Failed to read image: " << filePath << " (" << stbi_failure_reason() << ")");
        delete qrcode;
        return -1;
    }

    std::string result;
    if (!qrcode->ParseStrFromRGB(buffer.get(), width, height, result))
    {
        return -1;
    }

    DSG_LOG("QRCode:" << result);

    delete qrcode;
    return 0;
}