#include "AssetManager.h"

#include <cstdlib>
#include "Engine/Core/Assert.h"

_NPGS_BEGIN
_ASSET_BEGIN

FAssetManager* FAssetManager::GetInstance()
{
	static FAssetManager Instance;
	return &Instance;
}

FAssetManager::~FAssetManager()
{
	ClearAssets();
}

_ASSET_END
_NPGS_END
