#pragma once
#include "Scene.h"

#include "Manager/SceneManager.h"
#include "Manager/CameraManager.h"
#include "Game/Menu/TitleMenu.h"
#include "Object/Skydome/Skydome.h"
#include "Object/Player/Player.h"


class TitleScene final : public Scene {
public:
	TitleScene() = default;
	~TitleScene() override;

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

	// カメラ
	CameraManager *cameraManager_ = nullptr;

	// テクスチャハンドル
	uint32_t graphHandleTitleLogo_ = 0;

	// スプライト
	KamataEngine::Sprite *spriteTitleLogo_ = nullptr;

	// タイトルメニュー
	TitleMenu titleMenu_;

	// スカイドーム
	Skydome *skydome_ = nullptr;
	KamataEngine::Model *modelSkydome_ = nullptr;

	// プレイヤー
	KamataEngine::Model *modelPlayer_ = nullptr;
	KamataEngine::WorldTransform playerWorldTransform_;

	// シーンマネージャー
	SceneManager *sceneManager_ = nullptr;
};