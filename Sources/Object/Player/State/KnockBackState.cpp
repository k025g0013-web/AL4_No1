#include "KnockBackState.h"

#include "Object/Player/Player.h"
#include "RootState.h"

#include "Math/Easing.h"

using namespace KamataEngine;

void KnockBackState::Initialize(Player &player) {
	player.knockBackTimer_ = 0;

	// 攻撃を中断
	player.attackPhase_ = Player::AttackPhase::kCharge;
	player.attackParameter_ = 0;

	player.worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
	player.worldTransformAttack_.scale_ = { 0.0f, 0.0f, 0.0f };

	if (player.lrDirection_ == LRDirection::kRight) {
		player.knockBackVelocity_.x = -0.8f;
	} else {
		player.knockBackVelocity_.x = 0.8f;
	}

	player.knockBackVelocity_.y = 0.0f;
}

void KnockBackState::Update(Player &player) {
	player.knockBackTimer_++;

	CollisionMapInfo info{};
	info.move = player.knockBackVelocity_;

	player.CheckMapCollision(info);
	player.MoveOnResult(info);

	// 徐々に止まる
	player.knockBackVelocity_.x *= 0.8f;

	if (player.knockBackTimer_ >= player.kKnockBackTime) {
		player.isAttack_ = false;
		player.isInvincible_ = false;
		player.RequestState(std::make_unique<RootState>());
	}
}