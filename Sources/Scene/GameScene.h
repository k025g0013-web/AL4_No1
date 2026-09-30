#pragma once

#include "Scene.h"

class GameScene : public Scene {
public:
	GameScene() = default;
	~GameScene() override = default;

	void Initialize();
	void Update() override;
	void Draw() override;

	bool IsFinished() const override;

private:
	bool isFinished_ = false;
};