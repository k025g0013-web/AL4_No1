#include "GameOverScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

void GameOverScene::Initialize() {
	isFinished_ = false; 
}

void GameOverScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinished_ = true;
	}
}

void GameOverScene::Draw() {}

bool GameOverScene::IsFinished() const { return isFinished_; }