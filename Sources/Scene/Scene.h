#pragma once
#include "KamataEngine.h"

class Scene {
public:
	virtual ~Scene() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual bool IsFinished() const = 0;
};