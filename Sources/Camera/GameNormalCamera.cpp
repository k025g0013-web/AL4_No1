
#include "GameNormalCamera.h"

#include "Object/Player/Player.h"
#include "Math/Vector.h"

#include <algorithm>

using namespace KamataEngine;

void GameNormalCamera::Initialize() {
    camera_.farZ = 2000.0f;
    camera_.Initialize();
}

void GameNormalCamera::Update() {
    if (target_ == nullptr) {
        return;
    }

    if (mode_ == Mode::kFollow) {
        // 追従対象の座標と速度
        const WorldTransform &targetWorldTransform =
            target_->GetWorldTransform();

        const Vector3 &targetVelocity =
            target_->GetVelocity();

        // 目標座標を計算
        targetPosition_ = AddVector(
            targetWorldTransform.translation_,
            AddVector(
                targetOffset_,
                Multiply(velocityBias_, targetVelocity)
            )
        );

        // なめらかに追従
        camera_.translation_ = Lerp(
            camera_.translation_,
            targetPosition_,
            interpolationRate_
        );

        // プレイヤーを画面内に収める
        camera_.translation_.x = std::clamp(
            camera_.translation_.x,
            targetWorldTransform.translation_.x + margin_.left,
            targetWorldTransform.translation_.x + margin_.right
        );

        camera_.translation_.y = std::clamp(
            camera_.translation_.y,
            targetWorldTransform.translation_.y + margin_.bottom,
            targetWorldTransform.translation_.y + margin_.top
        );

    } else if (mode_ == Mode::kForcedScroll) {
        // 強制スクロール
        camera_.translation_ = AddVector(
            camera_.translation_,
            scrollSpeed_
        );
    }

    // カメラの移動範囲を制限
    camera_.translation_.x = std::clamp(
        camera_.translation_.x,
        movableArea_.left,
        movableArea_.right
    );

    camera_.translation_.y = std::clamp(
        camera_.translation_.y,
        movableArea_.bottom,
        movableArea_.top
    );

    camera_.UpdateMatrix();
}

void GameNormalCamera::Reset() {
    if (target_ == nullptr) {
        return;
    }

    const WorldTransform &targetWorldTransform =
        target_->GetWorldTransform();

    camera_.translation_ = AddVector(
        targetWorldTransform.translation_,
        targetOffset_
    );

    targetPosition_ = camera_.translation_;

    camera_.UpdateMatrix();
}

Vector3 GameNormalCamera::Lerp(
    const Vector3 &start,
    const Vector3 &end,
    float t
) {
    return Vector3{
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t,
        start.z + (end.z - start.z) * t
    };
}
