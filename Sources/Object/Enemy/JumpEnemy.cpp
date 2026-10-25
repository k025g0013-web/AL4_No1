#include "JumpEnemy.h"

using namespace KamataEngine;

void JumpEnemy::Initialize(
    Model *model,
    Camera *camera,
    const Vector3 &position
) {
    // 基本的な初期化は親クラスに任せる
    Enemy::Initialize(model, camera, position);

    jumpTimer_ = 0.0f;
}

void JumpEnemy::Update() {
    // 地面にいる間だけジャンプまでの時間を計測する
    if (behavior_ == Behavior::kWalk && onGround_) {
        jumpTimer_ += 1.0f / 60.0f;

        // 一定時間経過したらジャンプ
        if (jumpTimer_ >= kJumpInterval) {
            velocity_.y = kJumpPower;
            onGround_ = false;
            jumpTimer_ = 0.0f;
        }
    }

    // 歩行・死亡・旋回などの既存処理を利用する
    Enemy::Update();
}

void JumpEnemy::Move(bool applyGravity) {
    (void)applyGravity;
    // 親クラスのマップ衝突判定と着地処理を使い、
    // 重力を有効にして移動する
    Enemy::Move(true);
}