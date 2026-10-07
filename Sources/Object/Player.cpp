#define NOMINMAX
#include "Player.h"
#include "Math/Easing.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include "Math/UpdateWorldTransform.h"

using namespace KamataEngine;

Player::Player() {};
Player::~Player() {
	delete model_;
	delete modelAttack_;
};

#pragma region 初期化・更新・描画
// 初期化処理
void Player::Initialize(Model* model, Model* modelAttack, Camera* camera, const Vector3& position) {
	// nullポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録する
	model_ = model;
	modelAttack_ = modelAttack;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// ワールド変換の初期化
	worldTransformAttack_.Initialize();
	worldTransformAttack_.translation_ = position;

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// 初期回転
	lrDirection_ = LRDirection::kRight;
	float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
	worldTransform_.rotation_.y = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	worldTransformAttack_.rotation_.y = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

	turnTimer_ = 0.0f;
};

// 更新処理
void Player::Update() {
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// 画面端による移動量の制限
	if (camera_) {
		float screenMarginX = 21.0f;
		float minX = camera_->translation_.x - screenMarginX + (kWidth / 2.0f);
		float maxX = camera_->translation_.x + screenMarginX - (kWidth / 2.0f);
		float nextX = worldTransform_.translation_.x + collisionMapInfo.move.x;

		if (nextX < minX) {
			collisionMapInfo.move.x = minX - worldTransform_.translation_.x;
			if (velocity_.x < 0.0f)
				velocity_.x = 0.0f;
		} else if (nextX > maxX) {
			collisionMapInfo.move.x = maxX - worldTransform_.translation_.x;
			if (velocity_.x > 0.0f)
				velocity_.x = 0.0f;
		}
	}

	// 移動量を加味して衝突判定する
	CollisionMap(collisionMapInfo);

	// 判定結果を反映して移動させる
	MoveOnResult(collisionMapInfo);

	// 壁との挟まれ判定
	if (camera_) {
		float screenMarginX = 21.0f;
		float minX = camera_->translation_.x - screenMarginX + (kWidth / 2.0f);
		float maxX = camera_->translation_.x + screenMarginX - (kWidth / 2.0f);
		CheckScreenAndWallSandwich(collisionMapInfo, minX, maxX);
	}

	// 各種チェッカーと接地切り替え
	IsHitCeilingChecker(collisionMapInfo);
	IsHitWallChecker(collisionMapInfo);
	InstallationStateSwitching(collisionMapInfo);

	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更
		behavior_ = behaviorRequest_;
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Player::Behavior::kRoot:
			BehaviorRootInitialize();
			break;
		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kKnockBack:
			BehaviorKnockBackInitialize();
			isInvincible_ = true;
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	// Behaviorの実行
	switch (behavior_) {
	case Player::Behavior::kRoot:
		BehaviorRootUpdate();
		break;
	case Player::Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kKnockBack:
		BehaviorKnockBackUpdate();
		break;
	}

	// 行列更新
	UpdateWorldTransform(worldTransform_);

	UpdateWorldTransform(worldTransformAttack_);
};

// 描画処理
void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);

	// 攻撃時にのみエフェクトを描画
	if (behavior_ == Behavior::kAttack) {
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
};
#pragma endregion

#pragma region 各Behaviorの処理
void Player::BehaviorRootInitialize() {}

void Player::BehaviorRootUpdate() {
	// 移動入力
	MoveInput();

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotation = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 自キャラの角度を設定する
		float t = 1.0f - std::clamp(turnTimer_ / kTimeTurn, 0.0f, 1.0f);
		worldTransform_.rotation_.y = turnFirstRotationY_ + (destinationRotation - turnFirstRotationY_) * t;
	}

	if (turnTimer_ <= 0.0f) {
		if (Input::GetInstance()->TriggerKey(DIK_Z)) {
			behaviorRequest_ = Behavior::kAttack;
		}
	}
}

