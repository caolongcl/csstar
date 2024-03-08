#pragma once

#include <cstdint>

namespace DSG
{
  struct Result
  {
    enum class Val : uint32_t
    {
      eSuccess,
      eErrMqtt,
    };

    Result(Val v) : val(v)
    {
    }

    Val val;

    operator bool() const
    {
      return val == Val::eSuccess;
    }
  };

  using RV = Result::Val;
}