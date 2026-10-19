
#pragma once

#include "KamataEngine.h"

class Player;

// カメラの移動可能範囲
struct CameraRect {
    float left = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float top = 0.0f;
};

class GameNormalCamera {
public:
    void Initialize();
    void Update();
    void Reset();

    void SetTarget(Player *target) {
        target_ = target;
    }

    void SetMovableArea(const CameraRect &area) {
        movableArea_ = area;
    }

    KamataEngine::Vector3 GetTranslation() const {
        return camera_.translation_;
    }

private:
    // 補間
    KamataEngine::Vector3 Lerp(
        const KamataEngine::Vector3 &start,
        const KamataEngine::Vector3 &end,
        float t
    );

private:
    KamataEngine::Camera camera_{};
    Player *target_ = nullptr;

    // カメラの動作モード
    enum class Mode {
        kFollow,
        kForcedScroll
    };
    Mode mode_ = Mode::kFollow;

    // 強制スクロール速度
    KamataEngine::Vector3 scrollSpeed_{ 0.05f, 0.0f, 0.0f };

    // プレイヤーとの距離
    KamataEngine::Vector3 targetOffset_{ 0.0f, 0.0f, -30.0f };

    // カメラの移動範囲
    CameraRect movableArea_{ 0.0f, 100.0f, 0.0f, 100.0f };

    // 目標座標
    KamataEngine::Vector3 targetPosition_{};

    // 追従設定
    float interpolationRate_ = 0.1f;
    float velocityBias_ = 15.5f;

    // プレイヤーを画面内に収める範囲
    CameraRect margin_{ -25.0f, 25.0f, -25.0f, 25.0f };
};
