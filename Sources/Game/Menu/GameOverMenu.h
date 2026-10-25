#pragma once

#include "Menu.h"

class GameOverMenu final : public Menu {
public:
	GameOverMenu() = default;
	~GameOverMenu() override = default;

	
	/// 初期化
	void Initialize() override;

	/// 更新
	void Update() override;

	/// 描画
	void Draw() override;


	// 選択されたか否か
	bool IsDecided() const { return isDecided_; }

	// 何が選択されているか
	int GetSelectedItem() const { return selectedItem_; }

private:
	// テクスチャハンドル
	uint32_t graphHandleContinueText_ = 0;
	uint32_t graphHandleBackTitleText_ = 0;

	// スプライト
	KamataEngine::Sprite *spriteContinueText_ = nullptr;
	KamataEngine::Sprite *spriteBackTitleText_ = nullptr;

	// 選択されている項目
	int selectedItem_ = 0;

	// 選択の合否フラグ
	bool isDecided_ = false;

	// 選択項目の色
	KamataEngine::Vector4 startColor;
	KamataEngine::Vector4 exitColor;


	// 移動元の座標
	KamataEngine::Vector2 startPositionContinue_;
	KamataEngine::Vector2 startPositionBackTitle_;

	// 移動先の座標
	KamataEngine::Vector2 targetPositionContinue_;
	KamataEngine::Vector2 targetPositionBackTitle_;

	// 揺れを適用する前の座標
	KamataEngine::Vector2 basePositionContinue_;
	KamataEngine::Vector2 basePositionBackTitle_;

	// イージング用
	float moveTimer_ = 0.0f;
	float moveDuration_ = 0.2f;

	// Sine波による揺れ
	float sineTimer_ = 0.0f;

	// 揺れの速さ
	float sineSpeed_ = 4.0f;

	// 揺れの幅
	float sineAmplitude_ = 5.0f;
};