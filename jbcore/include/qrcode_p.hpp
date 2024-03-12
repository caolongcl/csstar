#pragma once

#include <string>

#include "qrcode.hpp"

namespace DSG
{
  class QRCodeP : public QRCode
  {
  public:
    QRCodeP() = default;
    ~QRCodeP() = default;

    QRCodeP(const QRCodeP &) = default;
    QRCodeP &operator=(const QRCodeP &) = default;
    QRCodeP(QRCodeP &&) = default;
    QRCodeP &operator=(QRCodeP &&) = default;

  private:
    auto ParseStrFromRGB(unsigned char *rgb, int width, int height, std::string& result) -> Result override;
  };
}