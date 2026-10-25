#include "TitleMenu.h"
using namespace KamataEngine;

#include "Math/Easing.h"

#include <cmath>

void TitleMenu::Initialize() {
    // テクスチャ読み込み
    graphHandleStartText_ = TextureManager::Load("startText.png");
    graphHandleExitText_ = TextureManager::Load("exitText.png");

    // 初期座標
    startPositionStart_ = { 180.0f, 400.0f };
    startPositionExit_ = { 150.0f, 500.0f };

    targetPositionStart_ = startPositionStart_;
    targetPositionExit_ = startPositionExit_;

	basePositionStart_ = startPositionStart_;
	basePositionExit_ = startPositionExit_;

    // スプライト生成
    spriteStartText_ = Sprite::Create(graphHandleStartText_, startPositionStart_);
    spriteExitText_ = Sprite::Create(graphHandleExitText_, startPositionExit_);

	spriteStartText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	spriteExitText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
}

void TitleMenu::Update() {
	auto *input = Input::GetInstance();

	// 選択変更前の項目を保存
	int previousItem = selectedItem_;

	// 決定
	if (input->TriggerKey(DIK_SPACE)) isDecided_ = true;

	// 上
	if (input->TriggerKey(DIK_UP) || input->TriggerKey(DIK_W)) {
		selectedItem_--;
		if (selectedItem_ < 0) selectedItem_ = 1;
	}

	// 下
	if (input->TriggerKey(DIK_DOWN) || input->TriggerKey(DIK_S)) {
		selectedItem_++;
		if (selectedItem_ > 1) selectedItem_ = 0;
	}


	// 選択項目が変わった
	if (previousItem != selectedItem_) {
		// 揺れを適用する前の座標を移動開始位置にする
		startPositionStart_ = basePositionStart_;
		startPositionExit_ = basePositionExit_;

		moveTimer_ = 0.0f;

		if (selectedItem_ == 0) {
			targetPositionStart_ = { 180.0f, 400.0f };
			targetPositionExit_ = { 150.0f, 500.0f };

			spriteStartText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			spriteExitText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		} else {
			targetPositionStart_ = { 150.0f, 400.0f };
			targetPositionExit_ = { 180.0f, 500.0f };

			spriteStartText_->SetColor({ 0.5f, 0.5f, 0.5f, 1.0f });
			spriteExitText_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}
	}

	// イージング
	if (moveTimer_ < moveDuration_) {
		moveTimer_ += 1.0f / 60.0f;

		float t = moveTimer_ / moveDuration_;
		if (t > 1.0f) {
			t = 1.0f;
		}

		basePositionStart_ = Easing::Interpolate(
			EasingType::easeOutQuad,
			t,
			startPositionStart_,
			targetPositionStart_
		);

		basePositionExit_ = Easing::Interpolate(
			EasingType::easeOutQuad,
			t,
			startPositionExit_,
			targetPositionExit_
		);
	}

	// Sine波による上下の揺れ
	sineTimer_ += 1.0f / 60.0f;

	float sineOffsetStart =
		std::sin(sineTimer_ * sineSpeed_) * sineAmplitude_;

	float sineOffsetExit =
		std::sin(sineTimer_ * sineSpeed_ + 1.0f) * sineAmplitude_;

	// 基準座標に揺れを加えてスプライトに反映
	spriteStartText_->SetPosition({
		basePositionStart_.x,
		basePositionStart_.y + sineOffsetStart
		});

	spriteExitText_->SetPosition({
		basePositionExit_.x,
		basePositionExit_.y + sineOffsetExit
		});
}

void TitleMenu::Draw() {
	spriteStartText_->Draw();
	spriteExitText_->Draw();
}