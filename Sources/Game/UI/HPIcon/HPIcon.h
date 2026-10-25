#pragma once

#include <KamataEngine.h>
#include <vector>

class Player;

class HpIcon {
public:
    HpIcon() = default;
    ~HpIcon();

    void Initialize(Player *player);
    void Update();
    void Draw();

private:
    Player *player_ = nullptr;

    uint32_t textureHandle_ = 0;
    std::vector<KamataEngine::Sprite *> sprites_;

    int currentHp_ = 0;

    // アイコンの表示位置
    KamataEngine::Vector2 startPosition_ = { 30.0f, 30.0f };

    // アイコン同士の間隔
    float iconSpacing_ = 60.0f;
};