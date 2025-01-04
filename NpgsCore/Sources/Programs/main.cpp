#include "Npgs.h"
#include "Application.h"

using namespace Npgs;
using namespace Npgs::Util;

#include <vulkan/vulkan_raii.hpp>

int main()
{
    FLogger::Init();

    FApplication App({ 1280, 960 }, "Von-Neumann Probe in Galaxy Simulator FPS:", true, false);
    App.ExecuteMainRender();
    return 0;
}
