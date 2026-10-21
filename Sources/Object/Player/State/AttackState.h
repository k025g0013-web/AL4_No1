#pragma once
#include "PlayerState.h"

class AttackState : public PlayerState {
public:
	// 初期化・更新
	void Initialize(Player &player) override;
	void Update(Player &player) override;
};