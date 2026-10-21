#pragma once

class Player;

class PlayerState {
public:
    // デストラクタ
    virtual ~PlayerState() = default;

    // 初期化・更新
    virtual void Initialize(Player &player) = 0;
    virtual void Update(Player &player) = 0;
};