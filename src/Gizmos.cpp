#include "Gizmos.h"
#include "gl_core_4_4.h"

#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Gizmos* Gizmos::sm_instance = nullptr;

Gizmos::Gizmos(unsigned int _maxLines, 
	unsigned int _maxTris,
	unsigned int _max2DLines, 
	unsigned int _max2DTris) : m_maxLines(_maxLines),
	m_lineCount(0),
	m_lines(new Line[_maxLines]),
	m_maxTris(_maxTris),
	m_triCount(0),
	m_tris(new Tri[_maxTris]),
	m_transparentTriCount(0),
	m_transparentTris(new Tri[_maxTris]),
	m_max2DLines(_max2DLines),
	m_2DlineCount(0),
	m_2Dlines(new Line[_max2DLines]),
	m_max2DTris(_max2DTris),
	m_2DtriCount(0),
	m_2Dtris(new Tri[_max2DTris]) {

	// Create shaders
	const char* vsSource = "#version 150\n \
					 in vec4 Position; \
					 in vec4 Color; \
					 out vec4 vColor; \
					 uniform mat4 ProjectionView; \
					 void main() { vColor = Color; gl_Position = ProjectionView * Position; }";

	const char* fsSource = "#version 150\n \
					 in vec4 vColor; \
                     out vec4 FragColor; \
					 void main()	{ FragColor = vColor; }";
    
    // Create vertex and fragment shader
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	// Reference and compile shaders
	glShaderSource(vs, 1, (const char**)&vsSource, 0);
	glCompileShader(vs);
	glShaderSource(fs, 1, (const char**)&fsSource, 0);
	glCompileShader(fs);

	// Create a new shader program
	m_shader = glCreateProgram();
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);
	glBindAttribLocation(m_shader, 0, "Position");
	glBindAttribLocation(m_shader, 1, "Color");
	glLinkProgram(m_shader);
    
	int success = GL_FALSE;
    glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
        
		glGetShaderInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link Gizmo shader program!\n%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
    
    // create VBOs
	glGenBuffers( 1, &m_lineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxLines * sizeof(Line), m_lines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_triVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(Tri), m_tris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_transparentTriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_maxTris * sizeof(Tri), m_transparentTris, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DlineVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DLines * sizeof(Line), m_2Dlines, GL_DYNAMIC_DRAW);

	glGenBuffers( 1, &m_2DtriVBO );
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glBufferData(GL_ARRAY_BUFFER, m_max2DTris * sizeof(Tri), m_2Dtris, GL_DYNAMIC_DRAW);

	glGenVertexArrays(1, &m_lineVAO);
	glBindVertexArray(m_lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_triVAO);
	glBindVertexArray(m_triVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_triVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_transparentTriVAO);
	glBindVertexArray(m_transparentTriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_transparentTriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_2DlineVAO);
	glBindVertexArray(m_2DlineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DlineVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), ((char*)0) + 16);

	glGenVertexArrays(1, &m_2DtriVAO);
	glBindVertexArray(m_2DtriVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_2DtriVBO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Vertex), ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Gizmos::~Gizmos() {
	delete[] m_lines;
	delete[] m_tris;
	delete[] m_transparentTris;
	glDeleteBuffers( 1, &m_lineVBO );
	glDeleteBuffers( 1, &m_triVBO );
	glDeleteBuffers( 1, &m_transparentTriVBO );
	glDeleteVertexArrays( 1, &m_lineVAO );
	glDeleteVertexArrays( 1, &m_triVAO );
	glDeleteVertexArrays( 1, &m_transparentTriVAO );
	delete[] m_2Dlines;
	delete[] m_2Dtris;
	glDeleteBuffers( 1, &m_2DlineVBO );
	glDeleteBuffers( 1, &m_2DtriVBO );
	glDeleteVertexArrays( 1, &m_2DlineVAO );
	glDeleteVertexArrays( 1, &m_2DtriVAO );
	glDeleteProgram(m_shader);
}

void Gizmos::Create(unsigned int _maxLines,
	unsigned int _maxTris,
	unsigned int _max2DLines, 
	unsigned int _max2DTris) {
	if (sm_instance == nullptr) {
		sm_instance = new Gizmos(_maxLines, _maxTris, _max2DLines, _max2DTris);
	}
}

void Gizmos::Destroy() {
	delete sm_instance;
	sm_instance = nullptr;
}

void Gizmos::Clear() {
	sm_instance->m_lineCount = 0;
	sm_instance->m_triCount = 0;
	sm_instance->m_transparentTriCount = 0;
	sm_instance->m_2DlineCount = 0;
	sm_instance->m_2DtriCount = 0;
}

// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
// at the transform's translation. Optional scale available.
void Gizmos::AddTransform(const glm::mat4& _transform, 
	float _scale) {
	glm::vec4 vXAxis = _transform[3] + _transform[0] * _scale;
	glm::vec4 vYAxis = _transform[3] + _transform[1] * _scale;
	glm::vec4 vZAxis = _transform[3] + _transform[2] * _scale;

	glm::vec4 red(1,0,0,1);
	glm::vec4 green(0,1,0,1);
	glm::vec4 blue(0,0,1,1);

	AddLine(_transform[3].xyz(), vXAxis.xyz(), red, red);
	AddLine(_transform[3].xyz(), vYAxis.xyz(), green, green);
	AddLine(_transform[3].xyz(), vZAxis.xyz(), blue, blue);
}

