/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Color.h
@date: 02/07/2015
@author: Emmanuel Vaccaro
@brief: Defined color container
===============================================*/

#ifndef _COLOR_H_
#define _COLOR_H_

// Utilities
#include "GLM_Header.h"

class Color {
public:
	float r, g, b, a;
	static float gammaCurve;
	static Color black;
	static Color blue;
	static Color clear;
	static Color cyan;
	static Color gray;
	static Color green;
	static Color magenta;
	static Color red;
	static Color white;
	static Color yellow;
	static Color Lerp(const Color& _from, const Color& _to, float _t);
	static float Max(Color& _color);
	static float Max(float _r, float _g, float _b, float _a);
	static float Min(Color& _color);
	static float Min(float _r, float _g, float _b, float _a);
	Color();
	Color(float _c);
	Color(float _r, float _g, float _b, float _a);
	~Color();
	float operator[](int _colorIndex);
	Color operator=(Color& _rhs);
	Color operator=(vec4& _rhs);
	Color operator-(Color& _rhs);
	Color operator*(Color& _rhs);
	Color operator/(Color& _rhs);
	Color operator+(Color& _rhs);
	Color operator-=(Color& _rhs);
	Color operator*=(Color& _rhs);
	Color operator/=(Color& _rhs);
	Color operator+=(Color& _rhs);
	Color ToGamma(float _gammaCurve = Color::gammaCurve);
	Color ToLinear(float _gammaCurve = Color::gammaCurve);
	Color ToGreyScale();
	Color ToLightness();
	Color ToLuminosity();
	vec4 ToVec4();
	vec3 ToVec3();
	float GetGreyScale();
	float GetLightness();
	float GetLuminosity();
	float GetMaxComponent();
	float GetMinComponent();

	float greyscale;
	float maxColorComponent;
};

#endif // _COLOR_H_