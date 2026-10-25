
#include "PoseMenu.h"

using namespace KamataEngine;

#include "Math/Easing.h"

#include <cmath>

void PoseMenu::Initialize() {
    // 状態を初期化
    selectedItem_ = 1;
    isDecided_ = false;

    moveTimer_ = 0.0f;
    sineTimer_ = 0.0f;

    // テクスチャ読み込み
    graphHandleResetText_ = TextureManager::Load("resetText.png");
    graphHandleResumeText_ = TextureManager::Load("resumeText.png");
    graphHandleBackTitleText_ = TextureManager::Load("backTitleText.png");

    // 初期座標
    startPositionReset_ = { 150.0f, 300.0f };
    startPositionResume_ = { 180.0f, 400.0f };
    startPositionBackTitle_ = { 150.0f, 500.0f };

    targetPositionReset_ = startPositionReset_;
    targetPositionResume_ = startPositionResume_;
    targetPositionBackTitle_ = startPositionBackTitle_;

    basePositionReset_ = startPositionReset_;
    basePositionResume_ = startPositionResume_;
    basePositionBackTitle_ = startPositionBackTitle_;

    // スプライト生成
    spriteResetText_ = Sprite::Create(
        graphHandleResetText_, startPositionReset_);

    spriteResumeText_ = Sprite::Create(
        graphHandleResumeText_, startPositionResume_);

    spriteBackTitleText_ = Sprite::Create(
        graphHandleBackTitleText_, startPositionBackTitle_);

    // 初期色設定
    spriteResetText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
    spriteResumeText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
    spriteBackTitleText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
}

void PoseMenu::Update() {
    auto *input = Input::GetInstance();

    int previousItem = selectedItem_;

    // 決定
    if (input->TriggerKey(DIK_SPACE)) {
        isDecided_ = true;
    }

    // 上
    if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_W)) {
        selectedItem_--;
        if (selectedItem_ < 0) {
            selectedItem_ = 2;
        }
    }

    // 下
    if (input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_S)) {
        selectedItem_++;
        if (selectedItem_ > 2) {
            selectedItem_ = 0;
        }
    }

    // 選択項目が変わった
    if (previousItem != selectedItem_) {
        startPositionReset_ = basePositionReset_;
        startPositionResume_ = basePositionResume_;
        startPositionBackTitle_ = basePositionBackTitle_;

        moveTimer_ = 0.0f;

        if (selectedItem_ == 0) {
            // リセットを選択
            targetPositionReset_ = { 180.0f, 300.0f };
            targetPositionResume_ = { 150.0f, 400.0f };
            targetPositionBackTitle_ = { 150.0f, 500.0f };

            spriteResetText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            spriteResumeText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
            spriteBackTitleText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
        } else if (selectedItem_ == 1) {
            // 再開を選択
            targetPositionReset_ = { 150.0f, 300.0f };
            targetPositionResume_ = { 180.0f, 400.0f };
            targetPositionBackTitle_ = { 150.0f, 500.0f };

            spriteResetText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
            spriteResumeText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
            spriteBackTitleText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
        } else {
            // タイトルへ戻るを選択
            targetPositionReset_ = { 150.0f, 300.0f };
            targetPositionResume_ = { 150.0f, 400.0f };
            targetPositionBackTitle_ = { 180.0f, 500.0f };

            spriteResetText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
            spriteResumeText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
            spriteBackTitleText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        }
    }

    // イージング
    if (moveTimer_ < moveDuration_) {
        moveTimer_ += 1.0f / 60.0f;

        float t = moveTimer_ / moveDuration_;
        if (t > 1.0f) {
            t = 1.0f;
        }

        basePositionReset_ = Easing::Interpolate(
            EasingType::easeOutQuad,
            t,
            startPositionReset_,
            targetPositionReset_);

        basePositionResume_ = Easing::Interpolate(
            EasingType::easeOutQuad,
            t,
            startPositionResume_,
            targetPositionResume_);

        basePositionBackTitle_ = Easing::Interpolate(
            EasingType::easeOutQuad,
            t,
            startPositionBackTitle_,
            targetPositionBackTitle_);
    }

    // Sine波による上下の揺れ
    sineTimer_ += 1.0f / 60.0f;

    float offsetReset =
        std::sin(sineTimer_ * sineSpeed_) * sineAmplitude_;

    float offsetResume =
        std::sin(sineTimer_ * sineSpeed_ + 1.0f) * sineAmplitude_;

    float offsetTitle =
        std::sin(sineTimer_ * sineSpeed_ + 2.0f) * sineAmplitude_;

    // スプライト位置に反映
    spriteResetText_->SetPosition({
        basePositionReset_.x,
        basePositionReset_.y + offsetReset
        });

    spriteResumeText_->SetPosition({
        basePositionResume_.x,
        basePositionResume_.y + offsetResume
        });

    spriteBackTitleText_->SetPosition({
        basePositionBackTitle_.x,
        basePositionBackTitle_.y + offsetTitle
        });
}

void PoseMenu::Draw() {
    spriteResetText_->Draw();
    spriteResumeText_->Draw();
    spriteBackTitleText_->Draw();
}