void Gizmos::AddAABB(const glm::vec3& _center, 
	const glm::vec3& _rvExtents, 
	const glm::vec4& _colour, 
	const glm::mat4* _transform) {
	glm::vec3 vVerts[8];
	glm::vec3 vX(_rvExtents.x, 0, 0);
	glm::vec3 vY(0, _rvExtents.y, 0);
	glm::vec3 vZ(0, 0, _rvExtents.z);

	if (_transform != nullptr) {
		vX = (*_transform * glm::vec4(vX,0)).xyz();
		vY = (*_transform * glm::vec4(vY,0)).xyz();
		vZ = (*_transform * glm::vec4(vZ,0)).xyz();
	}

	// top verts
	vVerts[0] = _center - vX - vZ - vY;
	vVerts[1] = _center - vX + vZ - vY;
	vVerts[2] = _center + vX + vZ - vY;
	vVerts[3] = _center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = _center - vX - vZ + vY;
	vVerts[5] = _center - vX + vZ + vY;
	vVerts[6] = _center + vX + vZ + vY;
	vVerts[7] = _center + vX - vZ + vY;

	AddLine(vVerts[0], vVerts[1], _colour, _colour);
	AddLine(vVerts[1], vVerts[2], _colour, _colour);
	AddLine(vVerts[2], vVerts[3], _colour, _colour);
	AddLine(vVerts[3], vVerts[0], _colour, _colour);

	AddLine(vVerts[4], vVerts[5], _colour, _colour);
	AddLine(vVerts[5], vVerts[6], _colour, _colour);
	AddLine(vVerts[6], vVerts[7], _colour, _colour);
	AddLine(vVerts[7], vVerts[4], _colour, _colour);

	AddLine(vVerts[0], vVerts[4], _colour, _colour);
	AddLine(vVerts[1], vVerts[5], _colour, _colour);
	AddLine(vVerts[2], vVerts[6], _colour, _colour);
	AddLine(vVerts[3], vVerts[7], _colour, _colour);
}

void Gizmos::AddCapsule(const glm::vec3 center, 
                        const float length, 
                        const float radius,
                        const int rows,
                        const int cols,
						const glm::vec4 color,
						const glm::vec3 direction,
                        const glm::mat4* rotation) {
    float half_sphere_center = (length * 0.5f) - radius;
	glm::vec4 right = glm::vec4(direction, 0) * half_sphere_center;
	glm::vec4 left = glm::vec4(direction, 0) * -half_sphere_center;

    if (rotation) {
        right = (*rotation) * right;
        left = (*rotation) * left;
    }

    glm::vec3 right_center = center + right.xyz();
    glm::vec3 left_center = center + left.xyz();

    AddSphere(right_center, radius, rows, cols, color);
    AddSphere(left_center, radius, rows, cols, color);

    for (int i = 0; i < cols; ++i) {
        float x = (float)i / (float)cols;
        x *= glm::pi<float>();
		glm::vec4 pos = glm::vec4(cosf(x), 0, sinf(x), 0) * radius;
        if (rotation) {
            pos = (*rotation) * pos;
        }
        AddLine(left_center + pos.xyz(), right_center + pos.xyz(), color);
    }
}


void Gizmos::AddAABBFilled(const glm::vec3& _center, 
	const glm::vec3& _rvExtents, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform) {
	glm::vec3 vVerts[8];
	glm::vec3 vX(_rvExtents.x, 0, 0);
	glm::vec3 vY(0, _rvExtents.y, 0);
	glm::vec3 vZ(0, 0, _rvExtents.z);

	if (_transform != nullptr) {
		vX = (*_transform * glm::vec4(vX, 0)).xyz();
		vY = (*_transform * glm::vec4(vY, 0)).xyz();
		vZ = (*_transform * glm::vec4(vZ, 0)).xyz();
	}

	// top verts
	vVerts[0] = _center - vX - vZ - vY;
	vVerts[1] = _center - vX + vZ - vY;
	vVerts[2] = _center + vX + vZ - vY;
	vVerts[3] = _center + vX - vZ - vY;

	// bottom verts
	vVerts[4] = _center - vX - vZ + vY;
	vVerts[5] = _center - vX + vZ + vY;
	vVerts[6] = _center + vX + vZ + vY;
	vVerts[7] = _center + vX - vZ + vY;

	glm::vec4 vWhite(1,1,1,1);

	AddLine(vVerts[0], vVerts[1], vWhite, vWhite);
	AddLine(vVerts[1], vVerts[2], vWhite, vWhite);
	AddLine(vVerts[2], vVerts[3], vWhite, vWhite);
	AddLine(vVerts[3], vVerts[0], vWhite, vWhite);

	AddLine(vVerts[4], vVerts[5], vWhite, vWhite);
	AddLine(vVerts[5], vVerts[6], vWhite, vWhite);
	AddLine(vVerts[6], vVerts[7], vWhite, vWhite);
	AddLine(vVerts[7], vVerts[4], vWhite, vWhite);

	AddLine(vVerts[0], vVerts[4], vWhite, vWhite);
	AddLine(vVerts[1], vVerts[5], vWhite, vWhite);
	AddLine(vVerts[2], vVerts[6], vWhite, vWhite);
	AddLine(vVerts[3], vVerts[7], vWhite, vWhite);

	// top
	AddTri(vVerts[2], vVerts[1], vVerts[0], _fillColour);
	AddTri(vVerts[3], vVerts[2], vVerts[0], _fillColour);

	// bottom
	AddTri(vVerts[5], vVerts[6], vVerts[4], _fillColour);
	AddTri(vVerts[6], vVerts[7], vVerts[4], _fillColour);

	// front
	AddTri(vVerts[4], vVerts[3], vVerts[0], _fillColour);
	AddTri(vVerts[7], vVerts[3], vVerts[4], _fillColour);

	// back
	AddTri(vVerts[1], vVerts[2], vVerts[5], _fillColour);
	AddTri(vVerts[2], vVerts[6], vVerts[5], _fillColour);

	// left
	AddTri(vVerts[0], vVerts[1], vVerts[4], _fillColour);
	AddTri(vVerts[1], vVerts[5], vVerts[4], _fillColour);

	// right
	AddTri(vVerts[2], vVerts[3], vVerts[7], _fillColour);
	AddTri(vVerts[6], vVerts[2], vVerts[7], _fillColour);
}

