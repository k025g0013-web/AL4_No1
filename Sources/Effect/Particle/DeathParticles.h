
#pragma once
#include "IParticle.h"

#include <array>

class DeathParticles : public IParticle {
public:
    ~DeathParticles() override = default;

    void Initialize(
        KamataEngine::Model *model,
        KamataEngine::Camera *camera
    ) override;

    void Emit(
        const KamataEngine::Vector3 &position
    ) override;

    void Update() override;

private:
    // 破片ごとの速度
    std::array<KamataEngine::Vector3, kNumParticles> velocities_{};

    // 重力加速度（下方向）
    static inline const float kGravity = -0.004f;

    // 初速
    static inline const float kMinSpeed = 0.10f;
    static inline const float kMaxSpeed = 0.20f;

    // 初期サイズ
    static inline const float kInitialScale = 1.0f;

    // 最終的なサイズの割合
    static inline const float kMinScaleRate = 0.05f;
};
