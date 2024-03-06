#include <iostream>
#include <string>

namespace CS
{
  class Sdk
  {
  public:
    Sdk(const std::string &name);
    ~Sdk();
    void PrintMyName() const;

  private:
    std::string _name;
  };
}