void Gizmos::AddCylinderFilled(const glm::vec3& _center, 
	float _radius, 
	float _fHalfLength,
	unsigned int _segments, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform) {
	glm::vec4 white(1,1,1,1);

	float segmentSize = (2 * glm::pi<float>()) / _segments;

	for ( unsigned int i = 0 ; i < _segments ; ++i ) {
		glm::vec3 v0top(0,_fHalfLength,0);
		glm::vec3 v1top( sinf( i * segmentSize ) * _radius, _fHalfLength, cosf( i * segmentSize ) * _radius);
		glm::vec3 v2top( sinf( (i+1) * segmentSize ) * _radius, _fHalfLength, cosf( (i+1) * segmentSize ) * _radius);
		glm::vec3 v0bottom(0,-_fHalfLength,0);
		glm::vec3 v1bottom( sinf( i * segmentSize ) * _radius, -_fHalfLength, cosf( i * segmentSize ) * _radius);
		glm::vec3 v2bottom( sinf( (i+1) * segmentSize ) * _radius, -_fHalfLength, cosf( (i+1) * segmentSize ) * _radius);

		if (_transform != nullptr) {
			v0top = (*_transform * glm::vec4(v0top, 0)).xyz();
			v1top = (*_transform * glm::vec4(v1top, 0)).xyz();
			v2top = (*_transform * glm::vec4(v2top, 0)).xyz();
			v0bottom = (*_transform * glm::vec4(v0bottom, 0)).xyz();
			v1bottom = (*_transform * glm::vec4(v1bottom, 0)).xyz();
			v2bottom = (*_transform * glm::vec4(v2bottom, 0)).xyz();
		}

		// triangles
		AddTri( _center + v0top, _center + v1top, _center + v2top, _fillColour);
		AddTri( _center + v0bottom, _center + v2bottom, _center + v1bottom, _fillColour);
		AddTri( _center + v2top, _center + v1top, _center + v1bottom, _fillColour);
		AddTri( _center + v1bottom, _center + v2bottom, _center + v2top, _fillColour);

		// lines
		AddLine(_center + v1top, _center + v2top, white, white);
		AddLine(_center + v1top, _center + v1bottom, white, white);
		AddLine(_center + v1bottom, _center + v2bottom, white, white);
	}
}

void Gizmos::AddRing(const glm::vec3& _center, 
	float _innerRadius, 
	float _outerRadius,
	unsigned int _segments, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform) {
	glm::vec4 vSolid = _fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * glm::pi<float>()) / _segments;

	for ( unsigned int i = 0 ; i < _segments ; ++i ) {
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * _outerRadius, 0, cosf( i * fSegmentSize ) * _outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * _outerRadius, 0, cosf( (i+1) * fSegmentSize ) * _outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize ) * _innerRadius, 0, cosf( i * fSegmentSize ) * _innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize ) * _innerRadius, 0, cosf( (i+1) * fSegmentSize ) * _innerRadius );

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*_transform * glm::vec4(v2inner, 0)).xyz();
		}

		if (_fillColour.w != 0) {
			AddTri(_center + v2outer, _center + v1outer, _center + v1inner, _fillColour);
			AddTri(_center + v1inner, _center + v2inner, _center + v2outer, _fillColour);

			AddTri(_center + v1inner, _center + v1outer, _center + v2outer, _fillColour);
			AddTri(_center + v2outer, _center + v2inner, _center + v1inner, _fillColour);
		} else {
			// line
			AddLine(_center + v1inner + _center, _center + v2inner + _center, vSolid, vSolid);
			AddLine(_center + v1outer + _center, _center + v2outer + _center, vSolid, vSolid);
		}
	}
}

