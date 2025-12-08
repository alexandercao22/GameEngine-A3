#include "Resource.h"

void Resource::RefAdd() {
	_refCount++;
}

void Resource::RefSub() {
	_refCount--;
}

int Resource::GetRef() {
	return _refCount;
}

void Resource::SetData(AssetData& data) {
	_data = std::move(data);
}