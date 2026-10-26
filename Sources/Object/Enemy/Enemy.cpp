#include "Enemy.h"
#include "audio/Audio.h"

#include "Object/Player/Player.h"
#include "Scene/GameScene.h"

#include <cassert>
#include <cmath>
#include <numbers>

#include "Math/UpdateWorldTransform.h"

#include "Effect/HitEffect.h"

using namespace KamataEngine;

void Enemy::Initialize(Model *model, KamataEngine::Camera *camera, const Vector3 &position) {
#ifdef _DEBUG
	assert(model);
#endif

	// モデル・カメラを保存
	model_ = model;
	camera_ = camera;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期回転
	worldTransform_.rotation_.y =
		3.0f * std::numbers::pi_v<float> / 2.0f;

	// 初期状態
	behavior_ = Behavior::kWalk;
	behaviorRequest_ = Behavior::kUnknown;

	// 初期向き
	direction_ = LRDirection::kLeft;

	onGround_ = false;
	isDead_ = false;

	// 音声読み込み
	auto *audio = Audio::GetInstance();
	deathSEHandle_ = audio->LoadWave("seDeath.mp3");

	// 歩行状態の初期化
	BehaviorWalkInitialize();
}

void Enemy::Update() {
	// 振る舞い変更リクエストがある場合
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更
		behavior_ = behaviorRequest_;

		// 各振る舞いの初期化
		switch (behavior_) {
		case Behavior::kWalk:
			BehaviorWalkInitialize();
			break;

		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		}

		// リクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	// 現在のBehaviorを実行
	switch (behavior_) {
	case Behavior::kWalk:
		BehaviorWalkUpdate();
		Move(false);
		break;

	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}

	// 回転アニメーション
	UpdateTurnAnimation();

	// ワールド行列を更新
	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}


// 移動処理
void Enemy::Move(bool applyGravity) {
	// 前方にブロックがあれば方向転換
	if (IsFrontBlocked()) {
		Turn();
	}

	// 重力の適用
	if (applyGravity) {
		if (!onGround_) {
			velocity_.y -= kGravity;
		}
	} else {
		// 空中浮遊する敵はY方向に移動しない
		velocity_.y = 0.0f;
	}

	// 移動量を設定
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	// マップとの衝突判定
	collision_.CheckMap(
		collisionMapInfo,
		worldTransform_.translation_,
		kWidth,
		kHeight
	);

	// 移動
	worldTransform_.translation_.x += collisionMapInfo.move.x;
	worldTransform_.translation_.y += collisionMapInfo.move.y;

	// 重力を使用する敵のみ接地状態を更新
	if (applyGravity) {
		InstallationStateSwitching(collisionMapInfo);
	}
}

// 歩行Behavior初期化
void Enemy::BehaviorWalkInitialize() {
	// 歩行タイマーをリセット
	walkTimer_ = 0.0f;

	// 向きに応じて移動速度を設定
	if (direction_ == LRDirection::kLeft) {
		velocity_.x = -kWalkSpeed;

	} else {
		velocity_.x = kWalkSpeed;
	}

	// 落下速度をリセット
	velocity_.y = 0.0f;
}

// 歩行Behavior更新
void Enemy::BehaviorWalkUpdate() {
	// 経過時間
	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(
		walkTimer_ / kWalkMotionTime *
		(2.0f * std::numbers::pi_v<float>)
	);

	float degree =
		kWalkMotionAngleStart +
		kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	// 歩行アニメーションを適用
	ApplyWalkAnimation(degree);
}

// 通常の敵の歩行アニメーション
void Enemy::ApplyWalkAnimation(float degree) {
	worldTransform_.rotation_.x =
		degree * std::numbers::pi_v<float> / 180.0f;
}

void Enemy::SetDeathVelocity(const Player *player) {
	if (!player) {
		return;
	}

	const float playerX =
		player->GetWorldTransform().translation_.x;
	const float enemyX =
		worldTransform_.translation_.x;

	// プレイヤーと反対方向へ吹っ飛ぶ
	if (enemyX < playerX) {
		// 敵が左、プレイヤーが右
		deathDirection_ = -1.0f;
	} else {
		// 敵が右、プレイヤーが左
		deathDirection_ = 1.0f;
	}
}

// 死亡Behavior初期化
void Enemy::BehaviorDeathInitialize() {
	Audio::GetInstance()->PlayWave(deathSEHandle_, false, 1.0f);

	deathTimer_ = 0;

	// 吹っ飛ぶ速度
	velocity_.x = 0.1f * deathDirection_;
	velocity_.y = 0.3f;

	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.z = 0.0f;

	// 回転速度と方向
	deathRotationVelocity_.x = 0.0f;
	deathRotationVelocity_.y = 0.0f;
	deathRotationVelocity_.z = 0.2f * deathDirection_;

	// 方向転換アニメーション停止
	isTurning_ = false;
}