void Player::BehaviorAttackInitialize() {
	attackParameter_ = 0;

	//	攻撃エフェクト用worldTransform初期化
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;
	worldTransformAttack_.scale_ = {1.0f, 1.0f, 1.0f};
}

void Player::BehaviorAttackUpdate() {
	// 自機とエフェクトのworldTransform同期
	worldTransformAttack_.translation_ = worldTransform_.translation_;
	worldTransformAttack_.rotation_ = worldTransform_.rotation_;

	// 予備動作
	attackParameter_++;

	// 移動速度の計算
	float currentMoveSpeed = 0.0f;
	if (attackPhase_ == AttackPhase::kMove) {
		currentMoveSpeed = 1.0f;
	}

	// カメラへの影響を防ぐため、メンバ変数の速度はゼロクリア
	velocity_ = {};

	// 攻撃中の移動量を計算するためのローカル変数
	Vector3 velocity = {};

	switch (attackPhase_) {
		// 溜め動作
	case AttackPhase::kCharge:
	default: {
		// 時間
		float t = static_cast<float>(attackParameter_) / chargeTime_;
		t = std::clamp(t, 0.0f, 1.0f);

		// 変形
		worldTransform_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 1.6f);
		worldTransform_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 0.3f);

		worldTransformAttack_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 0.0f, 0.5f);
		worldTransformAttack_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 0.0f, 0.5f);

		// 突進動作へ移行
		if (attackParameter_ >= chargeTime_) {
			attackPhase_ = AttackPhase::kMove;
			attackParameter_ = 0; // カウンターをリセット
		}
		break;
	}

		// 突進動作
	case AttackPhase::kMove: {
		// 向き
		if (lrDirection_ == LRDirection::kLeft) {
			velocity.x = -currentMoveSpeed;
		} else if (lrDirection_ == LRDirection::kRight) {
			velocity.x = currentMoveSpeed;
		}

		// 時間
		float t = static_cast<float>(attackParameter_) / attackTime_;
		t = std::clamp(t, 0.0f, 1.0f);

		// 変形
		worldTransform_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 1.6f, 0.6f);
		worldTransform_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 0.3f, 1.4f);

		worldTransformAttack_.scale_.x = Easing::Interpolate(EasingType::easeInQuad, t, 0.5f, 4.0f);
		worldTransformAttack_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 0.5f, 1.0f);
		worldTransformAttack_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 0.5f, 1.0f);

		float offset = 2.0f;
		if (lrDirection_ == LRDirection::kRight) {
			worldTransformAttack_.translation_.x = worldTransform_.translation_.x + offset;
		} else {
			worldTransformAttack_.translation_.x = worldTransform_.translation_.x - offset;
		}

		// 余韻動作へ移行
		if (attackParameter_ >= attackTime_) {
			attackPhase_ = AttackPhase::kCoolTime;
			attackParameter_ = 0; // カウンターをリセット
		}
		break;
	}

	// 余韻動作
	case AttackPhase::kCoolTime: {
		// 時間
		float t = static_cast<float>(attackParameter_) / coolTime_;
		t = std::clamp(t, 0.0f, 1.0f);

		// 変形
		worldTransform_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 1.3f, 1.0f);
		worldTransform_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 0.7f, 1.0f);

		worldTransformAttack_.scale_.y = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 0.0f);
		worldTransformAttack_.scale_.z = Easing::Interpolate(EasingType::easeInQuad, t, 1.0f, 0.0f);

		// 通常状態に戻す
		if (attackParameter_ >= coolTime_) {
			attackPhase_ = AttackPhase::kCharge; // 初期化
			behaviorRequest_ = Behavior::kRoot;  // 通常状態へ
		}
		break;
	}
	}

	// 攻撃用の衝突判定情報を初期化して用意
	CollisionMapInfo attackCollisionInfo;
	attackCollisionInfo.move = velocity; // 計算したローカル移動量を設定

	CollisionMap(attackCollisionInfo);

	MoveOnResult(attackCollisionInfo);

	IsHitWallChecker(attackCollisionInfo);
	InstallationStateSwitching(attackCollisionInfo);
}

