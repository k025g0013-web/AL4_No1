
#include "CameraManager.h"

void CameraManager::Initialize(Player *player) {
    // 描画用カメラ
    camera_.Initialize();

    // 通常カメラ
    normalCamera_.Initialize();
    normalCamera_.SetTarget(player);
    normalCamera_.SetMovableArea(
        CameraRect{ 20.35f, 177.6f, 11.0f, 100.0f }
    );
    normalCamera_.Reset();

    // デバッグカメラ
    debugCamera_.Initialize();
}

void CameraManager::Update() {
    // 通常カメラの更新
    normalCamera_.Update();

    // デバッグカメラの更新
    debugCamera_.Update();

    auto &camera = camera_.GetCamera();

    if (isDebugCameraActive_) {
        const auto &debug = debugCamera_.GetCamera();

        camera.matView = debug.matView;
        camera.matProjection = debug.matProjection;
        camera.TransferMatrix();
    } else {
        camera.translation_ = normalCamera_.GetTranslation();
        camera_.UpdateMatrix();
    }
}

void CameraManager::Reset() {
    normalCamera_.Reset();
}