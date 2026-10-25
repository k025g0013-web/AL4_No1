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
    bool finished_ = false;
};