#pragma once

#include "Enemy.h"

class NormalEnemy : public Enemy {
public:
	NormalEnemy() = default;
	~NormalEnemy() override = default;

	// 初期化
	void Initialize(
		KamataEngine::Model *model,
		KamataEngine::Camera *camera,
		const KamataEngine::Vector3 &position
	);

	// 更新
	void Update();

	// 衝突応答
	void OnCollision(Player *player);
};