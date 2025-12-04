#pragma once
#include <string>
class Resource {
private:
	int ReferenceCount = 0;
public:
	virtual Resource* LoadFromDisk(std::string id) = 0;
	virtual void UnLoad() = 0;
	//virtual GetProperties();
	virtual ~Resource() = default;

};