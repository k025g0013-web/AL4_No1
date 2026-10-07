#include "ResultScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

void ResultScene::Initialize() {
	isFinished_ = false; 
}

void ResultScene::Update() { 
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinished_ = true;
	}
}

void ResultScene::Draw() {}

bool ResultScene::IsFinished() const { return isFinished_; }