/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Gizmos.h
@date: 15/06/2015
@author: Emmanuel Vaccaro
@brief: Draws primitive and complex shapes
===============================================*/

#ifndef _GIZMOS_H_
#define _GIZMOS_H_

#include <glm/fwd.hpp>

class Gizmos {
public:

	static void	Create(unsigned int _maxLines = 0xffff, 
		unsigned int _maxTris = 0xffff,
		unsigned int _max2DLines = 0xff, 
		unsigned int _max2DTris = 0xff);
	static void	Destroy();

	// Removes all Gizmos
	static void	Clear();

	// Draws current Gizmo buffers, either using a combined (projection * view) matrix, or separate matrices
	static void	Draw(const glm::mat4& _projectionView);
	static void	Draw(const glm::mat4& _projection, const glm::mat4& _view);

	// The projection matrix here should ideally be orthographic with a near of -1 and far of 1
	static void	Draw2D(const glm::mat4& _projection);

	// Adds a single debug line
	static void	AddLine(const glm::vec3& _rv0, const glm::vec3& _rv1,
		const glm::vec4& _colour);

	// Adds a single debug line
	static void	AddLine(const glm::vec3& _rv0, const glm::vec3& _rv1,
		const glm::vec4& _colour0, const glm::vec4& _colour1);

	// Adds a triangle.
	static void	AddTri(const glm::vec3& _rv0, const glm::vec3& _rv1, const glm::vec3& _rv2, const glm::vec4& _colour);

	// Adds 3 unit-length lines (red,green,blue) representing the 3 axis of a transform, 
	// at the transform's translation. Optional scale available.
	static void	AddTransform(const glm::mat4& _transform, float _fScale = 1.0f);

	// Adds a wireframe Axis-Aligned Bounding-Box with optional transform for rotation/translation.
	static void	AddAABB(const glm::vec3& _center, const glm::vec3& _extents,
		const glm::vec4& _colour, const glm::mat4* _transform = nullptr);

	// Adds an Axis-Aligned Bounding-Box with optional transform for rotation.
	static void	AddAABBFilled(const glm::vec3& _center, const glm::vec3& _extents,
		const glm::vec4& _fillColour, const glm::mat4* _transform = nullptr);

	// Adds a cylinder aligned to the Y-axis with optional transform for rotation.
	static void	AddCylinderFilled(const glm::vec3& _center, float _radius, float _fHalfLength,
		unsigned int _segments, const glm::vec4& _fillColour, const glm::mat4* _transform = nullptr);

	// Adds a double-sided hollow ring in the XZ axis with optional transform for rotation.
	// If _rvFilLColour.w == 0 then only an outer and inner line is drawn.
	static void	AddRing(const glm::vec3& _center, float _innerRadius, float _outerRadius,
		unsigned int _segments, const glm::vec4& _fillColour, const glm::mat4* _transform = nullptr);

	// Adds a double-sided disk in the XZ axis with optional transform for rotation.
	// If _rvFilLColour.w == 0 then only an outer line is drawn.
	static void	AddDisk(const glm::vec3& _center, float _radius,
		unsigned int _segments, const glm::vec4& _fillColour, const glm::mat4* _transform = nullptr);

	// Adds an arc, around the Y-axis
	// If _rvFilLColour.w == 0 then only an outer line is drawn.
	static void	AddArc(const glm::vec3& _center, float _rotation,
		float _radius, float _halfAngle,
		unsigned int _segments, const glm::vec4& _fillColour, const glm::mat4* _transform = nullptr);

	// Adds an arc, around the Y-axis, starting at the inner radius and extending to the outer radius
	// If _rvFilLColour.w == 0 then only an outer line is drawn.
	static void	AddArcRing(const glm::vec3& _center, float _rotation,
		float _innerRadius, float _outerRadius, float _arcHalfAngle,
		unsigned int _segments, const glm::vec4& _fillColour, const glm::mat4* _transform = nullptr);

	// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
	static void	AddSphere(const glm::vec3& _center, float _radius, int _rows, int _columns, const glm::vec4& _fillColour,
		const glm::mat4* _transform = nullptr, float _longMin = 0.f, float _longMax = 360,
		float _latMin = -90, float _latMax = 90);
	// Adds a Sphere at a given position, with a given number of rows, and columns, radius and a max and min long and latitude
	static void	AddSphereFilled(const glm::vec3& _center, float _radius, int _rows, int _columns, const glm::vec4& _fillColour,
		const glm::mat4* _transform = nullptr, float _longMin = 0.f, float _longMax = 360,
		float _latMin = -90, float _latMax = 90);
	// Adds a single Hermite spline curve
	static void	AddHermiteSpline(const glm::vec3& _start, const glm::vec3& _end,
		const glm::vec3& _tangentStart, const glm::vec3& _tangentEnd, unsigned int _segments, const glm::vec4& _colour);

	static void AddCapsule(const glm::vec3 center, const float length, const float radius, const int rows, const int cols, const glm::vec4 color,
		const glm::vec3 direction, const glm::mat4* rotation = 0);
	// 2-dimensional gizmos
	static void	Add2DLine(const glm::vec2& _start, const glm::vec2& _end, const glm::vec4& _colour);
	static void	Add2DLine(const glm::vec2& _start, const glm::vec2& _end, const glm::vec4& _colour0, const glm::vec4& _colour1);
	static void	Add2DTri(const glm::vec2& _v0, const glm::vec2& _v1, const glm::vec2& _v2, const glm::vec4& _colour);
	static void	Add2DAABB(const glm::vec2& _center, const glm::vec2& _extents, const glm::vec4& _colour, const glm::mat4* _transform = nullptr);
	static void	Add2DAABBFilled(const glm::vec2& _center, const glm::vec2& _extents, const glm::vec4& _colour, const glm::mat4* _transform = nullptr);
	static void	Add2DCircle(const glm::vec2& _center, float _radius, unsigned int _segments, const glm::vec4& _colour, const glm::mat4* _transform = nullptr);

private:

	Gizmos(unsigned int _maxLines, unsigned int _maxTris,
		unsigned int _max2DLines, unsigned int _max2DTris);
	~Gizmos();

	struct Vertex {
		float x, y, z, w;
		float r, g, b, a;
	};

	struct Line {
		Vertex v0;
		Vertex v1;
	};

	struct Tri {
		Vertex v0;
		Vertex v1;
		Vertex v2;
	};

	// Shader
	unsigned int m_shader;

	// Line data
	unsigned int m_maxLines;
	unsigned int m_lineCount;
	unsigned int m_lineVAO;
	unsigned int m_lineVBO;
	Line* m_lines;

	// Triangle data
	unsigned int m_maxTris;
	unsigned int m_triCount;
	unsigned int m_triVAO;
	unsigned int m_triVBO;
	Tri* m_tris;

	// Transparent triangle data
	unsigned int m_transparentTriCount;
	unsigned int m_transparentTriVAO;
	unsigned int m_transparentTriVBO;
	Tri* m_transparentTris;

	// 2D line data
	unsigned int m_max2DLines;
	unsigned int m_2DlineCount;
	unsigned int m_2DlineVAO;
	unsigned int m_2DlineVBO;
	Line* m_2Dlines;

	// 2D triangle data
	unsigned int m_max2DTris;
	unsigned int m_2DtriCount;
	unsigned int m_2DtriVAO;
	unsigned int m_2DtriVBO;
	Tri* m_2Dtris;

	// Singleton instance
	static Gizmos* sm_instance;
};

#endif //_GIZMOS_H_