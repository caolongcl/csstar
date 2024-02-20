#include <iostream>

#include "sdk.hpp"

int main(int, char **)
{
    demo::Sdk sdk("sdk_lib");
    sdk.PrintMyName();
    return 0;
}
