
#pragma once

#include "Menu.h"

class PoseMenu final : public Menu {
public:
    PoseMenu() = default;
    ~PoseMenu() override = default;

    // 初期化
    void Initialize() override;

    // 更新
    void Update() override;

    // 描画
    void Draw() override;

    // 選択されたか否か
    bool IsDecided() const { return isDecided_; }

    // 選択されている項目
    int GetSelectedItem() const { return selectedItem_; }

private:
    // テクスチャハンドル
    uint32_t graphHandleResetText_ = 0;
    uint32_t graphHandleResumeText_ = 0;
    uint32_t graphHandleBackTitleText_ = 0;

    // スプライト
    KamataEngine::Sprite *spriteResetText_ = nullptr;
    KamataEngine::Sprite *spriteResumeText_ = nullptr;
    KamataEngine::Sprite *spriteBackTitleText_ = nullptr;

    // 選択項目
    int selectedItem_ = 1;

    // 決定フラグ
    bool isDecided_ = false;

    // 選択項目の色
    KamataEngine::Vector4 resetColor_;
    KamataEngine::Vector4 resumeColor_;
    KamataEngine::Vector4 titleColor_;

    // 移動元の座標
    KamataEngine::Vector2 startPositionReset_;
    KamataEngine::Vector2 startPositionResume_;
    KamataEngine::Vector2 startPositionBackTitle_;

    // 移動先の座標
    KamataEngine::Vector2 targetPositionReset_;
    KamataEngine::Vector2 targetPositionResume_;
    KamataEngine::Vector2 targetPositionBackTitle_;

    // 揺れを適用する前の座標
    KamataEngine::Vector2 basePositionReset_;
    KamataEngine::Vector2 basePositionResume_;
    KamataEngine::Vector2 basePositionBackTitle_;

    // イージング
    float moveTimer_ = 0.0f;
    float moveDuration_ = 0.2f;

    // Sine波による揺れ
    float sineTimer_ = 0.0f;
    float sineSpeed_ = 4.0f;
    float sineAmplitude_ = 5.0f;
};
