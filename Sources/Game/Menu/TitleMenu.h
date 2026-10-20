#pragma once

#include "Menu.h"

class TitleMenu final : public Menu {
public:
	TitleMenu() = default;
	~TitleMenu() override = default;

	
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
	uint32_t graphHandleStartText_ = 0;
	uint32_t graphHandleExitText_ = 0;

	// スプライト
	KamataEngine::Sprite *spriteStartText_ = nullptr;
	KamataEngine::Sprite *spriteExitText_ = nullptr;

	// 選択されている項目
	int selectedItem_ = 0;

	// 選択の合否フラグ
	bool isDecided_ = false;

	// 選択項目の色
	KamataEngine::Vector4 startColor;
	KamataEngine::Vector4 exitColor;

	// 移動元の座標
	KamataEngine::Vector2 startPositionStart_;
	KamataEngine::Vector2 startPositionExit_;

	// 移動先の座標
	KamataEngine::Vector2 targetPositionStart_;
	KamataEngine::Vector2 targetPositionExit_;

	// イージング用
	float moveTimer_ = 0.0f;
	float moveDuration_ = 0.2f;
};