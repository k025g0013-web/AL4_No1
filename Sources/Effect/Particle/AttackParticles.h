#pragma once
#include "IParticle.h"

#include <array>

class AttackParticles : public IParticle {
public:
    ~AttackParticles() override = default;

    void Initialize(
        KamataEngine::Model *model,
        KamataEngine::Camera *camera
    ) override;

    void Emit(
        const KamataEngine::Vector3 &position
    ) override;

    void Update() override;

private:
    // 粒子ごとの速度
    std::array<KamataEngine::Vector3, kNumParticles> velocities_{};

    // 演出時間
    static constexpr float kDuration = 0.25f;

    // 粒子の初期サイズ
    static constexpr float kInitialScale = 0.12f;

    // 終了時のサイズ倍率
    static constexpr float kMinScaleRate = 0.1f;

    // 重力
    static constexpr float kGravity = -0.004f;

    // 粒子の発射角度の間隔
    static constexpr float kAngleUnit =
        6.28318530718f / static_cast<float>(kNumParticles);
};