void Player::BehaviorKnockBackInitialize() {
	knockBackTimer_ = 0;

	// 攻撃を中断
	attackPhase_ = AttackPhase::kCharge;
	attackParameter_ = 0;

	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	worldTransformAttack_.scale_ = {0.0f, 0.0f, 0.0f};

	if (lrDirection_ == LRDirection::kRight) {
		knockBackVelocity_.x = -0.8f;
	} else {
		knockBackVelocity_.x = 0.8f;
	}

	knockBackVelocity_.y = 0.0f;
}

void Player::BehaviorKnockBackUpdate() {

	knockBackTimer_++;

	CollisionMapInfo info;
	info.move = knockBackVelocity_;

	CollisionMap(info);
	MoveOnResult(info);

	// 徐々に止まる
	knockBackVelocity_.x *= 0.8f;

	if (knockBackTimer_ >= kKnockBackTime) {
		isInvincible_ = false;
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::RequestKnockBack() { behaviorRequest_ = Behavior::kKnockBack; }
#pragma endregion

#pragma region 更新処理全体の流れ
// 移動入力
void Player::MoveInput() {
	// 接地状態
	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力時は急ブレーキ
					velocity_.x *= (1.0f - kAcceleration);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = std::numbers::pi_v<float> * 3.0f / 2.0f;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 左移動中の右入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力時は急ブレーキ
					velocity_.x *= (1.0f - kAcceleration);
				}
				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = std::numbers::pi_v<float> / 2.0f;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速 / 減速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は速度減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 落下状態
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
};

// 移動量を加味して衝突判定する
void Player::CollisionMap(CollisionMapInfo& info) {
	CollisionMapTop(info);
	CollisionMapBottom(info);
	CollisionMapRight(info);
	CollisionMapLeft(info);
};

// 判定結果を反映して移動させる
void Player::MoveOnResult(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
};

// 天井に接触している場合の処理
void Player::IsHitCeilingChecker(const CollisionMapInfo& info) {
	// 天井に当たったか？
	if (info.ceiling) {
		velocity_.y = 0;
	}
}

// 壁に接触している場合の処理
void Player::IsHitWallChecker(const CollisionMapInfo& info) {
	if (info.wall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
};

// 接地状態の切り替え
void Player::InstallationStateSwitching(CollisionMapInfo& info) {
	if (onGround_) {
		// 接地状態
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			MapChipType mapChipType;

			// 移動後の4つの角の座標
			std::array<Vector3, kNumCorner> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] =
				    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
			}

			Vector3 leftBottom = {positionsNew[kLeftBottom].x, positionsNew[kLeftBottom].y - kBlank, positionsNew[kLeftBottom].z};

			Vector3 rightBottom = {positionsNew[kRightBottom].x, positionsNew[kRightBottom].y - kBlank, positionsNew[kRightBottom].z};

			// 真下の当たり判定を行う
			bool hit = false;

			// 左下点の判定
			MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(leftBottom);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(rightBottom);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替わる
				onGround_ = false;
			}
		}
	} else {
		// 空中状態
		if (info.landing) {
			// 着地状態に切り替える
			onGround_ = true;
			// 着地時にX速度を減衰
			velocity_.x *= (1.0f - kAttenuationLanding);
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}
};
#pragma endregion

#pragma region マップチップの当たり判定
// 指定した角の座標計算
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };

	Vector3 offset = offsetTable[static_cast<uint32_t>(corner)];
	return {center.x + offset.x, center.y + offset.y, center.z + offset.z};
};

