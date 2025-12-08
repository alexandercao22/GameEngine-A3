#pragma once

#include <string>
#include "PackageManager.h"

class Resource {
private:
	AssetData _data;
	int _refCount = 0;

public:
	virtual Resource* LoadFromDisk(std::string id) = 0;
	virtual void UnLoad() = 0;
	//virtual GetProperties();
	
	virtual void RefAdd();
	virtual void RefSub();
	virtual int GetRef();

	void SetData(AssetData& assetData);

	virtual ~Resource() = default;
};