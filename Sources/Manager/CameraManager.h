#pragma once

#include "Camera/GameCamera.h"
#include "Camera/GameNormalCamera.h"
#include "Camera/GameDebugCamera.h"

class Player;

class CameraManager {
public:
    void Initialize(Player *player);
    void Update();
    void Reset();

    void SetMovableArea(const CameraRect &area) {
        normalCamera_.SetMovableArea(area);
    }

    void SetDebugCameraActive(bool active) {
        isDebugCameraActive_ = active;
    }

    bool IsDebugCameraActive() const {
        return isDebugCameraActive_;
    }

    KamataEngine::Camera &GetCamera() {
        return camera_.GetCamera();
    }

    const KamataEngine::Camera &GetCamera() const {
        return camera_.GetCamera();
    }

    void SetTarget(Player *player) {
        normalCamera_.SetTarget(player);
    }

private:
    GameCamera camera_;
    GameNormalCamera normalCamera_;
    GameDebugCamera debugCamera_;

    bool isDebugCameraActive_ = false;
};
