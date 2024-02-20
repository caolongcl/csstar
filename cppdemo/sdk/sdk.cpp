#include "sdk.hpp"

namespace demo
{

  Sdk::Sdk(const std::string &name)
      : _name(name)
  {
    std::cout << _name << " created!\n";
  }

  Sdk::~Sdk()
  {
    std::cout << _name << " destroyed!\n";
  }

  void Sdk::PrintMyName() const
  {
    std::cout << "hello, my name is " << _name << "\n";
  }
}