void Gizmos::AddDisk(const glm::vec3& _center, 
	float _radius,
	unsigned int _segments, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform) {
	glm::vec4 vSolid = _fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * glm::pi<float>()) / _segments;

	for ( unsigned int i = 0 ; i < _segments ; ++i ) {
		glm::vec3 v1outer( sinf( i * fSegmentSize ) * _radius, 0, cosf( i * fSegmentSize ) * _radius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize ) * _radius, 0, cosf( (i+1) * fSegmentSize ) * _radius );

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*_transform * glm::vec4(v2outer, 0)).xyz();
		}

		if (_fillColour.w != 0) {
			AddTri(_center, _center + v1outer, _center + v2outer, _fillColour);
			AddTri(_center + v2outer, _center + v1outer, _center, _fillColour);
		} else {
			// line
			AddLine(_center + v1outer, _center + v2outer, vSolid, vSolid);
		}
	}
}

void Gizmos::AddArc(const glm::vec3& _center, 
	float _rotation,
	float _radius, 
	float _arcHalfAngle,
	unsigned int _segments, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform) {
	glm::vec4 vSolid = _fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * _arcHalfAngle) / _segments;

	for ( unsigned int i = 0 ; i < _segments ; ++i ) {
		glm::vec3 v1outer( sinf( i * fSegmentSize - _arcHalfAngle + _rotation ) * _radius, 0, cosf( i * fSegmentSize - _arcHalfAngle + _rotation ) * _radius);
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - _arcHalfAngle + _rotation ) * _radius, 0, cosf( (i+1) * fSegmentSize - _arcHalfAngle + _rotation ) * _radius);

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*_transform * glm::vec4(v2outer, 0)).xyz();
		}

		if (_fillColour.w != 0) {
			AddTri(_center, _center + v1outer, _center + v2outer, _fillColour);
			AddTri(_center + v2outer, _center + v1outer, _center, _fillColour);
		} else {
			// line
			AddLine(_center + v1outer, _center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (_fillColour.w == 0) {
		glm::vec3 v1outer( sinf( -_arcHalfAngle + _rotation ) * _radius, 0, cosf( -_arcHalfAngle + _rotation ) * _radius );
		glm::vec3 v2outer( sinf( _arcHalfAngle + _rotation ) * _radius, 0, cosf( _arcHalfAngle + _rotation ) * _radius );

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*_transform * glm::vec4(v2outer, 0)).xyz();
		}

		AddLine(_center, _center + v1outer, vSolid, vSolid);
		AddLine(_center, _center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::AddArcRing(const glm::vec3& _center, 
	float _rotation, 
	float _innerRadius, 
	float _outerRadius, 
	float _arcHalfAngle,
	unsigned int _segments, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform)
{
	glm::vec4 vSolid = _fillColour;
	vSolid.w = 1;

	float fSegmentSize = (2 * _arcHalfAngle) / _segments;

	for ( unsigned int i = 0 ; i < _segments ; ++i ) {
		glm::vec3 v1outer( sinf( i * fSegmentSize - _arcHalfAngle + _rotation ) * _outerRadius, 0, cosf( i * fSegmentSize - _arcHalfAngle + _rotation ) * _outerRadius );
		glm::vec3 v2outer( sinf( (i+1) * fSegmentSize - _arcHalfAngle + _rotation ) * _outerRadius, 0, cosf( (i+1) * fSegmentSize - _arcHalfAngle + _rotation ) * _outerRadius );
		glm::vec3 v1inner( sinf( i * fSegmentSize - _arcHalfAngle + _rotation  ) * _innerRadius, 0, cosf( i * fSegmentSize - _arcHalfAngle + _rotation  ) * _innerRadius );
		glm::vec3 v2inner( sinf( (i+1) * fSegmentSize - _arcHalfAngle + _rotation  ) * _innerRadius, 0, cosf( (i+1) * fSegmentSize - _arcHalfAngle + _rotation  ) * _innerRadius );

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*_transform * glm::vec4(v2inner, 0)).xyz();
		}

		if (_fillColour.w != 0) {
			AddTri(_center + v2outer, _center + v1outer, _center + v1inner, _fillColour);
			AddTri(_center + v1inner, _center + v2inner, _center + v2outer, _fillColour);

			AddTri(_center + v1inner, _center + v1outer, _center + v2outer, _fillColour);
			AddTri(_center + v2outer, _center + v2inner, _center + v1inner, _fillColour);
		} else {
			// line
			AddLine(_center + v1inner, _center + v2inner, vSolid, vSolid);
			AddLine(_center + v1outer, _center + v2outer, vSolid, vSolid);
		}
	}

	// edge lines
	if (_fillColour.w == 0) {
		glm::vec3 v1outer( sinf( -_arcHalfAngle + _rotation ) * _outerRadius, 0, cosf( -_arcHalfAngle + _rotation ) * _outerRadius );
		glm::vec3 v2outer( sinf( _arcHalfAngle + _rotation ) * _outerRadius, 0, cosf( _arcHalfAngle + _rotation ) * _outerRadius );
		glm::vec3 v1inner( sinf( -_arcHalfAngle + _rotation  ) * _innerRadius, 0, cosf( -_arcHalfAngle + _rotation  ) * _innerRadius );
		glm::vec3 v2inner( sinf( _arcHalfAngle + _rotation  ) * _innerRadius, 0, cosf( _arcHalfAngle + _rotation  ) * _innerRadius );

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer, 0)).xyz();
			v2outer = (*_transform * glm::vec4(v2outer, 0)).xyz();
			v1inner = (*_transform * glm::vec4(v1inner, 0)).xyz();
			v2inner = (*_transform * glm::vec4(v2inner, 0)).xyz();
		}

		AddLine(_center + v1inner, _center + v1outer, vSolid, vSolid);
		AddLine(_center + v2inner, _center + v2outer, vSolid, vSolid);
	}
}

