/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Scene.h
@date: 21/07/2015
@author: Emmanuel Vaccaro
@brief: A Scene contains all of the objects 
in your game.
===============================================*/

#ifndef _SCENE_H_
#define _SCENE_H_

#include "Game.h"

class Scene : public Game {
public:
	Scene();
	virtual ~Scene();
	void Init(CoreEngine* _engine); 
	bool Update();
	void Draw(RenderingEngine* _renderer);
};

#endif // _SCENE_H_