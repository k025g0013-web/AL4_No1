#include "GameOverMenu.h"

#include "KamataEngine.h"
#include "Math/Easing.h"

#include <cmath>

using namespace KamataEngine;

void GameOverMenu::Initialize() {
    selectedItem_ = 0;
    isDecided_ = false;
    moveTimer_ = 0.0f;
    sineTimer_ = 0.0f;

    // テクスチャ読み込み
    graphHandleContinueText_ =
        TextureManager::Load("continueText.png");

    graphHandleBackTitleText_ =
        TextureManager::Load("backTitleText.png");

    // 初期座標
    startPositionContinue_ = { 430.0f, 400.0f };
    startPositionBackTitle_ = { 400.0f, 500.0f };

    targetPositionContinue_ = startPositionContinue_;
    targetPositionBackTitle_ = startPositionBackTitle_;

    basePositionContinue_ = startPositionContinue_;
    basePositionBackTitle_ = startPositionBackTitle_;

    // スプライト生成
    spriteContinueText_ = Sprite::Create(
        graphHandleContinueText_,
        startPositionContinue_
    );

    spriteBackTitleText_ = Sprite::Create(
        graphHandleBackTitleText_,
        startPositionBackTitle_
    );

    // 選択中は白、非選択は暗め
    spriteContinueText_->SetColor(
        { 1.0f, 1.0f, 1.0f, 1.0f }
    );

    spriteBackTitleText_->SetColor(
        { 0.5f, 0.5f, 0.5f, 1.0f }
    );
}

void GameOverMenu::Update() {
    auto *input = Input::GetInstance();

    // 決定済みなら入力を受け付けない
    if (isDecided_) {
        return;
    }

    int previousItem = selectedItem_;

    // 決定
    if (input->TriggerKey(DIK_SPACE)) {
        isDecided_ = true;
        return;
    }

    // 上
    if (input->TriggerKey(DIK_UP) ||
        input->TriggerKey(DIK_W)) {
        selectedItem_--;

        if (selectedItem_ < 0) {
            selectedItem_ = 1;
        }
    }

    // 下
    if (input->TriggerKey(DIK_DOWN) ||
        input->TriggerKey(DIK_S)) {
        selectedItem_++;

        if (selectedItem_ > 1) {
            selectedItem_ = 0;
        }
    }

    // 選択項目が変わった
    if (previousItem != selectedItem_) {
        startPositionContinue_ = basePositionContinue_;
        startPositionBackTitle_ = basePositionBackTitle_;

        moveTimer_ = 0.0f;

        if (selectedItem_ == 0) {
            targetPositionContinue_ = { 430.0f, 400.0f };
            targetPositionBackTitle_ = { 400.0f, 500.0f };

            spriteContinueText_->SetColor(
                { 1.0f, 1.0f, 1.0f, 1.0f }
            );

            spriteBackTitleText_->SetColor(
                { 0.5f, 0.5f, 0.5f, 1.0f }
            );
        } else {
            targetPositionContinue_ = { 400.0f, 400.0f };
            targetPositionBackTitle_ = { 430.0f, 500.0f };

            spriteContinueText_->SetColor(
                { 0.5f, 0.5f, 0.5f, 1.0f }
            );

            spriteBackTitleText_->SetColor(
                { 1.0f, 1.0f, 1.0f, 1.0f }
            );
        }
    }

    // イージング
    if (moveTimer_ < moveDuration_) {
        moveTimer_ += 1.0f / 60.0f;

        float t = moveTimer_ / moveDuration_;

        if (t > 1.0f) {
            t = 1.0f;
        }

        basePositionContinue_ = Easing::Interpolate(
            EasingType::easeOutQuad,
            t,
            startPositionContinue_,
            targetPositionContinue_
        );

        basePositionBackTitle_ = Easing::Interpolate(
            EasingType::easeOutQuad,
            t,
            startPositionBackTitle_,
            targetPositionBackTitle_
        );
    }

    // 上下の揺れ
    sineTimer_ += 1.0f / 60.0f;

    float offsetContinue =
        std::sin(sineTimer_ * sineSpeed_) * sineAmplitude_;

    float offsetBackTitle =
        std::sin(sineTimer_ * sineSpeed_ + 1.0f)
        * sineAmplitude_;

    spriteContinueText_->SetPosition({
        basePositionContinue_.x,
        basePositionContinue_.y + offsetContinue
        });

    spriteBackTitleText_->SetPosition({
        basePositionBackTitle_.x,
        basePositionBackTitle_.y + offsetBackTitle
        });
}

void GameOverMenu::Draw() {
    spriteContinueText_->Draw();
    spriteBackTitleText_->Draw();
}