void Gizmos::AddSphereFilled(const glm::vec3& _center, 
	float _radius, 
	int _rows, 
	int _columns, 
	const glm::vec4& _fillColour, 
	const glm::mat4* _transform, 
	float _longMin, 
	float _longMax, 
	float _latMin, 
	float _latMax) {

	float inverseRadius = 1/_radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.0f/float(_columns);
	float invRows = 1.0f/float(_rows);

	float DEG_2_RAD = glm::pi<float>() / 180.0f;

	//Lets put everything in radians first
	float latitiudinalRange = (_latMax - _latMin) * DEG_2_RAD;
	float longitudinalRange = (_longMax - _longMin) * DEG_2_RAD;

	// for each row of the mesh
	glm::vec3* points = new glm::vec3[_rows*_columns + _columns];

	for (int row = 0; row <= _rows; ++row) {
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis = ratioAroundXAxis * latitiudinalRange + (_latMin * DEG_2_RAD);
		float y  =  _radius * sin(radiansAboutXAxis);
		float z  =  _radius * cos(radiansAboutXAxis);
		
		for ( int col = 0; col <= _columns; ++col ) {
			float ratioAroundYAxis   = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (_longMin * DEG_2_RAD);
			glm::vec3 point( -z * sinf(theta), y, -z * cosf(theta) );
			glm::vec3 normal(inverseRadius * point.x, inverseRadius * point.y, inverseRadius * point.z);

			if (_transform != nullptr) {
				point = (*_transform * glm::vec4(point, 0)).xyz();
				normal = (*_transform * glm::vec4(normal, 0)).xyz();
			}

			int index = row * _columns + (col % _columns);
			points[index] = point;
		}
	}
	
	for (int face = 0; face < (_rows * _columns); ++face ) {
		int nextFace = face + 1;		
		
		if (nextFace  % _columns == 0) {
			nextFace -= _columns;
		}

		glm::vec3 v0Next = _center + points[nextFace];
		glm::vec3 v1Next = _center + points[nextFace + _columns];

		glm::vec3 v0 = _center + points[face];
		glm::vec3 v1 = _center + points[face + _columns];
		
		AddLine(v0, v1, glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 1.f)); // White outline
		AddLine(v1Next, v1, glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 1.f)); // White outline

		if( face % _columns == 0 && longitudinalRange < (glm::pi<float>() * 2)) {
			continue;
		}

		AddTri(v1Next, v0, v0Next, _fillColour);
		AddTri(v1Next, v1, v0, _fillColour);
	}

	delete[] points;
}

void Gizmos::AddSphere(const glm::vec3& _center, 
	float _radius, 
	int _rows, 
	int _columns, 
	const glm::vec4& _fillColour,
    const glm::mat4* _transform, 
	float _longMin, 
	float _longMax,
    float _latMin, 
	float _latMax) {


	float inverseRadius = 1 / _radius;
	//Invert these first as the multiply is slightly quicker
	float invColumns = 1.0f / float(_columns);
	float invRows = 1.0f / float(_rows);

	float DEG_2_RAD = glm::pi<float>() / 180.0f;

	//Lets put everything in radians first
	float latitiudinalRange = (_latMax - _latMin) * DEG_2_RAD;
	float longitudinalRange = (_longMax - _longMin) * DEG_2_RAD;

	// for each row of the mesh
	glm::vec3* points = new glm::vec3[_rows*_columns + _columns];

	for (int row = 0; row <= _rows; ++row) {
		// y ordinates this may be a little confusing but here we are navigating around the xAxis in GL
		float ratioAroundXAxis = float(row) * invRows;
		float radiansAboutXAxis = ratioAroundXAxis * latitiudinalRange + (_latMin * DEG_2_RAD);
		float y = _radius * sin(radiansAboutXAxis);
		float z = _radius * cos(radiansAboutXAxis);

		for (int col = 0; col <= _columns; ++col) {
			float ratioAroundYAxis = float(col) * invColumns;
			float theta = ratioAroundYAxis * longitudinalRange + (_longMin * DEG_2_RAD);
			glm::vec3 point(-z * sinf(theta), y, -z * cosf(theta));
			glm::vec3 normal(inverseRadius * point.x, inverseRadius * point.y, inverseRadius * point.z);

			if (_transform != nullptr) {
				point = (*_transform * glm::vec4(point, 0)).xyz();
				normal = (*_transform * glm::vec4(normal, 0)).xyz();
			}

			int index = row * _columns + (col % _columns);
			points[index] = point;
		}
	}

	for (int face = 0; face < (_rows * _columns); ++face) {
		int nextFace = face + 1;

		if (nextFace  % _columns == 0) {
			nextFace -= _columns;
		}

		glm::vec3 v0Next = _center + points[nextFace];
		glm::vec3 v1Next = _center + points[nextFace + _columns];

		glm::vec3 v0 = _center + points[face];
		glm::vec3 v1 = _center + points[face + _columns];

		AddLine(v0, v1, glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 1.f)); // White outline
		AddLine(v1Next, v1, glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec4(1.f, 1.f, 1.f, 1.f)); // White outline

		if (face % _columns == 0 && longitudinalRange < (glm::pi<float>() * 2)) {
			continue;
		}

	}

    delete[] points;
}
void Gizmos::AddHermiteSpline(const glm::vec3& _start, 
	const glm::vec3& _end,
	const glm::vec3& _tangentStart, 
	const glm::vec3& _tangentEnd, 
	unsigned int _segments, 
	const glm::vec4& _colour) {
	_segments = _segments > 1 ? _segments : 1;

	glm::vec3 prev = _start;

	for ( unsigned int i = 1 ; i <= _segments ; ++i ) {
		float s = i / (float)_segments;

		float s2 = s * s;
		float s3 = s2 * s;
		float h1 = (2.0f * s3) - (3.0f * s2) + 1.0f;
		float h2 = (-2.0f * s3) + (3.0f * s2);
		float h3 =  s3- (2.0f * s2) + s;
		float h4 = s3 - s2;
		glm::vec3 p = (_start * h1) + (_end * h2) + (_tangentStart * h3) + (_tangentEnd * h4);

		AddLine(prev,p,_colour,_colour);
		prev = p;
	}
}

