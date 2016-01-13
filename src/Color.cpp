#include "Color.h"

float Color::gammaCurve = 2.2f;
Color Color::red = Color(1, 0, 0, 1);
Color Color::green = Color(0, 1, 0, 1);
Color Color::blue = Color(0, 0, 1, 1);
Color Color::black = Color(0, 0, 0, 1);
Color Color::white = Color(1, 1, 1, 1);
Color Color::clear = Color(0, 0, 0, 0);
Color Color::cyan = Color(0, 1, 1, 1);
Color Color::gray = Color(0.5f, 0.5f, 0.5f, 1);
Color Color::magenta = Color(1, 0, 1, 1);
Color Color::yellow = Color(1, 0.92f, 0.016f, 1);

Color Color::Lerp(const Color& _from, const Color& _to, float _t) {
	Color res;
	res.r = (1 - _t) * _from.r + _t * _to.r;
	res.g = (1 - _t) * _from.g + _t * _to.g;
	res.b = (1 - _t) * _from.b + _t * _to.b;
	res.a = (1 - _t) * _from.a + _t * _to.a;
	return res;
}
float Color::Max(Color& _color) {
	float max = FLT_MIN;
	int maxIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (_color[i] > max)
			maxIndex = i;
	}
	return _color[maxIndex];
}
float Color::Max(float _r, float _g, float _b, float _a) {
	Color color = Color(_r, _g, _b, _a);
	float max = FLT_MIN;
	int maxIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (color[i] > max) {
			maxIndex = i;
		}
	}
	return color[maxIndex];
}
float Color::Min(Color& _color) {
	float min = FLT_MAX;
	int minIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (_color[i] > min) {
			minIndex = i;
		}
	}
	return _color[minIndex];
}
float Color::Min(float _r, float _g, float _b, float _a) {
	Color color = Color(_r, _g, _b, _a);
	float min = FLT_MAX;
	int minIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (color[i] > min) {
			minIndex = i;
		}
	}
	return color[minIndex];
}

Color::Color() : r(1), g(1), b(1), a(1) {}
Color::Color(float _c) : r(_c), g(_c), b(_c), a(_c){}
Color::Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
Color::~Color(){}
float Color::operator[](int _colorIndex) {
	switch (_colorIndex) {
		case 0: return r;
		case 1: return g;
		case 2: return b;
		case 3:	return a;
	}
	return -1;
}
Color Color::operator=(Color& _rhs) {
	r = _rhs.r;
	g = _rhs.g;
	b = _rhs.b;
	a = _rhs.a;
	return *this;
}
Color Color::operator=(vec4& _rhs) {
	r = _rhs.x;
	g = _rhs.y;
	b = _rhs.z;
	a = _rhs.w;
	return *this;
}
Color Color::operator-(Color& _rhs) {
	Color result;
	result.r = r - _rhs.r;
	result.g = g - _rhs.g;
	result.b = b - _rhs.b;
	result.a = a - _rhs.a;
	return result;
}
Color Color::operator*(Color& _rhs) {
	Color result;
	result.r = r * _rhs.r;
	result.g = g * _rhs.g;
	result.b = b * _rhs.b;
	result.a = a * _rhs.a;
	return result;
}
Color Color::operator/(Color& _rhs) {
	Color result;
	result.r = r / _rhs.r;
	result.g = g / _rhs.g;
	result.b = b / _rhs.b;
	result.a = a / _rhs.a;
	return result;
}
Color Color::operator+(Color& _rhs) {
	Color result;
	result.r = r + _rhs.r;
	result.g = g + _rhs.g;
	result.b = b + _rhs.b;
	result.a = a + _rhs.a;
	return result;
}
Color Color::operator-=(Color& _rhs) {
	r -= _rhs.r;
	g -= _rhs.g;
	b -= _rhs.b;
	a -= _rhs.a;
	return *this;
}
Color Color::operator*=(Color& _rhs) {
	r *= _rhs.r;
	g *= _rhs.g;
	b *= _rhs.b;
	a *= _rhs.a;
	return *this;
}
Color Color::operator/=(Color& _rhs) {
	r /= _rhs.r;
	g /= _rhs.g;
	b /= _rhs.b;
	a /= _rhs.a;
	return *this;
}
Color Color::operator+=(Color& _rhs) {
	r += _rhs.r;
	g += _rhs.g;
	b += _rhs.b;
	a += _rhs.a;
	return *this;
}
Color Color::ToGamma(float _gammaCurve) {
	Color finalGamma;
	finalGamma.r = 255.0f * glm::pow(r / 255.0f, _gammaCurve);
	finalGamma.g = 255.0f * glm::pow(g / 255.0f, _gammaCurve);
	finalGamma.b = 255.0f * glm::pow(b / 255.0f, _gammaCurve);
	finalGamma.a = 255.0f * glm::pow(a / 255.0f, _gammaCurve);
	return finalGamma;
}

Color Color::ToLinear(float _gammaCurve) {
	Color finalGamma;
	finalGamma.r = 255.0f * glm::pow(r / 255.0f, 1 / _gammaCurve);
	finalGamma.g = 255.0f * glm::pow(g / 255.0f, 1 / _gammaCurve);
	finalGamma.b = 255.0f * glm::pow(b / 255.0f, 1 / _gammaCurve);
	finalGamma.a = 255.0f * glm::pow(a / 255.0f, 1 / _gammaCurve);
	return finalGamma;
}
Color Color::ToGreyScale() {
	Color res = Color(GetGreyScale());
	res.a = a;
	return res;
}
Color Color::ToLightness() {
	Color res = Color(GetLightness());
	res.a = a;
	return res;
}
Color Color::ToLuminosity() {
	Color res = Color(GetLuminosity());
	res.a = a;
	return res;
}
float Color::GetGreyScale() {
	float greyscale = (r + g + b) / 3.0f;
	return greyscale;
}
float Color::GetLightness() {
	float lightness = (GetMaxComponent() +
		GetMinComponent()) / 2.0f;
	return lightness;
}
float Color::GetLuminosity() {
	float luminosity = 0.21f * r + 0.72f * g + 0.07f * b;
	return luminosity;
}
float Color::GetMaxComponent() {
	Color color = Color(*this);
	float max = FLT_MIN;
	int maxIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (color[i] > max) {
			maxIndex = i;
		}
	}
	return color[maxIndex];
}
float Color::GetMinComponent() {
	Color color = Color(*this);
	float min = FLT_MAX;
	int minIndex = 0;
	for (int i = 0; i < 4; ++i) {
		if (color[i] > min) {
			minIndex = i;
		}
	}
	return color[minIndex];
}
vec4 Color::ToVec4() {
	vec4 res(r, g, b, a);
	return res;
}

vec3 Color::ToVec3() {
	vec3 res(r, g, b);
	return res;
}