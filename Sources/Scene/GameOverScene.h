#pragma once

#include "Scene.h"
#include "Manager/CameraManager.h"
#include "Game/UI/Menu/GameOverMenu.h"
#include "Object/Skydome/Skydome.h"

class GameOverScene final : public Scene {
public:
	GameOverScene() = default;
	~GameOverScene() override;

	/// 初期化
	void Initialize();

	/// 更新
	void Update() override;

	/// 描画
	void Draw() override;

	// シーン終了
	bool IsFinished() const override;

	int GetSelectedItem() const {
		return gameOverMenu_.GetSelectedItem();
	}

private:
	// シーン終了フラグ
	bool isFinished_ = false;

	// カメラ
	CameraManager *cameraManager_ = nullptr;

	// タイトルロゴ
	uint32_t graphHandleGameOverText_ = 0;
	KamataEngine::Sprite *spriteGameOver_ = nullptr;

	// ゲームオートメニュー
	GameOverMenu gameOverMenu_;

	// スカイドーム
	Skydome *skydome_ = nullptr;
	KamataEngine::Model *modelSkydome_ = nullptr;

	// 敵
	KamataEngine::Model *modelEnemy_ = nullptr;
	KamataEngine::WorldTransform enemyWorldTransform_[2];
};