void Gizmos::AddLine(const glm::vec3& _rv0,  
	const glm::vec3& _rv1, 
	const glm::vec4& _colour) {
	AddLine(_rv0,_rv1,_colour,_colour);
}

void Gizmos::AddLine(const glm::vec3& _rv0, 
	const glm::vec3& _rv1,
	const glm::vec4& _colour0, 
	const glm::vec4& _colour1) {
	if (sm_instance != nullptr && sm_instance->m_lineCount < sm_instance->m_maxLines) {
		Line& currentLine = sm_instance->m_lines[sm_instance->m_lineCount];
		// v0
		currentLine.v0.x = _rv0.x;
		currentLine.v0.y = _rv0.y;
		currentLine.v0.z = _rv0.z;
		currentLine.v0.w = 1;
		currentLine.v0.r = _colour0.r;
		currentLine.v0.g = _colour0.g;
		currentLine.v0.b = _colour0.b;
		currentLine.v0.a = _colour0.a;

		// v1
		currentLine.v1.x = _rv1.x;
		currentLine.v1.y = _rv1.y;
		currentLine.v1.z = _rv1.z;
		currentLine.v1.w = 1;
		currentLine.v1.r = _colour1.r;
		currentLine.v1.g = _colour1.g;
		currentLine.v1.b = _colour1.b;
		currentLine.v1.a = _colour1.a;

		sm_instance->m_lineCount++;
	}
}

void Gizmos::AddTri(const glm::vec3& _rv0, 
	const glm::vec3& _rv1, 
	const glm::vec3& _rv2, 
	const glm::vec4& _colour) {
	if (sm_instance != nullptr) {
		if (_colour.w == 1) {
			if (sm_instance->m_triCount < sm_instance->m_maxTris) {
				Tri& currentTri = sm_instance->m_tris[sm_instance->m_triCount];
				currentTri.v0.x = _rv0.x;
				currentTri.v0.y = _rv0.y;
				currentTri.v0.z = _rv0.z;
				currentTri.v0.w = 1;
				currentTri.v1.x = _rv1.x;
				currentTri.v1.y = _rv1.y;
				currentTri.v1.z = _rv1.z;
				currentTri.v1.w = 1;
				currentTri.v2.x = _rv2.x;
				currentTri.v2.y = _rv2.y;
				currentTri.v2.z = _rv2.z;
				currentTri.v2.w = 1;

				currentTri.v0.r = _colour.r;
				currentTri.v0.g = _colour.g;
				currentTri.v0.b = _colour.b;
				currentTri.v0.w = _colour.a;
				currentTri.v1.r = _colour.r;
				currentTri.v1.g = _colour.g;
				currentTri.v1.b = _colour.b;
				currentTri.v1.w = _colour.a;
				currentTri.v2.r = _colour.r;
				currentTri.v2.g = _colour.g;
				currentTri.v2.b = _colour.b;
				currentTri.v2.w = _colour.a;

				sm_instance->m_triCount++;
			}
		} else {
			if (sm_instance->m_transparentTriCount < sm_instance->m_maxTris) {
				Tri& currentTri = sm_instance->m_transparentTris[sm_instance->m_transparentTriCount];
				currentTri.v0.x = _rv0.x;
				currentTri.v0.y = _rv0.y;
				currentTri.v0.z = _rv0.z;
				currentTri.v0.w = 1;
				currentTri.v1.x = _rv1.x;
				currentTri.v1.y = _rv1.y;
				currentTri.v1.z = _rv1.z;
				currentTri.v1.w = 1;
				currentTri.v2.x = _rv2.x;
				currentTri.v2.y = _rv2.y;
				currentTri.v2.z = _rv2.z;
				currentTri.v2.w = 1;

				currentTri.v0.r = _colour.r;
				currentTri.v0.g = _colour.g;
				currentTri.v0.b = _colour.b;
				currentTri.v0.w = _colour.a;
				currentTri.v1.r = _colour.r;
				currentTri.v1.g = _colour.g;
				currentTri.v1.b = _colour.b;
				currentTri.v1.w = _colour.a;
				currentTri.v2.r = _colour.r;
				currentTri.v2.g = _colour.g;
				currentTri.v2.b = _colour.b;
				currentTri.v2.w = _colour.a;

				sm_instance->m_transparentTriCount++;
			}
		}
	}
}