// 死亡Behavior更新
void Enemy::BehaviorDeathUpdate() {
	// 死亡タイマー
	deathTimer_++;

	// 重力
	velocity_.y -= kGravity;

	// 座標に反映
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// 死亡時の回転
	worldTransform_.rotation_.x += deathRotationVelocity_.x;
	worldTransform_.rotation_.y += deathRotationVelocity_.y;
	worldTransform_.rotation_.z += deathRotationVelocity_.z;

	// 一定時間経過したら消滅
	if (deathTimer_ >= kDeathTime) {
		isDead_ = true;
	}
}

// 前方に障害物があるか
bool Enemy::IsFrontBlocked() const {
	if (!mapChipField_) {
		return false;
	}

	// 敵の中心位置
	const Vector3 &position = worldTransform_.translation_;

	// 敵の前方にあるX座標
	float frontX = position.x;

	if (direction_ == LRDirection::kLeft) {
		frontX -= kWidth / 2.0f + 0.01f;
	} else {
		frontX += kWidth / 2.0f + 0.01f;
	}

	// 敵の上側と下側の座標
	float topY = position.y + kHeight / 2.0f;
	float bottomY = position.y - kHeight / 2.0f;

	// 上下2点で前方のブロックを調べる
	Vector3 topPosition = { frontX, topY, position.z };
	Vector3 bottomPosition = { frontX, bottomY, position.z };

	MapChipField::IndexSet topIndex =
		mapChipField_->GetMapChipIndexSetByPosition(topPosition);

	MapChipField::IndexSet bottomIndex =
		mapChipField_->GetMapChipIndexSetByPosition(bottomPosition);

	// マップの外側なら壁として扱う
	int xIndex = static_cast<int>(topIndex.xIndex);

	if (direction_ == LRDirection::kLeft) {
		xIndex = static_cast<int>(bottomIndex.xIndex);
	}

	if (xIndex < 0 ||
		xIndex >= static_cast<int>(mapChipField_->GetNumBlockHorizontal())) {
		return true;
	}

	// 上下いずれかのマスがブロックなら反転
	return
		mapChipField_->GetMapChipTypeByIndex(
			topIndex.xIndex, topIndex.yIndex) == MapChipType::kBlock
		||
		mapChipField_->GetMapChipTypeByIndex(
			bottomIndex.xIndex, bottomIndex.yIndex) == MapChipType::kBlock;
}

void Enemy::InstallationStateSwitching(
	const CollisionMapInfo &info
) {
	if (!mapChipField_) {
		onGround_ = false;
		return;
	}

	if (onGround_) {

		// 上方向へ移動した場合は空中へ
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}

	} else {

		// 空中から着地した場合
		if (info.landing) {
			onGround_ = true;

			// 着地時にX速度を少し減衰
			velocity_.x *=
				(1.0f - kAttenuationLanding);

			// Y速度をリセット
			velocity_.y = 0.0f;
		}
	}
}

// 向きを反転
void Enemy::Turn() {
	if (direction_ == LRDirection::kLeft) {
		// 左方向から右方向へ反転
		direction_ = LRDirection::kRight;
		velocity_.x = kWalkSpeed;
	} else {
		// 右方向から左方向へ反転
		direction_ = LRDirection::kLeft;
		velocity_.x = -kWalkSpeed;
	}

	// 回転アニメーション開始
	isTurning_ = true;
	turnTimer_ = 0.0f;
	turnStartRotationY_ = worldTransform_.rotation_.y;
}

// ワールド座標取得
Vector3 Enemy::GetWorldPosition() const {
	Vector3 worldPos{};
	// ワールド行列から平行移動成分を取得
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

// 衝突応答
void Enemy::OnCollision(Player *player) {
	// 衝突判定が無効なら何もしない
	if (GetIsCollisionDisabled()) {
		return;
	}

	// プレイヤーの攻撃判定
	if (player->IsAttack() ||
		player->GetWorldTransform().scale_.z > 1.0f) {

		SetDeathVelocity(player);


		// 死亡状態へ
		behaviorRequest_ = Behavior::kDeath;

		// ヒットエフェクト位置
		Vector3 effectPos{
			(worldTransform_.translation_.x + player->GetWorldTransform().translation_.x) / 2.0f,
			(worldTransform_.translation_.y + player->GetWorldTransform().translation_.y) / 2.0f,
			(worldTransform_.translation_.z + player->GetWorldTransform().translation_.z) / 2.0f
		};

		// ヒットエフェクト生成
		if (gameScene_ && gameScene_->GetEffectManager()) {
			gameScene_->GetEffectManager()->Create<HitEffect>(effectPos);
		}

		return;
	}
}

void Enemy::UpdateTurnAnimation() {
	if (isTurning_) {
		turnTimer_ += 1.0f / 60.0f;

		float t = turnTimer_ / kTurnTime;

		if (t >= 1.0f) {
			t = 1.0f;
			isTurning_ = false;
		}

		worldTransform_.rotation_.y =
			turnStartRotationY_ + kTurnAngle * t;
	}
}