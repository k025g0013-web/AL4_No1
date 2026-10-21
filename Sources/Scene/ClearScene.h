#pragma once

#include "Scene.h"
#include "Effect/Fade.h"

class ClearScene final : public Scene {
public:
    ClearScene();
    ~ClearScene() override;

    void Initialize();
    void Update() override;
    void Draw() override;

    bool IsFinished() const override {
        return finished_;
    }

private:
    Fade *fade_ = nullptr;

    float duration_ = 1.0f;
    bool finished_ = false;
    bool isFadeOutStarted_ = false;
};