void Gizmos::Add2DAABB(const glm::vec2& _center, 
	const glm::vec2& _extents, 
	const glm::vec4& _colour, 
	const glm::mat4* _transform) {
	glm::vec2 verts[4];
	glm::vec2 vX(_extents.x, 0);
	glm::vec2 vY(0, _extents.y);

	if (_transform != nullptr) {
		vX = (*_transform * glm::vec4(vX, 0, 0)).xy();
		vY = (*_transform * glm::vec4(vY, 0, 0)).xy();
	}

	verts[0] = _center - vX - vY;
	verts[1] = _center + vX - vY;
	verts[2] = _center + vX + vY;
	verts[3] = _center - vX + vY;

	Add2DLine(verts[0], verts[1], _colour, _colour);
	Add2DLine(verts[1], verts[2], _colour, _colour);
	Add2DLine(verts[2], verts[3], _colour, _colour);
	Add2DLine(verts[3], verts[0], _colour, _colour);
}

void Gizmos::Add2DAABBFilled(const glm::vec2& _center, 
	const glm::vec2& _extents, 
	const glm::vec4& _colour, 
	const glm::mat4* _transform) {	

	glm::vec2 verts[4];
	glm::vec2 vX(_extents.x, 0);
	glm::vec2 vY(0, _extents.y);

	if (_transform != nullptr) {
		vX = (*_transform * glm::vec4(vX,0,0)).xy();
		vY = (*_transform * glm::vec4(vY,0,0)).xy();
	}

	verts[0] = _center - vX - vY;
	verts[1] = _center + vX - vY;
	verts[2] = _center + vX + vY;
	verts[3] = _center - vX + vY;
	
	Add2DTri(verts[0], verts[1], verts[2], _colour);
	Add2DTri(verts[0], verts[2], verts[3], _colour);
}

void Gizmos::Add2DCircle(const glm::vec2& _center, 
	float _radius, 
	unsigned int _segments, 
	const glm::vec4& _colour, 
	const glm::mat4* _transform) {

	glm::vec4 solidColour = _colour;
	solidColour.w = 1;

	float segmentSize = (2 * glm::pi<float>()) / _segments;

	for ( unsigned int i = 0 ; i < _segments ; ++i ) {
		glm::vec2 v1outer( sinf( i * segmentSize ) * _radius, cosf( i * segmentSize ) * _radius );
		glm::vec2 v2outer( sinf( (i+1) * segmentSize ) * _radius, cosf( (i+1) * segmentSize ) * _radius );

		if (_transform != nullptr) {
			v1outer = (*_transform * glm::vec4(v1outer,0,0)).xy();
			v2outer = (*_transform * glm::vec4(v2outer,0,0)).xy();
		}

		if (_colour.w != 0) {
			Add2DTri(_center, _center + v1outer, _center + v2outer, _colour);
			Add2DTri(_center + v2outer, _center + v1outer, _center, _colour);
		} else {
			// line
			Add2DLine(_center + v1outer, _center + v2outer, solidColour, solidColour);
		}
	}
}

void Gizmos::Add2DLine(const glm::vec2& _rv0,
	const glm::vec2& _rv1, 
	const glm::vec4& _colour) {
	Add2DLine(_rv0,_rv1,_colour,_colour);
}

void Gizmos::Add2DLine(const glm::vec2& _rv0, 
	const glm::vec2& _rv1, 
	const glm::vec4& _colour0, 
	const glm::vec4& _colour1) {
	if (sm_instance != nullptr && sm_instance->m_2DlineCount < sm_instance->m_max2DLines) {
		Line& currentLine = sm_instance->m_2Dlines[sm_instance->m_2DlineCount];
		currentLine.v0.x = _rv0.x;
		currentLine.v0.y = _rv0.y;
		currentLine.v0.z = 1;
		currentLine.v0.w = 1;
		currentLine.v0.r = _colour0.r;
		currentLine.v0.g = _colour0.g;
		currentLine.v0.b = _colour0.b;
		currentLine.v0.a = _colour0.a;
		currentLine.v1.x = _rv1.x;
		currentLine.v1.y = _rv1.y;
		currentLine.v1.z = 1;
		currentLine.v1.w = 1;
		currentLine.v1.r = _colour1.r;
		currentLine.v1.g = _colour1.g;
		currentLine.v1.b = _colour1.b;
		currentLine.v1.a = _colour1.a;
		sm_instance->m_2DlineCount++;
	}
}