// 上方向の当たり判定
void Player::CollisionMapTop(CollisionMapInfo& info) {
	// 上昇あり？
	if (info.move.y < 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		Vector3 topBefore = {worldTransform_.translation_.x, worldTransform_.translation_.y + kHeight / 2.0f - kBlank, worldTransform_.translation_.z};

		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(topBefore);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::max(0.0f, (rect.bottom - worldTransform_.translation_.y) - (kHeight / 2.0f - kBlank));

			// 天井に当たったことを記録する
			info.ceiling = true;
		}
	}
};

// 下方向の当たり判定
void Player::CollisionMapBottom(CollisionMapInfo& info) {
	// 下降あり？
	if (info.move.y > 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	// 隣接セルがともにブロックであればヒット
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	// 隣接セルがともにブロックであればヒット
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		Vector3 bottomBefore = {worldTransform_.translation_.x, worldTransform_.translation_.y - kHeight / 2.0f + kBlank, worldTransform_.translation_.z};

		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(bottomBefore);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + (kHeight / 2.0f + kBlank));

			// 地面に当たったことを記録する
			info.landing = true;
		}
	}
};

// 右方向の当たり判定
void Player::CollisionMapRight(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.move.x < 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 右側の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		Vector3 rightBefore = {worldTransform_.translation_.x + kWidth / 2.0f, worldTransform_.translation_.y, worldTransform_.translation_.z};

		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(rightBefore);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::min(info.move.x, (rect.left - worldTransform_.translation_.x) - (kWidth / 2.0f + kBlank));

			// 壁に当たったことを判定結果に記録する
			info.wall = true;
		}
	}
};

// 左方向の当たり判定
void Player::CollisionMapLeft(CollisionMapInfo& info) {
	// 左移動あり？
	if (info.move.x > 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] =
		    CornerPosition({worldTransform_.translation_.x + info.move.x, worldTransform_.translation_.y + info.move.y, worldTransform_.translation_.z + info.move.z}, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 左側の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		Vector3 leftBefore = {worldTransform_.translation_.x - kWidth / 2.0f, worldTransform_.translation_.y, worldTransform_.translation_.z};

		// 現在座標が壁の外か判定
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(leftBefore);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// めり込み先ブロックの範囲矩形
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.move.x = std::max(info.move.x - 1.0f, (rect.right - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank));

			// 地面に当たったことを記録する
			info.wall = true;
		}
	}
};
#pragma endregion

#pragma region AABBによる当たり判定
Vector3 Player::GetWorldPosition() const {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の並行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
};

void Player::OnCollision(const Enemy* enemy) {
	if (behavior_ == Behavior::kAttack || behavior_ == Behavior::kKnockBack || isInvincible_) {
		return;
	}

	(void)enemy;
	// デスフラグを立てる
	isDead_ = true;
};

void Player::OnCollision(const ShieldEnemy* shieldEnemy) {
	if (behavior_ == Behavior::kAttack || behavior_ == Behavior::kKnockBack || isInvincible_) {
		return;
	}

	(void)shieldEnemy;
	// デスフラグを立てる
	isDead_ = true;
};
#pragma endregion

// 画面端と壁に挟まれたかどうかの判定
void Player::CheckScreenAndWallSandwich(const CollisionMapInfo& info, float minX, float maxX) {
	// プレイヤーが画面左端に接触しているか？
	bool isAtLeftScreenEdge = (worldTransform_.translation_.x <= minX);

	// マップチップ右側の壁に衝突しているか？
	bool isHitRightWall = info.wall && (lrDirection_ == LRDirection::kRight || velocity_.x >= 0.0f);

	// 左画面端と右の壁に挟まれたら即死
	if (isAtLeftScreenEdge && isHitRightWall) {
		isDead_ = true;
		return;
	}

	// 逆方向（画面右端と左の壁に挟まれたケース
	bool isAtRightScreenEdge = (worldTransform_.translation_.x >= maxX);
	bool isHitLeftWall = info.wall && (lrDirection_ == LRDirection::kLeft || velocity_.x <= 0.0f);

	if (isAtRightScreenEdge && isHitLeftWall) {
		isDead_ = true;
		return;
	}
}