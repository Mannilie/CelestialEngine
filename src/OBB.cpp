#include "OBB.h"

OBB::OBB(){}
OBB::~OBB(){}
void OBB::SetFrom(const Bounds& _bounds, const Transform& _transform) {
	local = _transform;
	e = _bounds.size; //Obtain half-size
}

bool OBB::Intersects(const OBB& _obb, Manifold* _manifold) {
	Transform atx = this->local;
	Transform btx = _obb.local;
	glm::mat3 atxRotation = glm::toMat3(atx.rotation);
	glm::mat3 btxRotation = glm::toMat3(btx.rotation);
	vec3 eA = this->e;
	vec3 eB = _obb.e;

	glm::mat3 C = atxRotation * btxRotation;

	// Generate a rotation matrix that transfroms from world space to this OBB's coordinate space
	glm::mat3 absC;
	bool parallel = false;
	const float kCosTol = float(1.0e-6);
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			float val = glm::abs(C[i][j]);
			absC[i][j] = val;

			if (val + kCosTol >= 1.0f) {
				parallel = true;
			}
		}
	}

	vec3 t = C * (btx.position - atx.position);
	float s;
	float aMax = -FLT_MAX;
	float bMax = -FLT_MAX;
	float eMax = -FLT_MAX;
	int aAxis = -1;
	int bAxis = -1;
	int eAxis = -1;
	vec3 nA;
	vec3 nB;
	vec3 nE;

	// Test the three major axis of this OBB
	for (int i = 0; i < 3; ++i) {
		s = glm::abs(t[i]) - (eA[i] + glm::dot(absC[i], eB));
		if (TrackFaceAxis(&aAxis, i, s, &aMax, atxRotation[i], &nA)) {
			return false;
		}
	}

	// Test the three major axis of the OBB _obb
	for (int i = 0; i < 3; ++i) {
		s = glm::abs(glm::dot(t, absC[i])) - (eB[i] + glm::dot(absC[i], eA));
		if (TrackFaceAxis(&bAxis, i + 3, s, &bMax, btxRotation[i], &nB)) {
			return false;
		}
	}

	if (!parallel) {
		float Ra;
		float Rb;
		// A.x <cross> _obb.x
		Ra = eA.y *	absC[2][0] + eA.z * absC[1][0];
		Rb = eB.y * absC[0][2] + eB.z * absC[0][1];
		s = glm::abs(t.z * C[1][0] - t.y * C[2][0]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 6, s, &eMax, vec3(0.0f, -C[2][0], C[1][0]), &nE)) {
			return false;
		}
		// A.x < cross> _obb.y
		Ra = eA.y *	absC[2][1] + eA.z * absC[1][1];
		Rb = eB.x * absC[0][2] + eB.z * absC[0][0];
		s = glm::abs(t.z * C[1][1] - t.y * C[2][1]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 7, s, &eMax, vec3(0.0f, -C[2][1], C[1][1]), &nE)) {
			return false;
		}
		// A.x <ceAoss> _obb.z
		Ra = eA.y *	absC[2][2] + eA.z * absC[1][2];
		Rb = eB.x * absC[0][1] + eB.y * absC[0][0];
		s = glm::abs(t.z * C[1][2] - t.y * C[2][2]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 8, s, &eMax, vec3(0.0f, -C[2][2], C[1][2]), &nE)) {
			return false;
		}
		// A.y <cross> _obb.x
		Ra = eA.x * absC[2][0] + eA.z * absC[0][0];
		Rb = eB.y * absC[1][2] + eB.z * absC[1][1];
		s = glm::abs(t.x * C[2][0] - t.z * C[0][0]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 9, s, &eMax, vec3(C[2][0], 0.0f, -C[0][0]), &nE)) {
			return false;
		}
		// A.y <cross> _obb.y
		Ra = eA.x *	absC[2][1] + eA.z * absC[0][1];
		Rb = eB.x * absC[1][2] + eB.z * absC[1][0];
		s = glm::abs(t.x * C[2][1] - t.z * C[0][1]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 10, s, &eMax, vec3(C[2][1], 0.0f, -C[0][1]), &nE)) {
			return false;
		}
		// A.y <cross> _obb.z
		Ra = eA.x *	absC[2][2] + eA.z * absC[0][2];
		Rb = eB.x * absC[1][1] + eB.y * absC[1][0];
		s = glm::abs(t.x * C[2][2] - t.z * C[0][2]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 11, s, &eMax, vec3(C[2][2], 0.0f, -C[0][2]), &nE)) {
			return false;
		}
		// A.z <cross> _obb.x
		Ra = eA.x *	absC[1][0] + eA.y * absC[0][0];
		Rb = eB.y * absC[2][2] + eB.z * absC[2][1];
		s = glm::abs(t.y * C[0][0] - t.x * C[1][0]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 12, s, &eMax, vec3(-C[1][0], C[0][0], 0.0f), &nE)) {
			return false;
		}
		// A.z <cross> _obb.y
		Ra = eA.x *	absC[1][1] + eA.y * absC[0][1];
		Rb = eB.x * absC[2][2] + eB.z * absC[2][0];
		s = glm::abs(t.y * C[0][1] - t.x * C[1][1]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 13, s, &eMax, vec3(-C[1][1], C[0][1], 0.0f), &nE)) {
			return false;
		}
		// A.z <cross> _obb.z
		Ra = eA.x *	absC[1][2] + eA.y * absC[0][2];
		Rb = eB.x * absC[2][1] + eB.y * absC[2][0];
		s = glm::abs(t.y * C[0][2] - t.x * C[1][2]) - (Ra + Rb);
		if (TrackEdgeAxis(&eAxis, 14, s, &eMax, vec3(-C[1][2], C[0][2], 0.0f), &nE)) {
			return false;
		}
	}
	const float kRelTol = 0.95f;
	const float kAbsTol = 0.01f;
	int axes;
	float sMax;
	vec3 n;
	float faceMax = glm::max(aMax, bMax);
	if (kRelTol * eMax < faceMax + kAbsTol) {
		axes = eAxis;
		sMax = eMax;
		n = nE;
	} else {
		if (kRelTol * bMax > aMax + kAbsTol) {
			axes = bAxis;
			sMax = bMax;
			n = nB;
		} else {
			axes = aAxis;
			sMax = aMax;
			n = nA;
		}
	}

	if (glm::dot(n, btx.position - atx.position) < 0.0f) {
		n = -n;
	}

	if (axes < 6) {
		Transform rtx;
		Transform itx;
		vec3 eR;
		vec3 eI;
		bool flip;

		if (axes < 3) {
			rtx = atx;
			itx = btx;
			eR = eA;
			eI = eB;
			flip = false;
		} else {
			rtx = btx;
			itx = atx;
			eR = eB;
			eI = eA;
			flip = true;
			n = -n;
		}

		// Compute reference and incident edge information necessary for clipping
		ClipVertex incident[4];
		ComputeIncidentFace(itx, eI, n, incident);
		unsigned int clipEdges[4];
		glm::mat3 basis;
		vec3 e;
		ComputeReferenceEdgesAndBasis(eR, rtx, n, axes, clipEdges, &basis, &e);

		// Clip the incident face against the reference face side planes
		ClipVertex out[8];
		float depths[8];
		int outNum;
		outNum = Clip(rtx.position, e, clipEdges, basis, incident, out, depths);

		if (outNum) {
			_manifold->contactCount = outNum;
			_manifold->normal = flip ? -n : n;
			for (int i = 0; i < outNum; ++i) {
				Contact* c = _manifold->contacts + i;
				FeaturePair pair = out[i].f;
				if (flip) {
					std::swap(pair.inI, pair.inR);
					std::swap(pair.outI, pair.outR);
				}
				c->fp = out[i].f;
				c->position = out[i].v;
				c->penetration = depths[i];
			}
		}
	} else {
		n = atxRotation * n;
		if (glm::dot(n, btx.position - atx.position) < 0.0f) {
			n = -n;
		}
		vec3 PA, QA;
		vec3 PB, QB;
		SupportEdge(atx, eA, n, &PA, &QA);
		SupportEdge(btx, eB, -n, &PB, &QB);
		vec3 CA, CB;
		EdgesContact(&CA, &CB, PA, QA, PB, QB);
		_manifold->normal = n;
		_manifold->contactCount = 1;
		Contact* c = _manifold->contacts;
		FeaturePair pair;
		pair.key = axes;
		c->fp = pair;
		c->penetration = sMax;
		c->position = (CA + CB) * 0.5f;
	}

	// Note(Manny): Intersect info needs to be computed here

	// No separating axis exists, so the two OBB don't intersect.
	return true;
}
bool OBB::TrackFaceAxis(int* _axis, int _index, float _s, float* _sMax, const vec3& _normal, vec3* _axisNormal) {
	if (_s > 0.0f) {
		return true;
	}
	if (_s > *_sMax) {
		*_sMax = _s;
		*_axis = _index;
		*_axisNormal = _normal;
	}
	return false;
}
bool OBB::TrackEdgeAxis(int* _axis, int _index, float _s, float* _sMax, const vec3& _normal, vec3* _axisNormal) {
	if (_s > 0.0f) {
		return true;
	}
	float l = 1.0f / glm::length(_normal);
	_s *= l;
	if (_s > *_sMax) {
		*_sMax = _s;
		*_axis = _index;
		*_axisNormal = _normal * l;
	}
	return false;
}
void OBB::ComputeReferenceEdgesAndBasis(const vec3& _eR, const Transform& _rtx, vec3 _n, int _axis,
	unsigned int* out, glm::mat3* _basis, vec3* _e) {
	glm::mat3 rotation = glm::toMat3(_rtx.rotation);
	vec3 r = rotation * _eR;
	_n = r * _n;
	if (_axis >= 3) {
		_axis -= 3;
	}
	switch (_axis) {
	case 0:
		if (_n.x > 0.0f) {
			out[0] = 1;
			out[1] = 8;
			out[2] = 7;
			out[3] = 9;
			*_e = vec3(_eR.y, _eR.z, _eR.x);
			*_basis = glm::mat3(r[1], r[2], r[0],
				r[1], r[2], r[0],
				r[1], r[2], r[0]);
		} else {
			out[0] = 11;
			out[1] = 3;
			out[2] = 10;
			out[3] = 5;
			*_e = vec3(_eR.z, _eR.y, _eR.x);
			*_basis = glm::mat3(r[2], r[1], r[0],
				r[2], r[1], r[0],
				r[2], r[1], r[0]);
		} break;
	case 1:
		if (_n.y > 0.0f) {
			out[0] = 0;
			out[1] = 1;
			out[2] = 2;
			out[3] = 3;
			*_e = vec3(_eR.z, _eR.x, _eR.y);
			*_basis = glm::mat3(r[2], r[0], r[1],
				r[2], r[0], r[1],
				r[2], r[0], r[1]);
		} else {
			out[0] = 4;
			out[1] = 5;
			out[2] = 6;
			out[3] = 7;
			*_e = vec3(_eR.z, _eR.x, _eR.y);
			*_basis = glm::mat3(r[2], r[0], r[1],
				r[2], r[0], r[1],
				r[2], r[0], r[1]);
		} break;
	case 2:
		if (_n.z > 0.0f) {
			out[0] = 11;
			out[1] = 4;
			out[2] = 8;
			out[3] = 0;
			*_e = vec3(_eR.y, _eR.x, _eR.z);
			*_basis = glm::mat3(r[1], r[0], r[2],
				r[1], r[0], r[2],
				r[1], r[0], r[2]);
		} else {
			out[0] = 6;
			out[1] = 10;
			out[2] = 2;
			out[3] = 9;
			*_e = vec3(_eR.y, _eR.x, _eR.z);
			*_basis = glm::mat3(r[1], r[0], r[2],
				r[1], r[0], r[2],
				r[1], r[0], r[2]);
		} break;
	default:
		break;
	}
}
void OBB::ComputeIncidentFace(const Transform& _rtx, const vec3& _e, vec3 _n, ClipVertex* _out) {
	_n = -(glm::toMat3(_rtx.rotation) * _n);
	vec3 absN = glm::abs(_n);
	if (absN.x > absN.y && absN.x > absN.z) {
		if (_n.x > 0.0f) {
			_out[0].v = vec3(_e.x,  _e.y, -_e.z);
			_out[1].v = vec3(_e.x,  _e.y,  _e.z);
			_out[2].v = vec3(_e.x, -_e.y,  _e.z);
			_out[3].v = vec3(_e.x, -_e.y, -_e.z);
			_out[0].f.inI = 9;
			_out[0].f.outI = 1;
			_out[1].f.inI = 1;
			_out[1].f.outI = 8;
			_out[2].f.inI = 8;
			_out[2].f.outI = 7;
			_out[3].f.inI = 7;
			_out[3].f.outI = 9;
		} else {
			_out[0].v = vec3(-_e.x, -_e.y,  _e.z);
			_out[1].v = vec3(-_e.x,  _e.y,  _e.z);
			_out[2].v = vec3(-_e.x,  _e.y, -_e.z);
			_out[3].v = vec3(-_e.x, -_e.y, -_e.z);
			_out[0].f.inI = 5;
			_out[0].f.outI = 11;
			_out[1].f.inI = 11;
			_out[1].f.outI = 3;
			_out[2].f.inI = 3;
			_out[2].f.outI = 10;
			_out[3].f.inI = 10;
			_out[3].f.outI = 5;
		}
	} else if (absN.y > absN.x && absN.y > absN.z) {
		if (_n.y > 0.0f) {
			_out[0].v = vec3(-_e.x, _e.y,  _e.z);
			_out[1].v = vec3( _e.x, _e.y,  _e.z);
			_out[2].v = vec3( _e.x, _e.y, -_e.z);
			_out[3].v = vec3(-_e.x, _e.y, -_e.z);
			_out[0].f.inI = 3;
			_out[0].f.outI = 0;
			_out[1].f.inI = 0;
			_out[1].f.outI = 1;
			_out[2].f.inI = 1;
			_out[2].f.outI = 2;
			_out[3].f.inI = 2;
			_out[3].f.outI = 3;
		} else {
			_out[0].v = vec3( _e.x, -_e.y,  _e.z);
			_out[1].v = vec3(-_e.x, -_e.y,  _e.z);
			_out[2].v = vec3(-_e.x, -_e.y, -_e.z);
			_out[3].v = vec3( _e.x, -_e.y, -_e.z);
			_out[0].f.inI = 7;
			_out[0].f.outI = 4;
			_out[1].f.inI = 4;
			_out[1].f.outI = 5;
			_out[2].f.inI = 5;
			_out[2].f.outI = 6;
			_out[3].f.inI = 5;
			_out[3].f.outI = 6;
		}
	} else {
		if (_n.z > 0.0) {
			_out[0].v = vec3(-_e.x,  _e.y, _e.z);
			_out[1].v = vec3(-_e.x, -_e.y, _e.z);
			_out[2].v = vec3( _e.x, -_e.y, _e.z);
			_out[3].v = vec3( _e.x,  _e.y, _e.z);
			_out[0].f.inI = 0;
			_out[0].f.outI = 11;
			_out[1].f.inI = 11;
			_out[1].f.outI = 4;
			_out[2].f.inI = 4;
			_out[2].f.outI = 8;
			_out[3].f.inI = 8;
			_out[3].f.outI = 0;
		} else {
			_out[0].v = vec3( _e.x, -_e.y, -_e.z);
			_out[1].v = vec3(-_e.x, -_e.y, -_e.z);
			_out[2].v = vec3(-_e.x,  _e.y, -_e.z);
			_out[3].v = vec3( _e.x,  _e.y, -_e.z);
			_out[0].f.inI = 9;
			_out[0].f.outI = 6;
			_out[1].f.inI = 6;
			_out[1].f.outI = 10;
			_out[2].f.inI = 10;
			_out[2].f.outI = 2;
			_out[3].f.inI = 2;
			_out[3].f.outI = 9;
		}
	}
	for (int i = 0; i < 4; ++i) {
		_out[i].v = glm::toMat3(_rtx.rotation) * _out[i].v + _rtx.position;
	}
}

