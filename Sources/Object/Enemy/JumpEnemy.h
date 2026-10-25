#pragma once

#include "Object/Enemy/Enemy.h"

class JumpEnemy : public Enemy {
public:
    void Initialize(
        KamataEngine::Model *model,
        KamataEngine::Camera *camera,
        const KamataEngine::Vector3 &position
    ) override;

    void Update() override;

    // 通常の敵と同じ移動処理に重力を適用する
    void Move(bool applyGravity) override;

private:
    // ジャンプまでの待ち時間
    float jumpTimer_ = 0.0f;

    // ジャンプの間隔（秒）
    static constexpr float kJumpInterval = 0.3f;

    // ジャンプの初速
    static constexpr float kJumpPower = 0.35f;
};