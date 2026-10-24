
#include "DeathParticles.h"

#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Math/UpdateWorldTransform.h"

#include <algorithm>
#include <cstdlib>

using namespace KamataEngine;

void DeathParticles::Initialize(Model *model, Camera *camera) {
    IParticle::Initialize(model, camera);
}

void DeathParticles::Emit(const Vector3 &position) {
    counter_ = 0.0f;
    finished_ = false;

    color_ = { 1.0f, 1.0f, 1.0f, 1.0f };
    objectColor_.SetColor(color_);

    for (uint32_t i = 0; i < kNumParticles; ++i) {
        auto &transform = worldTransform_[i];

        // 発生位置を設定
        transform.translation_ = position;

        // 初期サイズ
        transform.scale_ = {
            kInitialScale,
            kInitialScale,
            kInitialScale
        };

        const float baseAngle = kAngleUnit * static_cast<float>(i);
        const float randomAngle = (static_cast<float>(std::rand()) / RAND_MAX - 0.5f) * (kAngleUnit * 0.4f);
        const float angle = baseAngle + randomAngle;

        // 初速
        const float randomRate = static_cast<float>(std::rand()) / RAND_MAX;
        const float speed = 0.12f + randomRate * 0.08f;

        // 全方向へ飛び散る初速を設定
        velocities_[i] = {
            std::cos(angle) * speed,
            std::sin(angle) * speed + 0.08f,
            0.0f
        };

        UpdateWorldTransform(transform);
    }
}

void DeathParticles::Update() {
    if (finished_) {
        return;
    }

    // 60 FPSを基準にした1フレーム分の時間
    constexpr float deltaTime = 1.0f / 60.0f;

    // 経過時間を更新
    counter_ += deltaTime;
    counter_ = (std::min)(counter_, kDuration);

    const float progress = counter_ / kDuration;

    for (uint32_t i = 0; i < kNumParticles; ++i) {
        auto &transform = worldTransform_[i];
        auto &velocity = velocities_[i];

        // 重力で下向きに加速
        velocity.y += kGravity;

        // 速度に応じて移動
        transform.translation_.x += velocity.x;
        transform.translation_.y += velocity.y;
        transform.translation_.z += velocity.z;

        // 徐々に縮小
        const float scale =
            kInitialScale *
            (1.0f - progress * (1.0f - kMinScaleRate));

        transform.scale_ = { scale, scale, scale };

        UpdateWorldTransform(transform);
    }

    // 徐々に透明化
    color_.w = std::clamp(1.0f - progress, 0.0f, 1.0f);
    objectColor_.SetColor(color_);

    if (counter_ >= kDuration) {
        finished_ = true;
    }
}
