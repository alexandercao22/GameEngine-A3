#include "Resource.h"

void Resource::RefAdd() {
	ReferenceCount++;
}
void Resource::RefSub() {
	ReferenceCount--;
}
int Resource::GetRef() {
	return ReferenceCount;
}