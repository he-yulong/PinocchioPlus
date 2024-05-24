#pragma once
#include "transform.h"

template<class Real = double>
class DualQuaternion {
public:
	union {
		struct {
			Quaternion<Real> real;
			Quaternion<Real> dual;
		};
		float v[8];
	};
	// Default constructor
	inline DualQuaternion() : real(0, 0, 0, 1), dual(0, 0, 0, 0) { }
	// Parameterized constructor
	inline DualQuaternion(const Quaternion<double>& r, const Quaternion<double>& d) :
		real(r), dual(d) { }
	// Copy constructor
	inline DualQuaternion(const DualQuaternion& dq) : real(dq.real), dual(dq.dual) { }
	// Assignment operator
	DualQuaternion& operator=(const DualQuaternion& dq) {
		if (this != &dq) {
			real = dq.real;
			dual = dq.dual;
		}
		return *this;
	}
};

template<class Real = double>
DualQuaternion<Real> operator+(const DualQuaternion<Real>& l, const DualQuaternion<Real>& r) {
	return DualQuaternion<Real>(l.real + r.real, l.dual + r.dual);
}
template<class Real = double>
DualQuaternion<Real> operator*(const DualQuaternion<Real>& dq, double f) {
	return DualQuaternion<Real>(dq.real * f, dq.dual * f);
}
// Multiplication order is left to right
// Left to right is the OPPOSITE of matrices and quaternions
template<class Real = double>
DualQuaternion<Real> operator*(const DualQuaternion<Real>& l, const DualQuaternion<Real>& r) {
	DualQuaternion<Real> lhs = normalized(l);
	DualQuaternion<Real> rhs = normalized(r);

	return DualQuaternion<Real>(lhs.real * rhs.real, lhs.real * rhs.dual + lhs.dual * rhs.real);
}
template<class Real = double>
bool operator==(const DualQuaternion<Real>& l, const DualQuaternion<Real>& r) {
	return l.real == r.real && l.dual == r.dual;
}
template<class Real = double>
bool operator!=(const DualQuaternion<Real>& l, const DualQuaternion<Real>& r) {
	return l.real != r.real || l.dual != r.dual;
}
template<class Real = double>
float dot(const DualQuaternion<Real>& l, const DualQuaternion<Real>& r) {
	return dot(l.real, r.real);
}
template<class Real = double>
DualQuaternion<Real> conjugate(const DualQuaternion<Real>& dq) {
	return DualQuaternion<Real>(conjugate(dq.real), conjugate(dq.dual));
}
template<class Real = double>
DualQuaternion<Real> normalized(const DualQuaternion<Real>& dq) {
	float magSq = dot(dq.real, dq.real);
	if (magSq < 0.000001f) {
		return DualQuaternion();
	}
	float invMag = 1.0f / sqrtf(magSq);

	return DualQuaternion<Real>(dq.real * invMag, dq.dual * invMag);
}
template<class Real = double>
void normalize(DualQuaternion<Real>& dq) {
	float magSq = dot(dq.real, dq.real);
	if (magSq < 0.000001f) return;
	float invMag = 1.0f / sqrtf(magSq);
	dq.real = dq.real * invMag;
	dq.dual = dq.dual * invMag;
}
template<class Real = double>
DualQuaternion<Real> transformToDualQuat(const Transform<Real>& t) {
	Quaternion<Real> d(t.trans[0], t.trans[1], t.trans[2], 0);

	Quaternion<Real> qr = t.getRot();
	Quaternion<Real> qd = qr * d * 0.5f;

	return DualQuaternion<Real>(qr, qd);
}
template<class Real = double>
Transform<Real> dualQuatToTransform(const DualQuaternion<Real>& dq) {
	Transform<Real> result;

	result.rotation = dq.real;

	Quaternion<Real> d = conjugate(dq.real) * (dq.dual * 2.0f);
	result.position = vec3(d.x, d.y, d.z);

	return result;
}
template<class Real = double>
Vector3 transformVector(const DualQuaternion<Real>& dq, const Vector3& v) {
	return dq.real * v;
}
template<class Real = double>
Vector3 transformPoint(const DualQuaternion<Real>& dq, const Vector3& v) {
	Quaternion<Real> d = conjugate(dq.real) * (dq.dual * 2.0f);
	Vector3 t = d.v;
	return dq.real * v + t;
}
