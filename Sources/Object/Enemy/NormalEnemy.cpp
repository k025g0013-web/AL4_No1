#include "NormalEnemy.h"

void NormalEnemy::Initialize(
	KamataEngine::Model *model,
	KamataEngine::Camera *camera,
	const KamataEngine::Vector3 &position
) {
	Enemy::Initialize(model, camera, position);
}

void NormalEnemy::Update() {
	Enemy::Update();
}

void NormalEnemy::OnCollision(Player *player) {
	Enemy::OnCollision(player);
}