#include "TitleMenu.h"
using namespace KamataEngine;

#include "Math/Easing.h"

void TitleMenu::Initialize() {
    // テクスチャ読み込み
    graphHandleStartText_ = TextureManager::Load("startText.png");
    graphHandleExitText_ = TextureManager::Load("exitText.png");

    // 初期座標
    startPositionStart_ = { 130.0f, 200.0f };
    startPositionExit_ = { 100.0f, 300.0f };

    targetPositionStart_ = startPositionStart_;
    targetPositionExit_ = startPositionExit_;

    // スプライト生成
    spriteStartText_ = Sprite::Create(graphHandleStartText_, startPositionStart_);
    spriteExitText_ = Sprite::Create(graphHandleExitText_, startPositionExit_);
}

void TitleMenu::Update() {
	auto *input = Input::GetInstance();

	// 選択変更前の項目を保存
	int previousItem = selectedItem_;

	// 決定
	if (input->TriggerKey(DIK_SPACE)) isDecided_ = true;

	// 上
	if (input->TriggerKey(DIK_UP)) {
		selectedItem_--;
		if (selectedItem_ < 0) selectedItem_ = 1;
	}

	// 下
	if (input->TriggerKey(DIK_DOWN)) {
		selectedItem_++;
		if (selectedItem_ > 1) selectedItem_ = 0;
	}


	// 選択項目が変わった
	if (previousItem != selectedItem_) {
		// 現在位置を移動開始位置にする
		startPositionStart_ = spriteStartText_->GetPosition();
		startPositionExit_ = spriteExitText_->GetPosition();

		// タイマーをリセット
		moveTimer_ = 0.0f;

		// 移動先を設定
		if (selectedItem_ == 0) {
			targetPositionStart_ = { 130.0f, 200.0f };
			targetPositionExit_ = { 100.0f, 300.0f };
		} else {
			targetPositionStart_ = { 100.0f, 200.0f };
			targetPositionExit_ = { 130.0f, 300.0f };
		}
	}


	// イージング
	if (moveTimer_ < moveDuration_) {
		moveTimer_ += 1.0f / 60.0f;
		float t = moveTimer_ / moveDuration_;
		if (t > 1.0f) t = 1.0f;
		
		spriteStartText_->SetPosition(
			Easing::Interpolate(
				EasingType::easeOutQuad, t,
				startPositionStart_, targetPositionStart_)
		);

		spriteExitText_->SetPosition(
			Easing::Interpolate(
				EasingType::easeOutQuad, t,
				startPositionExit_, targetPositionExit_)
		);
	}


}

void TitleMenu::Draw() {
	spriteStartText_->Draw();
	spriteExitText_->Draw();
}