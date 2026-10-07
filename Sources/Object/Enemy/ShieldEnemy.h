#pragma once

#include "Enemy.h"

class ShieldEnemy : public Enemy {
public:
	ShieldEnemy() = default;
	~ShieldEnemy() override = default;

	// 初期化
	void Initialize(
		KamataEngine::Model *model,
		KamataEngine::Camera *camera,
		const KamataEngine::Vector3 &position
	) override;

	// 更新
	void Update() override;

	// 衝突応答
	void OnCollision(Player *player) override;

	// 衝突時にプレイヤーの攻撃が正面からか確認
	bool IsFrontAttack(const Player *player) const;

protected:
	// Guard動作
	void BehaviorGuardInitialize();
	void BehaviorGuardUpdate();

protected:
	// ガード用タイマー
	uint32_t guardTimer_ = 0;

	// 15フレームの間ガードモーション
	static inline const uint32_t kGuardTime = 15;

	// ガード中フラグ
	bool isGuarding_ = false;
};