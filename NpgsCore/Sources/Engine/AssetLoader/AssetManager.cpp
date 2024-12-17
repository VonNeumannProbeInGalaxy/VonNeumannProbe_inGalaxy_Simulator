#include "AssetManager.h"

#include <cstdlib>
#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_ASSET_BEGIN

AssetManager* AssetManager::GetInstance()
{
	static AssetManager Instance;
	return &Instance;
}

AssetManager::~AssetManager()
{
	ClearAssets();
}

_ASSET_END
_NPGS_END