void Gizmos::Add2DTri(const glm::vec2& _rv0, 
	const glm::vec2& _rv1, 
	const glm::vec2& _rv2, 
	const glm::vec4& _colour) {
	if (sm_instance != nullptr) {
		if (sm_instance->m_2DtriCount < sm_instance->m_max2DTris) {
			Tri& currentTri = sm_instance->m_2Dtris[sm_instance->m_2DtriCount];
			currentTri.v0.x = _rv0.x;
			currentTri.v0.y = _rv0.y;
			currentTri.v0.z = 1;
			currentTri.v0.w = 1;
			currentTri.v1.x = _rv1.x;
			currentTri.v1.y = _rv1.y;
			currentTri.v1.z = 1;
			currentTri.v1.w = 1;
			currentTri.v2.x = _rv2.x;
			currentTri.v2.y = _rv2.y;
			currentTri.v2.z = 1;
			currentTri.v2.w = 1;
			currentTri.v0.r = _colour.r;
			currentTri.v0.g = _colour.g;
			currentTri.v0.b = _colour.b;
			currentTri.v0.a = _colour.a;
			currentTri.v1.r = _colour.r;
			currentTri.v1.g = _colour.g;
			currentTri.v1.b = _colour.b;
			currentTri.v1.a = _colour.a;
			currentTri.v2.r = _colour.r;
			currentTri.v2.g = _colour.g;
			currentTri.v2.b = _colour.b;
			currentTri.v2.a = _colour.a;

			sm_instance->m_2DtriCount++;
		}
	}
}

void Gizmos::Draw(const glm::mat4& _projection, 
	const glm::mat4& _view) {
	Draw(_projection * _view);
}

void Gizmos::Draw(const glm::mat4& _projectionView) {
	if (sm_instance != nullptr && (sm_instance->m_lineCount > 0 || sm_instance->m_triCount > 0 || sm_instance->m_transparentTriCount > 0)) {
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);

		glUseProgram(sm_instance->m_shader);

		unsigned int projectionViewUniform = glGetUniformLocation(sm_instance->m_shader, "ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(_projectionView));

		if (sm_instance->m_lineCount > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, sm_instance->m_lineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_instance->m_lineCount * sizeof(Line), sm_instance->m_lines);

			glBindVertexArray(sm_instance->m_lineVAO);
			glDrawArrays(GL_LINES, 0, sm_instance->m_lineCount * 2);
		}

		if (sm_instance->m_triCount > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, sm_instance->m_triVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_instance->m_triCount * sizeof(Tri), sm_instance->m_tris);

			glBindVertexArray(sm_instance->m_triVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_instance->m_triCount * 3);
		}

		if (sm_instance->m_transparentTriCount > 0) {
			// not ideal to store these, but Gizmos must work stand-alone
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);
			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
			int src, dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			// Setup blend states
			if (blendEnabled == GL_FALSE) {
				glEnable(GL_BLEND);
			}

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_instance->m_transparentTriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_instance->m_transparentTriCount * sizeof(Tri), sm_instance->m_transparentTris);

			glBindVertexArray(sm_instance->m_transparentTriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_instance->m_transparentTriCount * 3);

			// Reset state
			glDepthMask(depthMask);
			glBlendFunc(src, dst);
			if (blendEnabled == GL_FALSE) {
				glDisable(GL_BLEND);
			}
		}

		glUseProgram(shader);
	}
}

void Gizmos::Draw2D(const glm::mat4& _projection) {
	if (sm_instance != nullptr && (sm_instance->m_2DlineCount > 0 || sm_instance->m_2DtriCount > 0)) {
		int shader = 0;
		glGetIntegerv(GL_CURRENT_PROGRAM, &shader);
		glUseProgram(sm_instance->m_shader);

		unsigned int projectionViewUniform = glGetUniformLocation(sm_instance->m_shader, "ProjectionView");
		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(_projection));

		if (sm_instance->m_2DlineCount > 0) {
			glBindBuffer(GL_ARRAY_BUFFER, sm_instance->m_2DlineVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_instance->m_2DlineCount * sizeof(Line), sm_instance->m_2Dlines);

			glBindVertexArray(sm_instance->m_2DlineVAO);
			glDrawArrays(GL_LINES, 0, sm_instance->m_2DlineCount * 2);
		}

		if (sm_instance->m_2DtriCount > 0) {
			GLboolean blendEnabled = glIsEnabled(GL_BLEND);

			GLboolean depthMask = GL_TRUE;
			glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);

			int src, dst;
			glGetIntegerv(GL_BLEND_SRC, &src);
			glGetIntegerv(GL_BLEND_DST, &dst);

			if (blendEnabled == GL_FALSE) {
				glEnable(GL_BLEND);
			}

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glDepthMask(GL_FALSE);

			glBindBuffer(GL_ARRAY_BUFFER, sm_instance->m_2DtriVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sm_instance->m_2DtriCount * sizeof(Tri), sm_instance->m_2Dtris);

			glBindVertexArray(sm_instance->m_2DtriVAO);
			glDrawArrays(GL_TRIANGLES, 0, sm_instance->m_2DtriCount * 3);

			glDepthMask(depthMask);

			glBlendFunc(src, dst);

			if (blendEnabled == GL_FALSE) {
				glDisable(GL_BLEND);
			}
		}

		glUseProgram(shader);
	}
}