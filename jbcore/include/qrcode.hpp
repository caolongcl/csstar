#pragma once

#include <string>
#include "ptr.hpp"
#include "result.hpp"

namespace dsg {
struct QRCode {
  virtual ~QRCode() = default;
  virtual auto ParseStrFromRGB(unsigned char* rgb, int width, int height,
                               std::string& result) -> Result = 0;

  static auto Request() -> interface_ptr<QRCode>;
  static auto DumpVersion() -> void;
};

}  // namespace dsg