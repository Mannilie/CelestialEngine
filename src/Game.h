/*=============================================
-----------------------------------
Copyright (c) 2015 Emmanuel Vaccaro
-----------------------------------
@file: Game.h
@date: 21/07/2015
@author: Emmanuel Vaccaro
@brief: A base class that starts a game project
with a new scene.
===============================================*/

#ifndef _GAME_H_
#define _GAME_H_

// Sub-engines
#include "CoreEngine.h"
#include "RenderingEngine.h"

// Objects
#include "GameObject.h"

// Other
#include <vector>
using std::vector;

class Game {
public:
	Game(){}
	virtual ~Game(){}
	virtual void Init(CoreEngine* _engine) = 0;
	virtual bool Update();
	void UpdateHierarchy();
	void UpdateChildren(Transform* _transform);
	void UpdateGUIElements();
	virtual void Draw(RenderingEngine* _renderer) = 0;
	void AddToScene(GameObject* _gameObject);

	RenderingEngine* renderer;
	vector<GameObject*> objects;
};

#endif // _GAME_H_