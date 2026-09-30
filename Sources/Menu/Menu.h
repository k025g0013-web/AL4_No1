#pragma once
#include "KamataEngine.h"

class Menu {
public:
	virtual ~Menu() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
};