#define InFront( a ) \
	((a) < 0.0f)

#define Behind( a ) \
	((a) >= 0.0)

#define On( a ) \
	((a) < 0.005f && (a) > -0.005f)

int OBB::Orthographic(float _sign, float _e, int _axis, int _clipEdge, ClipVertex* _in, int _inCount, ClipVertex* _out) {
	int outCount = 0;
	ClipVertex a = _in[_inCount - 1];

	for (int i = 0; i < _inCount; ++i) {
		ClipVertex b = _in[i];
		float da = _sign * a.v[_axis] - _e;
		float db = _sign * b.v[_axis] - _e;
		ClipVertex cv;
		
		if (((InFront(da) && InFront(db)) || On(da) || On(db))) {
			// B
			assert(outCount < 8);
			_out[outCount++] = b;
		} else if (InFront(da) && Behind(db)) {
			// I
			cv.f = b.f;
			cv.v = a.v + (b.v - a.v) * (da / (da - db));
			cv.f.outR = _clipEdge;
			cv.f.outI = 0;
			assert(outCount < 8);
			_out[outCount++] = cv;
		} else if (Behind(da) && InFront(db)) {
			// I, B
			cv.f = a.f;
			cv.v = a.v + (b.v - a.v) * (da / (da - db));
			cv.f.inR = _clipEdge;
			cv.f.inI = 0;
			assert(outCount < 8);
			_out[outCount++] = cv;
			assert(outCount < 8);
			_out[outCount++] = b;
		}
		a = b;
	}
	return outCount;
}

