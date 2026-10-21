
#pragma once

#include "KamataEngine.h"

class Goal {
public:
    ~Goal();

    void Initialize(
        KamataEngine::Model *model,
        KamataEngine::Camera *camera,
        const KamataEngine::Vector3 &position
    );

    void Update();
    void Draw();

    // プレイヤーとの接触判定
    bool CheckCollision(
        const KamataEngine::Vector3 &playerPosition,
        float playerWidth,
        float playerHeight
    ) const;

    const KamataEngine::WorldTransform &GetWorldTransform() const {
        return worldTransform_;
    }

private:
    // ゴールのワールド変換
    KamataEngine::WorldTransform worldTransform_;

    // ゴールのモデル
    KamataEngine::Model *model_ = nullptr;

    // カメラ
    KamataEngine::Camera *camera_ = nullptr;

    // 当たり判定サイズ
    float width_ = 2.0f;
    float height_ = 2.0f;
};
