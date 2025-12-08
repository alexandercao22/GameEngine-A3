#pragma once

#include <string>
#include "PackageManager.h"

class Resource {
private:
	int _refCount = 0;

protected:
	AssetData _data;

public:
	virtual ~Resource() = default;

	virtual Resource* LoadFromDisk(std::string id) = 0;
	virtual void UnLoad() = 0;
	//virtual GetProperties();
	
	virtual void RefAdd();
	virtual void RefSub();
	virtual int GetRef();

	void SetData(AssetData& assetData);

};