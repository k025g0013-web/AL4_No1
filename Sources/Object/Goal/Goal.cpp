
#include "Goal.h"

#include "Math/UpdateWorldTransform.h"

using namespace KamataEngine;

#include <cassert>

Goal::~Goal() {
    delete model_;
};

void Goal::Initialize(
    Model *model,
    Camera *camera,
    const Vector3 &position
) {
    assert(model);

    model_ = model;


    worldTransform_.Initialize();
    worldTransform_.translation_ = position;
    
    camera_ = camera;
}

void Goal::Update() {
    UpdateWorldTransform(worldTransform_);
}

void Goal::Draw() {
    if (model_ && camera_) {
        model_->Draw(worldTransform_, *camera_);
    }
}

bool Goal::CheckCollision(
    const Vector3 &playerPosition,
    float playerWidth,
    float playerHeight
) const {
    // プレイヤーのAABB
    const float playerLeft =
        playerPosition.x - playerWidth * 0.5f;
    const float playerRight =
        playerPosition.x + playerWidth * 0.5f;
    const float playerBottom =
        playerPosition.y - playerHeight * 0.5f;
    const float playerTop =
        playerPosition.y + playerHeight * 0.5f;

    // ゴールのAABB
    const float goalLeft =
        worldTransform_.translation_.x - width_ * 0.5f;
    const float goalRight =
        worldTransform_.translation_.x + width_ * 0.5f;
    const float goalBottom =
        worldTransform_.translation_.y - height_ * 0.5f;
    const float goalTop =
        worldTransform_.translation_.y + height_ * 0.5f;

    // X・Y両方向で重なっていれば接触
    return playerLeft <= goalRight &&
        playerRight >= goalLeft &&
        playerBottom <= goalTop &&
        playerTop >= goalBottom;
}
