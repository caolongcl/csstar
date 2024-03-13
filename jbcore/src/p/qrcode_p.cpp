#include "def.hpp"
#include "qrcode_p.hpp"

#include <ZXing/ZXVersion.h>
#include <ZXing/ReadBarcode.h>

using namespace ZXing;

namespace DSG
{
#define DSG_Err RV::eErrQRCode

#define DSG_MQTTCALL_V(funccall) funccall

  ///////////////////////////////////////////
  auto QRCode::Request() -> QRCode *
  {
    return new QRCodeP();
  }

  auto QRCode::DumpVersion() -> void
  {
    DSG_LOG("{ZXing version:" << ZXing::ZXING_VERSION_STR << "}");
  }

  ///////////////////////////////////////////
  auto QRCodeP::ParseStrFromRGB(unsigned char *rgb, int width, int height, std::string &result) -> Result
  {
    if (!rgb || width <= 0 || height <= 0)
    {
      DSG_ERROR("invalid input rgb:" << (rgb != nullptr) << ", widht:" << width << ", height:" << height);
      return DSG_Err;
    }

    result.clear();

    ReaderOptions options;
    ImageView image{rgb, width, height, ImageFormat::RGB};

    auto barcodes = ReadBarcodes(image, options);
    if (barcodes.size() <= 0)
    {
      DSG_ERROR("QRCode Parse failed, no target");
      return DSG_Err;
    }

    auto &b = barcodes[0]; // 只处理第一个结果
    if (!b.isValid() || b.format() != BarcodeFormat::QRCode || b.contentType() != ContentType::Text)
    {
      DSG_ERROR("QRCode Parse failed " << ToString(b.error()));
      return DSG_Err;
    }

    result = b.text(TextMode::Plain);
    return RV::eSuccess;
  }
}