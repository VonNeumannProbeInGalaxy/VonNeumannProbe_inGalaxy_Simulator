#include "Npgs.h"

using namespace Npgs;
using namespace Npgs::Util;

int main()
{
	FLogger::Init();
	
	FApplication App({ 1280, 960 }, "Npgs FPS:", true, false);
	App.ExecuteMainRender();
	return 0;
}
