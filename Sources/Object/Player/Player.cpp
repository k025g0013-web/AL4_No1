#define NOMINMAX
#include "Player.h"
#include "State/PlayerState.h"
#include "State/RootState.h"

#include "Math/Easing.h"
#include "Math/UpdateWorldTransform.h"

#include <utility>
#include <algorithm>
#include <cassert>
#include <numbers>
#include <memory>

using namespace KamataEngine;

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

	// 初期状態をRootStateに設定する
	RequestState(std::make_unique<RootState>());
	ApplyRequestedState();
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

	// 振る舞いの変更
	ApplyRequestedState();

	// 振る舞い更新
	if (state_) {
		state_->Update(*this);
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
	if (isAttack_) {
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
};
#pragma endregion

void Player::RequestState(std::unique_ptr<PlayerState> state) {
	requestedState_ = std::move(state);
}

void Player::ApplyRequestedState() {
	if (!requestedState_) {
		return;
	}

	state_ = std::move(requestedState_);
	state_->Initialize(*this);
}

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
					velocity_.x *= (1.0f - kTurnBrake);
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
					velocity_.x *= (1.0f - kTurnBrake);
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

			// 微小な速度は0にスナップ
			if (std::abs(velocity_.x) < kStopThreshold) {
				velocity_.x = 0.0f;
			}
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
		// 空中での左右移動
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			float dir = Input::GetInstance()->PushKey(DIK_RIGHT) ? 1.0f : -1.0f;

			// 速度と逆方向に入力時はブレーキ
			if (velocity_.x * dir < 0.0f) {
				velocity_.x *= (1.0f - kTurnBrake);
			}
			velocity_.x += dir * kAirAcceleration;

			// 向きの切り替え
			LRDirection newDirection = (dir > 0.0f) ? LRDirection::kRight : LRDirection::kLeft;
			if (lrDirection_ != newDirection) {
				lrDirection_ = newDirection;
				turnFirstRotationY_ = (dir > 0.0f) ? std::numbers::pi_v<float> *3.0f / 2.0f : std::numbers::pi_v<float> / 2.0f;
				turnTimer_ = kTimeTurn;
			}

			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は空中用の弱い減衰
			velocity_.x *= (1.0f - kAirAttenuation);
			if (std::abs(velocity_.x) < kStopThreshold) {
				velocity_.x = 0.0f;
			}
		}

		// 可変ジャンプ: 上昇中にボタンを離したら上昇を弱める
		if (!Input::GetInstance()->PushKey(DIK_UP) && velocity_.y > 0.0f) {
			velocity_.y *= kJumpCutRate;
		}

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
			std::array<Vector3, kNumCorner> positionsNew{};

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
	std::array<Vector3, kNumCorner> positionsNew{};

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
	std::array<Vector3, kNumCorner> positionsNew{};

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
	std::array<Vector3, kNumCorner> positionsNew{};

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
	std::array<Vector3, kNumCorner> positionsNew{};

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
	Vector3 worldPos{};
	// ワールド行列の並行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
};

void Player::OnCollision(const Enemy* enemy) {
	if (isAttack_ || isInvincible_) {
		return;
	}

	(void)enemy;
	// デスフラグを立てる
	isDead_ = true;
};

void Player::OnCollision(const ShieldEnemy* shieldEnemy) {
	if (isAttack_ || isInvincible_) {
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