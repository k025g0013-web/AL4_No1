#pragma once

#include "stageManager.h"

class Scene;

class SceneManager {
public:
	enum class SceneType {
		kTitle,
		kGame,
		kResult,
	};


	SceneManager();
	~SceneManager();


	void Initialize(StageManager *stageManager);
	void Update();
	void Draw();


	// ゲーム終了リクエスト
	void RequestGameEnd() { isGameEnd_ = true; };

	// ゲームを終了するか
	bool IsGameEnd() const { return isGameEnd_; }

private:
	// ゲーム終了フラグ
	bool isGameEnd_ = false;

	// シーン切り替え
	void ChangeScene();

	// シーン
	Scene* currentScene_ = nullptr;
	SceneType scene_ = SceneType::kTitle;

	StageManager *stageManager_;
};