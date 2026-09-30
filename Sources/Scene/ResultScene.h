#pragma once

#include "Scene.h"

class ResultScene : public Scene {
public:
	ResultScene() = default;
	~ResultScene() override = default;

	void Initialize();
	void Update() override;
	void Draw() override;

	bool IsFinished() const override;

private:
	bool isFinished_ = false;
};