#include "GameDebugCamera.h"

void GameDebugCamera::Initialize() {
    // DebugCameraのコンストラクタで初期化される仕様なら、
    // 追加処理は不要
}

void GameDebugCamera::Update() {
    debugCamera_.Update();
}

const KamataEngine::Camera &GameDebugCamera::GetCamera() {
    return debugCamera_.GetCamera();
}