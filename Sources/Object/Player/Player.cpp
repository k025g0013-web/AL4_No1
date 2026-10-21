#define NOMINMAX
#include "Player.h"
#include "State/PlayerState.h"
#include "State/RootState.h"
#include "State/KnockBackState.h"

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
void Player::Initialize(Model *model, Model *modelAttack, Camera *camera, const Vector3 &position) {
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
	float destinationRotationYTable[] = { std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> *3.0f / 2.0f };
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
	CheckMapCollision(collisionMapInfo);

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

	// ダメージ後の無敵時間を更新
	if (damageInvincibleTimer_ > 0.0f) {
		damageInvincibleTimer_ -= 1.0f / 60.0f;

		if (damageInvincibleTimer_ < 0.0f) {
			damageInvincibleTimer_ = 0.0f;
		}
	}

	// ダメージ演出の更新
	if (damageAnimationTimer_ > 0.0f) {
		damageAnimationTimer_ -= 1.0f / 60.0f;

		if (damageAnimationTimer_ < 0.0f) {
			damageAnimationTimer_ = 0.0f;
		}
	}

	// ダメージ演出の更新
	DamageAnimation();

	// 行列更新
	UpdateWorldTransform(worldTransform_);

	UpdateWorldTransform(worldTransformAttack_);
};

// 描画処理
void Player::Draw() {
	// 無敵時間中はタイマーの値を使って点滅させる
	if (damageInvincibleTimer_ > 0.0f) {
		if (static_cast<int>(damageInvincibleTimer_ * 20.0f) % 2 == 0) {
			return;
		}
	}

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);

	// 攻撃時にのみエフェクトを描画
	if (isAttack_) {
		modelAttack_->Draw(worldTransformAttack_, *camera_);
	}
};
#pragma endregion

// 振る舞い切り替え
void Player::RequestState(std::unique_ptr<PlayerState> state) {
	requestedState_ = std::move(state);
}

// 振る舞いの適応
void Player::ApplyRequestedState() {
	if (!requestedState_) {
		return;
	}

	state_ = std::move(requestedState_);
	state_->Initialize(*this);
}

// マップとの衝突判定
void Player::CheckMapCollision(CollisionMapInfo &info) {
	collisionMap_.CheckMap(
		info, worldTransform_.translation_,
		kWidth, kHeight
	);
}

// プレイヤーの指定した角の座標を取得
Vector3 Player::CornerPosition(const Vector3 &center, Corner corner) const {
	switch (corner) {
	case kRightBottom:
		return { center.x + kWidth / 2.0f,
				center.y - kHeight / 2.0f,
				center.z };

	case kLeftBottom:
		return { center.x - kWidth / 2.0f,
				center.y - kHeight / 2.0f,
				center.z };

	case kRightTop:
		return { center.x + kWidth / 2.0f,
				center.y + kHeight / 2.0f,
				center.z };

	case kLeftTop:
		return { center.x - kWidth / 2.0f,
				center.y + kHeight / 2.0f,
				center.z };

	default:
		return center;
	}
}

