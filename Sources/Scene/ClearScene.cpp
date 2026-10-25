#include "ClearScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

ClearScene::ClearScene() {}

ClearScene::~ClearScene() {}

void ClearScene::Initialize() {
    finished_ = false;
}

void ClearScene::Update() {
    if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
        finished_ = true;
    }
}

void ClearScene::Draw() {
}