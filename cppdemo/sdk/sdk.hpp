#include <iostream>
#include <string>

namespace demo
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