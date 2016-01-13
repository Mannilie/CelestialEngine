/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Debug.h
@date: 13/06/2015
@author: Emmanuel Vaccaro
@brief: A way to debug in the engine
===============================================*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

// Other
#include <vector>
using std::vector;
#include <string>
using std::string;

struct TextLog {
	string text;
	int callCount;
	TextLog(string _text, int _callCount = 0) : 
	text(_text), callCount(_callCount) {}
};

class Object;
class Debug {
public:
	static Debug* instance;

	Debug();
	~Debug();
	static void Create();
	static bool Update();
	static void Log(string _text);
	static void Log(string _text, Object* _object);
	static void LogError(string _error);
	static void LogError(string _error, Object* _object);
	static void LogWarning(string _warning);
	static void LogWarning(string _warning, Object* _object);
	static bool HasError();
	static bool TextExists(string _text, vector<TextLog>& _textLog);

	vector<TextLog> text;
	vector<TextLog> errors;
	vector<TextLog> warnings;
};

#endif // _DEBUG_H_