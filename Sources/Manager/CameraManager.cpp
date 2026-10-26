#include "CameraManager.h"

#include <algorithm>
#include <cstdlib>

using namespace KamataEngine;

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

    // カメラシェイクの更新
    // プロジェクトのフレーム経過時間を使用
    const float deltaTime = 1.0f / 60.0f;
    UpdateShake(deltaTime);

    auto &camera = camera_.GetCamera();

    if (isDebugCameraActive_) {
        const auto &debug = debugCamera_.GetCamera();

        camera.matView = debug.matView;
        camera.matProjection = debug.matProjection;
        camera.TransferMatrix();
    } else {
        // 通常のカメラ位置にシェイクを適用
        camera.translation_ = normalCamera_.GetTranslation();
        camera.translation_.x += shakeOffset_.x;
        camera.translation_.y += shakeOffset_.y;

        camera_.UpdateMatrix();
    }
}

void CameraManager::Reset() {
    normalCamera_.Reset();

    shakeTimer_ = 0.0f;
    shakeOffset_ = {};
}

void CameraManager::StartShake(float duration, float intensity) {
    shakeDuration_ = duration;
    shakeIntensity_ = intensity;
    shakeTimer_ = duration;
    shakeOffset_ = {};
}

void CameraManager::UpdateShake(float deltaTime) {
    if (shakeTimer_ <= 0.0f || shakeDuration_ <= 0.0f) {
        shakeTimer_ = 0.0f;
        shakeOffset_ = {};
        return;
    }

    shakeTimer_ = (std::max)(0.0f, shakeTimer_ - deltaTime);

    // 時間経過とともに揺れを弱める
    const float ratio = shakeTimer_ / shakeDuration_;
    const float power = shakeIntensity_ * ratio;

    auto RandomRange = [](float min, float max) {
        const float t = static_cast<float>(rand()) / RAND_MAX;
        return min + (max - min) * t;
        };

    shakeOffset_.x = RandomRange(-power, power);
    shakeOffset_.y = RandomRange(-power, power);
    shakeOffset_.z = 0.0f;

    if (shakeTimer_ <= 0.0f) {
        shakeOffset_ = {};
    }
}