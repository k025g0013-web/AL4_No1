#pragma once

#include "Scene.h"
#include "Manager/CameraManager.h"
#include "Object/Skydome/Skydome.h"

class ClearScene final : public Scene {
public:
	ClearScene();
	~ClearScene() override;

	/// 初期化
	void Initialize();

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

	// ゲームクリア
	uint32_t graphHandleGameClearText_ = 0;
	KamataEngine::Sprite *spriteGameClearText_ = nullptr;

	// タイトルへもどる
	uint32_t graphHandleBackTitleText_ = 0;
	KamataEngine::Sprite *spriteBackTitleText_ = nullptr;
	float sineTimer_ = 0.0f;	// Sine波による揺れ
	float sineSpeed_ = 4.0f;	// 揺れの速さ
	float sineAmplitude_ = 5.0f;// 揺れの幅

	// スカイドーム
	Skydome *skydome_ = nullptr;
	KamataEngine::Model *modelSkydome_ = nullptr;
};