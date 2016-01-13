/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: OBB.h
@date: 16/07/2015
@author: Emmanuel Vaccaro
@brief: Collision boxes that can have an 
orientaiton applied to them.
===============================================*/

#ifndef _OBB_H_
#define _OBB_H_

// Components
#include "Transform.h"

// Structs
#include "Bounds.h"
#include "LineSegment.h"

// Utilities
#include "GLM_Header.h"

union FeaturePair {
	struct {
		unsigned int inR;
		unsigned int outR;
		unsigned int inI;
		unsigned int outI;
	};
	int key;
};

struct Contact {
	vec3 position;				// World coordinate of contact
	float penetration;			// Depth of penetration from collision
	float normalImpulse;		// Accumulated normal impulse
	float tangentImpulse[2];	// Accumulated friction impulse
	float bias;					// Restitution + baumgarte
	float normalMass;			// Normal constraint mass
	float tangentMass[2];		// Tangent constraint mass
	FeaturePair fp;				// Features on A and B for this contact
	unsigned int warmStarted;	// Used for debug rendering
};

struct ClipVertex {
	ClipVertex() { f.key = ~0; }
	vec3 v;
	FeaturePair f;
};

struct Manifold {
	Contact contacts[8];
	vec3 normal;			// From A to B
	vec3 tangentVectors[2];	// Tangent vectors
	int contactCount;
	bool sensor;
	Manifold* next;
	Manifold* prev;
};

class OBB {
public:
	OBB();
	~OBB();
	void SetFrom(const Bounds& _bounds, const Transform& _transform);
	bool Intersects(const OBB& _obb, Manifold* _manifold);
	bool TrackFaceAxis(int* _axis, int _index, float _s, float* _sMax, const vec3& _normal, vec3* _axisNormal);
	bool TrackEdgeAxis(int* _axis, int _index, float _s, float* _sMax, const vec3& _normal, vec3* _axisNormal);
	void ComputeReferenceEdgesAndBasis(const vec3& _eR, const Transform& _rtx, vec3 _n, int _axis, unsigned int* out, glm::mat3* _basis, vec3* _e);
	void ComputeIncidentFace(const Transform& _rtx, const vec3& _e, vec3 _n, ClipVertex* _out);
	int Orthographic(float _sign, float _e, int _axis, int _clipEdge, ClipVertex* _in, int _inCount, ClipVertex* _out);
	int Clip(const vec3& rPos, const vec3& _e, unsigned int* _clipEdges, const glm::mat3& _basis, ClipVertex* _incident, ClipVertex* _outVerts, float* _outDepths);
	void EdgesContact(vec3* _CA, vec3* _CB, const vec3& _PA, const vec3& _QA, const vec3& _PB, const vec3& _QB);
	void SupportEdge(const Transform& _tx, const vec3& _e, vec3 _n, vec3* _aOut, vec3* _bOut);
	float FloatSign(float _a);
	LineSegment Edge(int _edgeIndex);
	vec3 CornerPoint(int _cornerIndex);

	Transform local;
	vec3 e; 
};

#endif // _OBB_H_