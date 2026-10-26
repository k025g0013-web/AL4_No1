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
	// シーン切り替え
	void ChangeScene();

	// BGMの読み込みと再生
	void InitializeBGM();
	void PlayBGM(SceneType sceneType);
	void StopBGM();

private:
	// ゲーム終了フラグ
	bool isGameEnd_ = false;

	// シーン
	Scene* currentScene_ = nullptr;
	SceneType scene_ = SceneType::kTitle;

	StageManager *stageManager_;

	Fade fade_;
	TransitionState transitionState_ = TransitionState::None;

	float fadeDuration_ = 0.5f;

	uint32_t bgmTitleHandle_ = 0;
	uint32_t bgmGameHandle_ = 0;
	uint32_t bgmGameOverHandle_ = 0;
	uint32_t bgmClearHandle_ = 0;

	uint32_t bgmVoiceHandle_ = 0;
	SceneType playingBGMScene_ = SceneType::kTitle;
	bool isBGMPlaying_ = false;
};