#include <print>
#include <string>
#include "Engine/Base/CelestialObject/StellarClass.h"

int main() {
    std::string sc = "O2 Ia";
    StellarClass::Parse(sc);

    return 0;
}