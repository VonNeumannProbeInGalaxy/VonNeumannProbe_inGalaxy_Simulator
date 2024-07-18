#include <print>
#include <string>
#include "Engine/Base/CelestialObject/StellarClass.h"

int main() {
    std::string sc = "G2V";
    StellarClass::Parse(sc);

    return 0;
}