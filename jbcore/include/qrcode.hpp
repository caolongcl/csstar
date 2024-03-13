#pragma once

#include <string>
#include "result.hpp"

namespace DSG
{
  struct QRCode
  {
    virtual ~QRCode() = default;
    virtual auto ParseStrFromRGB(unsigned char *rgb, int width, int height, std::string& result) -> Result = 0;

    static auto Request() -> QRCode *;
    static auto DumpVersion() -> void;
  };

}