int OBB::Clip(const vec3& rPos, const vec3& _e, unsigned int* _clipEdges, const glm::mat3& _basis, 
	ClipVertex* _incident, ClipVertex* _outVerts, float* _outDepths) {
	int inCount = 4;
	int outCount;
	ClipVertex in[8];
	ClipVertex out[8];
	for (int i = 0; i < 4; ++i) {
		in[i].v = glm::transpose(_basis) * (_incident[i].v - rPos);
	}
	outCount = Orthographic(1.0f, _e.x, 0, _clipEdges[0], in, inCount, out);
	if (!outCount) {
		return 0;
	}
	inCount = Orthographic(1.0f, _e.y, 1, _clipEdges[1], out, outCount, in);
	if (!inCount) {
		return 0;
	}
	outCount = Orthographic(-1.0f, _e.x, 0, _clipEdges[2], in, inCount, out);
	if (!outCount) {
		return 0;
	}
	inCount = Orthographic(-1.0f, _e.y, 1, _clipEdges[3], out, outCount, in);

	// Keep incident vertices behind the reference face
	outCount = 0;
	for (int i = 0; i < inCount; ++i) {
		float d = in[i].v.z - _e.z;
		if (d <= 0.0f) {
			_outVerts[outCount].v = (_basis * in[i].v) + rPos;
			_outVerts[outCount].f = in[i].f;
			_outDepths[outCount++] = d;
		}
	}
	assert(outCount <= 8);
	return outCount;
}
void OBB::EdgesContact(vec3* _CA, vec3* _CB, const vec3& _PA, const vec3& _QA, const vec3& _PB, const vec3& _QB) {
	vec3 DA = _QA - _PA;
	vec3 DB = _QB - _PB;
	vec3 r = _PA - _PB;
	float  a = glm::dot(DA, DA);
	float  e = glm::dot(DB, DB);
	float  f = glm::dot(DB, r);
	float  c = glm::dot(DA, r);

	float b = glm::dot(DA, DB);
	float denom = a * e - b * b;

	float TA = (b * f - c * e) / denom;
	float TB = (b * TA + f) / e;

	*_CA = _PA + DA * TA;
	*_CB = _PB + DB * TB;
} 
void OBB::SupportEdge(const Transform& _tx, const vec3& _e, vec3 _n, vec3* _aOut, vec3* _bOut) {
	_n = glm::toMat3(_tx.rotation) * _n;
	vec3 absN = glm::abs(_n);
	vec3 a, b;
	// x > y
	if (absN.x > absN.y) {
		// x > y > z
		if (absN.y > absN.z) {
			a = vec3(_e.x, _e.y,  _e.z);
			b = vec3(_e.x, _e.y, -_e.z);
		} else { // x > z > y || z > x > y
			a = vec3(_e.x,  _e.y, _e.z);
			b = vec3(_e.x, -_e.y, _e.z);
		}
	} else {
		// y > x > z
		if (absN.x > absN.z) {
			a = vec3(_e.x, _e.y,  _e.z);
			b = vec3(_e.x, _e.y, -_e.z);
		} else { // z > y > x || y > z > x

			a = vec3( _e.x, _e.y, _e.z);
			b = vec3(-_e.x, _e.y, _e.z);
		}
	}

	float signx = FloatSign(_n.x);
	float signy = FloatSign(_n.y);
	float signz = FloatSign(_n.z);

	a.x *= signx;
	a.y *= signy;
	a.z *= signz;
	b.x *= signx;
	b.y *= signy;
	b.z *= signz;

	*_aOut = glm::toMat3(_tx.rotation) * a + _tx.position;
	*_bOut = glm::toMat3(_tx.rotation) * b + _tx.position;
}
float OBB::FloatSign(float _a) {
	if (_a >= 0.0f) {
		return 1.0f;
	} else {
		return -1.0f;
	}
}

