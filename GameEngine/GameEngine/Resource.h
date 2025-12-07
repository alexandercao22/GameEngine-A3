#pragma once

#include <string>

class Resource {
private:
	int _refCount = 0;

public:
	virtual Resource* LoadFromDisk(std::string id) = 0;
	virtual void UnLoad() = 0;
	//virtual GetProperties();
	
	virtual void RefAdd();
	virtual void RefSub();
	virtual int GetRef();
	virtual ~Resource() = default;

};