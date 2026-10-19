#pragma once

#include "KamataEngine.h"

class GameCamera {
public:
    void Initialize();
    void UpdateMatrix();

    KamataEngine::Camera &GetCamera() {
        return camera_;
    }

    const KamataEngine::Camera &GetCamera() const {
        return camera_;
    }

private:
    KamataEngine::Camera camera_;
};