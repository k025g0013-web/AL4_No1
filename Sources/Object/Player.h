#pragma once
#include "Math/AABB.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "LRDirection.h"
#include "CollisionMapInfo.h"
#include <3d\Model.h>

class Enemy;
class ShieldEnemy;
class MapChipField;

class Player {
public:
	enum class Behavior {
		kUnknown,
		kRoot,
		kAttack,
		kKnockBack,
	};

	enum class AttackPhase {
		kCharge,
		kMove,
		kCoolTime,
	};

public:
	Player();  // コンストラクタ
	~Player(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Model* modelAttack, KamataEngine::Camera* camera, const KamataEngine::Vector3& position); // 初期化
	void Update();                                                                                                                                      // 更新
	void Draw();                                                                                                                                        // 描画

	// 通常行動
	void BehaviorRootInitialize();
	void BehaviorRootUpdate();

	// 攻撃行動
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	// ノックバック行動
	void BehaviorKnockBackInitialize();
	void BehaviorKnockBackUpdate();

	void MoveInput();                                       // 移動入力
	void MoveOnResult(const CollisionMapInfo& info);        // 判定結果を反映して移動させる
	void IsHitCeilingChecker(const CollisionMapInfo& info); // 天井に接触している場合の処理
	void IsHitWallChecker(const CollisionMapInfo& info);    // 壁に接触している場合の処理

	// ワールド変換データを取得
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 速度を取得
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	// マップチップの配置データを取得
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// マップ衝突判定
	void CollisionMap(CollisionMapInfo& info);

	void CollisionMapTop(CollisionMapInfo& info);
	void CollisionMapBottom(CollisionMapInfo& info);
	void CollisionMapRight(CollisionMapInfo& info);
	void CollisionMapLeft(CollisionMapInfo& info);

	// 設置状態の切り替え処理
	void InstallationStateSwitching(CollisionMapInfo& info);

	// 指定した角の座標計算
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition() const;

	// 衝突応答
	void OnCollision(const Enemy* enemy);
	void OnCollision(const ShieldEnemy* shieldEnemy);

	// 【追加】画面端と壁の挟まれ判定
	void CheckScreenAndWallSandwich(const CollisionMapInfo& info, float minX, float maxX);

	// デスフラグのgetter
	bool IsDead() const { return isDead_; };

	// 攻撃状態チェック
	bool IsAttack() const { return behavior_ == Behavior::kAttack; };

	// 向き取得
	LRDirection GetDirection() const { return lrDirection_; }

	void RequestKnockBack();

private:
	// 振る舞い
	Behavior behavior_ = Behavior::kRoot;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// 攻撃行動
	uint32_t attackParameter_ = 0;
	AttackPhase attackPhase_ = AttackPhase::kCharge;
	float chargeTime_ = 7.0f;
	float attackTime_ = 10.0f;
	float coolTime_ = 5.0f;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransformAttack_;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* modelAttack_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 慣性
	static inline const float kAcceleration = 0.05f;

	// 速度減衰
	static inline const float kAttenuation = 0.2f;

	// 最大速度制限
	static inline const float kLimitRunSpeed = 2.0f;

	// 左右
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度 (下方向)
	static inline const float kGravityAcceleration = 0.05f;
	// 最大落下速度 (下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速 (上方向)
	static inline const float kJumpAcceleration = 1.0f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 1.9f;
	static inline const float kHeight = 1.9f;

	// 微小な余白
	static inline const float kBlank = 0.0001f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;

	// 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.75f;

	// デスフラグ
	bool isDead_ = false;

	// ノックバック時間
	uint32_t knockBackTimer_ = 0;
	static inline const uint32_t kKnockBackTime = 10;

	// ノックバックフラグ
	bool isInvincible_ = false;

	// ノックバック速度
	KamataEngine::Vector3 knockBackVelocity_ = {};
};