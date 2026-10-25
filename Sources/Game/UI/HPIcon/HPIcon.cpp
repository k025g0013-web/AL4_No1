#include "HpIcon.h"

#include "Object/Player/Player.h"

using namespace KamataEngine;

HpIcon::~HpIcon() {
    for (Sprite *sprite : sprites_) {
        delete sprite;
    }
    sprites_.clear();
}

void HpIcon::Initialize(Player *player) {
    player_ = player;

    // HPアイコンのテクスチャを読み込む
    textureHandle_ = TextureManager::Load("hpIcon.png");

    // 初期HPに合わせてアイコンを作成
    currentHp_ = player_->GetHP();

    for (int i = 0; i < currentHp_; ++i) {
        Sprite *sprite = Sprite::Create(
            textureHandle_,
            {
                startPosition_.x + iconSpacing_ * i,
                startPosition_.y
            }
        );

        sprites_.push_back(sprite);
    }
}

void HpIcon::Update() {
    if (player_ == nullptr) {
        return;
    }

    int hp = player_->GetHP();

    // HPに変化がなければ何もしない
    if (hp == currentHp_) {
        return;
    }

    // 既存のアイコンを削除
    for (Sprite *sprite : sprites_) {
        delete sprite;
    }
    sprites_.clear();

    // 現在のHP分だけアイコンを作成
    currentHp_ = hp;

    for (int i = 0; i < currentHp_; ++i) {
        Sprite *sprite = Sprite::Create(
            textureHandle_,
            {
                startPosition_.x + iconSpacing_ * i,
                startPosition_.y
            }
        );

        sprites_.push_back(sprite);
    }
}

void HpIcon::Draw() {
    for (Sprite *sprite : sprites_) {
        sprite->Draw();
    }
}