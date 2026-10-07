#include "SceneManager.h"

#include "Scene.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "ResultScene.h"

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {

	delete currentScene_;
	currentScene_ = nullptr;
}

void SceneManager::Initialize(StageManager* stageManager) {
	// 最初のシーン
	scene_ = SceneType::kTitle;

	TitleScene* titleScene = new TitleScene();
	titleScene->Initialize(this);

	currentScene_ = titleScene;

	stageManager_ = stageManager;
}

void SceneManager::Update() {

	// シーンの切り替えを確認
	ChangeScene();

	// 現在のシーンを更新
	if (currentScene_) {
		currentScene_->Update();
	}
}

void SceneManager::Draw() {

	if (currentScene_) {
		currentScene_->Draw();
	}
}

void SceneManager::ChangeScene() {

	if (currentScene_ == nullptr) {
		return;
	}

	// シーン終了条件を確認
	if (!currentScene_->IsFinished()) {
		return;
	}

	// 現在のシーンを削除
	delete currentScene_;
	currentScene_ = nullptr;

	// 次のシーンを生成
	switch (scene_) {
	case SceneType::kTitle: {
		scene_ = SceneType::kGame;

		GameScene *gameScene = new GameScene();
		gameScene->Initialize(stageManager_);

		currentScene_ = gameScene;
		break;
	}
	case SceneType::kGame: {
		scene_ = SceneType::kResult;

		ResultScene* resultScene = new ResultScene();
		resultScene->Initialize();

		currentScene_ = resultScene;
		break;
	}
	case SceneType::kResult: {
		scene_ = SceneType::kTitle;

		TitleScene* titleScene = new TitleScene();
		titleScene->Initialize(this);

		currentScene_ = titleScene;
		break;
	}
	}
}