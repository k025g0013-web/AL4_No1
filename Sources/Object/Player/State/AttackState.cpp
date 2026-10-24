#include "AttackState.h"

#include "Object/Player/Player.h"
#include "RootState.h"

#include "Math/Easing.h"

#include <numbers>

using namespace KamataEngine;

void AttackState::Initialize(Player &player) {
	player.attackParameter_ = 0;

	//	攻撃エフェクト用worldTransform初期化
	player.worldTransformAttack_.translation_ =
		player.worldTransform_.translation_;

	float destinationRotationYTable[] = {
		std::numbers::pi_v<float> / 2.0f,
		std::numbers::pi_v<float> *3.0f / 2.0f
	};

	player.worldTransformAttack_.rotation_.y =
		destinationRotationYTable[
			static_cast<uint32_t>(player.attackDirection_)
		];

	player.worldTransformAttack_.scale_ = { 1.0f, 1.0f, 1.0f };
}

void AttackState::Update(Player &player) {
	// 攻撃エフェクトの位置をプレイヤーに同期
	player.worldTransformAttack_.translation_ =
		player.worldTransform_.translation_;

	// 攻撃方向に合わせてエフェクトの向きを設定
	float destinationRotationYTable[] = {
		std::numbers::pi_v<float> / 2.0f,
		std::numbers::pi_v<float> *3.0f / 2.0f
	};

	player.worldTransformAttack_.rotation_.y =
		destinationRotationYTable[
			static_cast<uint32_t>(player.attackDirection_)
		];

	// 予備動作
	player.attackParameter_++;

	// 移動速度の計算
	float currentMoveSpeed = 0.0f;
	if (player.attackPhase_ == Player::AttackPhase::kMove) {
		currentMoveSpeed = 1.0f;
	}

	// カメラへの影響を防ぐため、メンバ変数の速度はゼロクリア
	player.velocity_ = {};

	// 攻撃中の移動量を計算するためのローカル変数
	Vector3 velocity = {};

	switch (player.attackPhase_) {
		// 溜め動作
	case Player::AttackPhase::kCharge:
	default: {
		// 時間
		float t = static_cast<float>(player.attackParameter_) / player.chargeTime_;
		t = std::clamp(t, 0.0f, 1.0f);

		// 変形
		player.worldTransform_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 1.6f);
		player.worldTransform_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 0.3f);

		player.worldTransformAttack_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 0.0f, 0.5f);
		player.worldTransformAttack_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 0.0f, 0.5f);

		// 突進動作へ移行
		if (player.attackParameter_ >= player.chargeTime_) {
			player.attackPhase_ = Player::AttackPhase::kMove;
			player.attackParameter_ = 0;

			// ダッシュ演出を開始
			player.isDashEffectActive_ = true;
		}
		break;
	}

		   // 突進動作
	case Player::AttackPhase::kMove: {
		// 向き
		if (player.lrDirection_ == LRDirection::kLeft) {
			velocity.x = -currentMoveSpeed;
		} else if (player.lrDirection_ == LRDirection::kRight) {
			velocity.x = currentMoveSpeed;
		}

		// 時間
		float t = static_cast<float>(player.attackParameter_) / player.attackTime_;
		t = std::clamp(t, 0.0f, 1.0f);

		// 変形
		player.worldTransform_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 1.6f, 0.6f);
		player.worldTransform_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 0.3f, 1.4f);

		player.worldTransformAttack_.scale_.x = Easing::Interpolate(EasingType::easeInQuad, t, 0.5f, 4.0f);
		player.worldTransformAttack_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 0.5f, 1.0f);
		player.worldTransformAttack_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 0.5f, 1.0f);

		float offset = 2.0f;
		if (player.lrDirection_ == LRDirection::kRight) {
			player.worldTransformAttack_.translation_.x = player.worldTransform_.translation_.x + offset;
		} else {
			player.worldTransformAttack_.translation_.x = player.worldTransform_.translation_.x - offset;
		}

		// 余韻動作へ移行
		if (player.attackParameter_ >= player.attackTime_) {
			player.attackPhase_ = Player::AttackPhase::kCoolTime;
			player.attackParameter_ = 0;

			// ダッシュ演出を終了
			player.isDashEffectActive_ = false;
		}
		break;
	}

	// 余韻動作
	case Player::AttackPhase::kCoolTime: {
		// 時間
		float t = static_cast<float>(player.attackParameter_) / player.coolTime_;
		t = std::clamp(t, 0.0f, 1.0f);

		// 変形
		player.worldTransform_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 1.3f, 1.0f);
		player.worldTransform_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 0.7f, 1.0f);

		player.worldTransformAttack_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 0.0f);
		player.worldTransformAttack_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 0.0f);

		// 通常状態に戻す
		if (player.attackParameter_ >= player.coolTime_) {
			player.attackPhase_ = Player::AttackPhase::kCharge;	// 初期化
			player.RequestState(std::make_unique<RootState>());	// 通常状態へ
		}
		break;
	}
	}

	// 攻撃用の衝突判定情報を初期化して用意
	CollisionMapInfo attackCollisionInfo{};
	// 計算したローカル移動量を設定
	attackCollisionInfo.move = velocity;

	player.CheckMapCollision(attackCollisionInfo);

	player.MoveOnResult(attackCollisionInfo);

	player.IsHitWallChecker(attackCollisionInfo);
	player.InstallationStateSwitching(attackCollisionInfo);
}