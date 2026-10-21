#pragma once

#include "Scene.h"

class GameOverScene final : public Scene {
public:
	GameOverScene() = default;
	~GameOverScene() override = default;

	void Initialize();
	void Update() override;
	void Draw() override;

	bool IsFinished() const override;

private:
	bool isFinished_ = false;
};