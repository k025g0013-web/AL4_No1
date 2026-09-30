#pragma once

#include "Scene.h"
#include "Scene/SceneManager.h"

#include "Menu/TitleMenu.h"

class TitleScene : public Scene {
public:
	TitleScene() = default;
	~TitleScene() override = default;


	/// 初期化
	void Initialize(SceneManager *sceneManager);
	
	/// 更新
	void Update() override;
	
	/// 描画
	void Draw() override;


	// シーン終了
	bool IsFinished() const override;

private:
	// シーン終了フラグ
	bool isFinished_ = false;

	// テクスチャハンドル
	uint32_t graphHandleTitleLogo_ = 0;

	// スプライト
	KamataEngine::Sprite *spriteTitleLogo_ = nullptr;

	// タイトルメニュー
	TitleMenu titleMenu_;

	// シーンマネージャー
	SceneManager *sceneManager_ = nullptr;
};