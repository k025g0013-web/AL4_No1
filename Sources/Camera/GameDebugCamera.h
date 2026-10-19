#pragma once

#include "KamataEngine.h"

class GameDebugCamera {
public:
    void Initialize();
    void Update();

    const KamataEngine::Camera &GetCamera();

private:
    KamataEngine::DebugCamera debugCamera_{ 1280, 720 };
};
