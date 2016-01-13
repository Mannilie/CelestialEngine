#include "LineSegment.h"

// Debugging
#include "Gizmos.h"

LineSegment::LineSegment() :
	start(vec3(-1)),
	end(vec3(1)) {}

LineSegment::LineSegment(const vec3& _start, const vec3& _end) :
	start(_start),
	end(_end) {}

LineSegment::~LineSegment(){}

void LineSegment::Draw(vec4 _color) {
	Gizmos::AddLine(start, end, _color);
}