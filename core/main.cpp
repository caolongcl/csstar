#include <iostream>

#include "mqtt.hpp"

int main(int, char **)
{
    CS::Sdk sdk("sdk_lib");
    sdk.PrintMyName();
    return 0;
}
