#include "Enemy.h"

#include "Object/Player.h"
#include "Scene/GameScene.h"

#include <cassert>
#include <cmath>
#include <numbers>

#include "Math/UpdateWorldTransform.h"

using namespace KamataEngine;

void Enemy::Initialize(Model *model, Camera *camera, const Vector3 &position) {
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
		break;

	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}

	// ワールド行列を更新
	UpdateWorldTransform(worldTransform_);
}

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
}


void Enemy::Move() {
	// 前方にブロックがあれば方向転換
	if (IsFrontBlocked()) {
		Turn();
	}

	// Y方向は一切動かさない
	velocity_.y = 0.0f;

	// 現在の移動量を衝突判定へ渡す
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.move = velocity_;

	collision_.CheckMap(
		collisionMapInfo,
		worldTransform_.translation_,
		kWidth,
		kHeight
	);

	// 衝突判定後の移動量を反映
	worldTransform_.translation_.x +=
		collisionMapInfo.move.x;

	worldTransform_.translation_.y +=
		collisionMapInfo.move.y;
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
	// 共通移動処理
	Move();

	// 経過時間
	walkTimer_ += 1.0f / 60.0f;

	float param =
		std::sin(walkTimer_ / kWalkMotionTime * (2.0f * std::numbers::pi_v<float>));

	float degree =
		kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;

	worldTransform_.rotation_.x =
		degree * std::numbers::pi_v<float> / 180.0f;
}

// 死亡Behavior初期化
void Enemy::BehaviorDeathInitialize() {
	// 死亡タイマーをリセット
	deathTimer_ = 0;

	// 吹き飛ぶ速度
	velocity_.x = 0.1f;
	velocity_.y = 0.3f;
}

// 死亡Behavior更新
void Enemy::BehaviorDeathUpdate() {
	// 死亡タイマー
	deathTimer_++;

	// 重力
	velocity_.y -= 0.01f;

	// 座標に反映
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// 回転
	worldTransform_.rotation_.z += 0.2f;
	worldTransform_.rotation_.x += 0.1f;

	// 一定時間経過したら消滅
	if (deathTimer_ >= kDeathTime) {
		isDead_ = true;
	}
}

// 前方に障害物があるか
bool Enemy::IsFrontBlocked() const {
	// マップチップが設定されていなければ判定しない
	if (!mapChipField_) {
		return false;
	}

	// 現在位置からマップチップ番号を取得
	MapChipField::IndexSet indexSet =
		mapChipField_->GetMapChipIndexSetByPosition(
			worldTransform_.translation_
		);

	int nextXIndex = static_cast<int>(indexSet.xIndex);

	// 向いている方向の隣のマップチップを調べる
	if (direction_ == LRDirection::kLeft) {
		nextXIndex--;
	} else {
		nextXIndex++;
	}

	// マップの外側
	if (nextXIndex < 0 ||
		nextXIndex >= static_cast<int>(
			mapChipField_->GetNumBlockHorizontal())) {
		// マップ外を壁として扱う
		return true;
	}

	// 前方のマップチップを取得
	MapChipType type =
		mapChipField_->GetMapChipTypeByIndex(
			static_cast<uint32_t>(nextXIndex),
			indexSet.yIndex
		);

	// ブロックなら障害物あり
	return type == MapChipType::kBlock;
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
		// 左 -> 右
		direction_ = LRDirection::kRight;
		velocity_.x = kWalkSpeed;
	} else {
		// 右 -> 左
		direction_ = LRDirection::kLeft;
		velocity_.x = -kWalkSpeed;
	}
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
		// 死亡状態へ
		behaviorRequest_ = Behavior::kDeath;

		// ヒットエフェクト位置
		Vector3 effectPos{
			(worldTransform_.translation_.x + player->GetWorldTransform().translation_.x) / 2.0f,
			(worldTransform_.translation_.y + player->GetWorldTransform().translation_.y) / 2.0f,
			(worldTransform_.translation_.z + player->GetWorldTransform().translation_.z) / 2.0f
		};

		// ヒットエフェクト生成
		if (gameScene_) {
			gameScene_->CreateHitEffect(effectPos);
		}

		return;
	}
}