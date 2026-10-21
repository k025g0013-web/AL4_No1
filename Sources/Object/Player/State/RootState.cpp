#include "RootState.h"

#include "Object/Player/Player.h"
#include "AttackState.h"

#include <algorithm>
#include <numbers>

using namespace KamataEngine;

void RootState::Initialize(Player &player) {
   player.isAttack_ = false;
}

void RootState::Update(Player &player) {
    // 移動入力
	player.MoveInput();

	// 旋回制御
	if (player.turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		player.turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> *3.0f / 2.0f };
		// 状態に応じた角度を取得する
		float destinationRotation = destinationRotationYTable[static_cast<uint32_t>(player.lrDirection_)];

		// 自キャラの角度を設定する
		float t = 1.0f - std::clamp(player.turnTimer_ / player.kTimeTurn, 0.0f, 1.0f);
		player.worldTransform_.rotation_.y = player.turnFirstRotationY_ + (destinationRotation - player.turnFirstRotationY_) * t;
	}

	if (player.turnTimer_ <= 0.0f) {
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			player.isAttack_ = true;
			player.RequestState(std::make_unique<AttackState>());
		}
	}
}