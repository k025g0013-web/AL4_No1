#pragma once

#include "stageManager.h"
#include "Effect/Fade.h"

class Scene;

class SceneManager {
public:
	enum class SceneType {
		kTitle,
		kGame,
		kGameOver,
		kClear,
	};

	enum class TransitionState {
		None,
		FadeOut,
		FadeIn
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

	Fade fade_;
	TransitionState transitionState_ = TransitionState::None;

	float fadeDuration_ = 0.5f;
};