void Player::DamageAnimation() {
	if (isDamage_) {
		// ダメージ演出中
		if (damageAnimationTimer_ > 0.0f) {
			// 進行度を 0.0f ～ 1.0f に変換
			float damageT =
				1.0f - damageAnimationTimer_ / kDamageAnimationTime;

			// 0 → 1 → 0 と変化する波形
			float pulse = std::sin(
				damageT * std::numbers::pi_v<float>
			);

			// Z軸を回転させて、ダメージを受けたように傾ける
			worldTransform_.rotation_.z = pulse * 0.35f;

			// 縮んでから元の大きさに戻す
			float scale = 1.0f - pulse * 0.2f;
			worldTransform_.scale_ = { scale, scale, 1.0f };
		} else {
			// 演出終了後は元の状態に戻す
			worldTransform_.rotation_.z = 0.0f;
			worldTransform_.scale_ = { 1.0f, 1.0f, 1.0f };
			isDamage_ = false;
		}
	}
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
				attackDirection_ = LRDirection::kRight;

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力時は急ブレーキ
					velocity_.x *= (1.0f - kTurnBrake);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = std::numbers::pi_v<float> *3.0f / 2.0f;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				attackDirection_ = LRDirection::kLeft;

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
		if (Input::GetInstance()->TriggerKey(DIK_UP)) {
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
				attackDirection_ = newDirection;
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

// 判定結果を反映して移動させる
void Player::MoveOnResult(const CollisionMapInfo &info) {
	worldTransform_.translation_.x += info.move.x;
	worldTransform_.translation_.y += info.move.y;
};

// 天井に接触している場合の処理
void Player::IsHitCeilingChecker(const CollisionMapInfo &info) {
	// 天井に当たったか？
	if (info.ceiling) {
		velocity_.y = 0;
	}
}

// 壁に接触している場合の処理
void Player::IsHitWallChecker(const CollisionMapInfo &info) {
	if (info.wall) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
};

// 接地状態の切り替え
void Player::InstallationStateSwitching(CollisionMapInfo &info) {
	// 接地状態
	if (onGround_) {
		// 上方向へ移動しようとしている場合は空中状態へ移行
		if (velocity_.y > 0.0f) {
			onGround_ = false;
			return;
		}

		// 移動後のプレイヤーの中心座標
		Vector3 position = {
			worldTransform_.translation_.x + info.move.x,
			worldTransform_.translation_.y + info.move.y,
			worldTransform_.translation_.z + info.move.z
		};

		// 移動後の足元を確認
		Vector3 leftBottom = CornerPosition(position, kLeftBottom);
		Vector3 rightBottom = CornerPosition(position, kRightBottom);

		// 足元がブロックに接しているか判定
		leftBottom.y -= kBlank;
		rightBottom.y -= kBlank;

		auto IsBlockAt = [this](const Vector3 &point) {
			MapChipField::IndexSet indexSet =
				mapChipField_->GetMapChipIndexSetByPosition(point);

			return mapChipField_->GetMapChipTypeByIndex(
				indexSet.xIndex, indexSet.yIndex
			) == MapChipType::kBlock;
			};

		bool hitGround = IsBlockAt(leftBottom) || IsBlockAt(rightBottom);

		// 足元にブロックがなければ落下開始
		if (!hitGround) {
			onGround_ = false;
		}
	}
	// 空中状態
	else if (info.landing) {
		// 着地状態へ移行
		onGround_ = true;

		// 着地時にX速度を減衰
		velocity_.x *= (1.0f - kAttenuationLanding);

		// Y速度をリセット
		velocity_.y = 0.0f;
	}
}

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

void Player::OnCollision(const Enemy *enemy) {
	(void)enemy;

	if (isAttack_ || isInvincible_ ||
		damageInvincibleTimer_ > 0.0f || isDead_) {
		return;
	}

	RequestState(std::make_unique<KnockBackState>());

	// HPを減らす
	hp_--;

	// ダメージ演出を開始
	damageAnimationTimer_ = kDamageAnimationTime;

	// フラグを立ててダメージ後の無敵時間を設定
	isDamage_ = true;
	damageInvincibleTimer_ = kDamageInvincibleTime;

	// HPが0以下なら死亡
	if (hp_ <= 0) {
		hp_ = 0;
		isDead_ = true;
	}
};

void Player::OnCollision(const ShieldEnemy *shieldEnemy) {
	(void)shieldEnemy;

	if (isAttack_ || isInvincible_ ||
		damageInvincibleTimer_ > 0.0f || isDead_) {
		return;
	}

	RequestState(std::make_unique<KnockBackState>());

	// HPを減らす
	hp_--;

	// ダメージ演出を開始
	damageAnimationTimer_ = kDamageAnimationTime;

	// フラグを立ててダメージ後の無敵時間を設定
	isDamage_ = true;
	damageInvincibleTimer_ = kDamageInvincibleTime;

	// HPが0以下なら死亡
	if (hp_ <= 0) {
		hp_ = 0;
		isDead_ = true;
	}
};
#pragma endregion

// 画面端と壁に挟まれたかどうかの判定
void Player::CheckScreenAndWallSandwich(const CollisionMapInfo &info, float minX, float maxX) {
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