LineSegment OBB::Edge(int _edgeIndex) {
	switch (_edgeIndex) {
		case 0: return LineSegment(CornerPoint(0), CornerPoint(1));
		case 1: return LineSegment(CornerPoint(0), CornerPoint(2));
		case 2: return LineSegment(CornerPoint(0), CornerPoint(4));
		case 3: return LineSegment(CornerPoint(1), CornerPoint(3));
		case 4: return LineSegment(CornerPoint(1), CornerPoint(5));
		case 5: return LineSegment(CornerPoint(2), CornerPoint(3));
		case 6: return LineSegment(CornerPoint(2), CornerPoint(6));
		case 7: return LineSegment(CornerPoint(3), CornerPoint(7));
		case 8: return LineSegment(CornerPoint(4), CornerPoint(5));
		case 9: return LineSegment(CornerPoint(4), CornerPoint(6));
		case 10: return LineSegment(CornerPoint(5), CornerPoint(7));
		case 11: return LineSegment(CornerPoint(6), CornerPoint(7));
		default:
			break;
	}
	return LineSegment();
}
vec3 OBB::CornerPoint(int _cornerIndex) {
	switch (_cornerIndex) {
		case 0: return local.position - e.x * local.right - e.y * local.up - e.z * local.forward;
		case 1: return local.position - e.x * local.right - e.y * local.up + e.z * local.forward;
		case 2: return local.position - e.x * local.right + e.y * local.up - e.z * local.forward;
		case 3: return local.position - e.x * local.right + e.y * local.up + e.z * local.forward;
		case 4: return local.position + e.x * local.right - e.y * local.up - e.z * local.forward;
		case 5: return local.position + e.x * local.right - e.y * local.up + e.z * local.forward;
		case 6: return local.position + e.x * local.right + e.y * local.up - e.z * local.forward;
		case 7: return local.position + e.x * local.right + e.y * local.up + e.z * local.forward;
		default:
			break;
	}
	return vec3(0);
}
