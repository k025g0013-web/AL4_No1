#include "ShieldEnemy.h"
#include "audio/Audio.h"

#include "Object/Player/Player.h"
#include "Object/Player/State/KnockBackState.h"
#include "Scene/GameScene.h"

#include "Math/UpdateWorldTransform.h"

#include "Effect/HitEffect.h"
#include "Effect/GuardEffect.h"

using namespace KamataEngine;

void ShieldEnemy::Initialize(Model *model, KamataEngine::Camera *camera, const Vector3 &position) {
	// 基底クラスの初期化
	Enemy::Initialize(model, camera, position);

	// ガード状態を初期化
	guardTimer_ = 0;
	isGuarding_ = false;

	auto *audio = Audio::GetInstance();
	guardSEHandle_ = audio->LoadWave("seGuard.mp3");
}

void ShieldEnemy::Update() {
	// 振る舞い変更
	if (behaviorRequest_ != Behavior::kUnknown) {
		behavior_ = behaviorRequest_;

		switch (behavior_) {
		case Behavior::kWalk:
			BehaviorWalkInitialize();
			break;

		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;

		case Behavior::kGuard:
			BehaviorGuardInitialize();
			break;

		default:
			break;
		}

		behaviorRequest_ = Behavior::kUnknown;
	}

	// 現在の振る舞いを実行
	switch (behavior_) {
	case Behavior::kWalk:
		BehaviorWalkUpdate();
		Move(true);
		break;

	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;

	case Behavior::kGuard:
		BehaviorGuardUpdate();
		break;

	default:
		break;
	}

	UpdateTurnAnimation();

	// ワールド行列更新
	UpdateWorldTransform(worldTransform_);
}

void ShieldEnemy::OnCollision(Player *player) {
	if (GetIsCollisionDisabled()) {
		return;
	}

	if (
		player->IsAttack() ||
		player->GetWorldTransform().scale_.z > 1.0f
		) {
		Vector3 effectPos{
			(
				worldTransform_.translation_.x +
				player->GetWorldTransform().translation_.x
			) / 2.0f,

			(
				worldTransform_.translation_.y +
				player->GetWorldTransform().translation_.y
			) / 2.0f,

			(
				worldTransform_.translation_.z +
				player->GetWorldTransform().translation_.z
			) / 2.0f,
		};

		// 正面からの攻撃ならガード
		if (IsFrontAttack(player)) {
			if (gameScene_ && gameScene_->GetEffectManager()) {
				gameScene_->GetEffectManager()->Create<GuardEffect>(effectPos);
			}

			player->RequestState(std::make_unique<KnockBackState>());

			behaviorRequest_ = Behavior::kGuard;

			Audio::GetInstance()->PlayWave(guardSEHandle_, false, 1.0f);

			return;
		}

		// 背後からの攻撃なら死亡
		SetDeathVelocity(player);

		behaviorRequest_ = Behavior::kDeath;

		if (gameScene_ && gameScene_->GetEffectManager()) {
			gameScene_->GetEffectManager()->Create<HitEffect>(effectPos);
		}

		return;
	}
}

bool ShieldEnemy::IsFrontAttack(const Player *player) const {
	return (
		(
			player->GetDirection() == LRDirection::kRight &&
			direction_ == LRDirection::kLeft
			) ||
		(
			player->GetDirection() == LRDirection::kLeft &&
			direction_ == LRDirection::kRight
			)
		);
}

void ShieldEnemy::BehaviorGuardInitialize() {
	guardTimer_ = 0;
	isGuarding_ = true;

	// プレイヤーと逆方向へノックバック
	if (direction_ == LRDirection::kLeft) {
		velocity_.x = 0.15f;
	} else {
		velocity_.x = -0.15f;
	}

	velocity_.y = 0.0f;
}

void ShieldEnemy::BehaviorGuardUpdate() {
	guardTimer_++;

	// ノックバック移動
	worldTransform_.translation_.x += velocity_.x;

	// 徐々に減速
	velocity_.x *= 0.8f;

	// ガード終了
	if (guardTimer_ >= kGuardTime) {
		isGuarding_ = false;
		behaviorRequest_ = Behavior::kWalk;
	}
}

void ShieldEnemy::ApplyWalkAnimation(float degree) {
	float baseRotation = 0.0f;

	if (direction_ == LRDirection::kLeft) {
		baseRotation = 3.0f * std::numbers::pi_v<float> / 2.0f;
	} else {
		baseRotation = std::numbers::pi_v<float> / 2.0f;
	}

	worldTransform_.rotation_.y =
		baseRotation + degree * std::numbers::pi_v<float> / 180.0f;
}