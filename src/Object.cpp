#include "Object.h"

int Object::instanceCount = 0;

Object::Object() {
	instanceCount++;
	instanceID = instanceCount;
}
Object::Object(string _name) : name(_name) {}
