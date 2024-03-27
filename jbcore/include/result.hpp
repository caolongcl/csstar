#pragma once

#include <cstdint>

namespace dsg {
struct Result {
  enum class Val : uint32_t {
    eSuccess,
    eErrMqtt,
    eErrQRCode,
  };

  Result(Val v) : val(v) {
  }

  Val val;

  operator bool() const {
    return val == Val::eSuccess;
  }
};

using RV = Result::Val;
} // namespace dsg