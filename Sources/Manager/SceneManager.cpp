#include "SceneManager.h"

#include "Scene/Scene.h"
#include "Scene/TitleScene.h"
#include "Scene/GameScene.h"
#include "Scene/GameOverScene.h"
#include "Scene/ClearScene.h"

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

    if (!currentScene_->IsFinished()) {
        return;
    }

    // ゲームシーンがクリアされたかを取得する
    bool isCleared = false;

    if (scene_ == SceneType::kGame) {
        GameScene *gameScene = dynamic_cast<GameScene *>(currentScene_);

        if (gameScene != nullptr) {
            isCleared = gameScene->IsCleared();
        }
    }

    // 現在のシーンを破棄
    delete currentScene_;
    currentScene_ = nullptr;

    // 次のシーンへ遷移
    switch (scene_) {
    case SceneType::kTitle: {
        scene_ = SceneType::kGame;

        GameScene *gameScene = new GameScene();
        gameScene->Initialize(stageManager_);
        currentScene_ = gameScene;
        break;
    }

    case SceneType::kGame: {
        if (isCleared) {
            // クリアしていたらクリア画面へ
            scene_ = SceneType::kClear;

            ClearScene *clearScene = new ClearScene();
            clearScene->Initialize();
            currentScene_ = clearScene;
        } else {
            // クリアしていなければゲームオーバー画面へ
            scene_ = SceneType::kGameOver;

            GameOverScene *gameOverScene = new GameOverScene();
            gameOverScene->Initialize();
            currentScene_ = gameOverScene;
        }
        break;
    }

    case SceneType::kGameOver: {
        // ゲームオーバー画面からタイトルへ
        scene_ = SceneType::kTitle;

        TitleScene *titleScene = new TitleScene();
        titleScene->Initialize(this);
        currentScene_ = titleScene;
        break;
    }

    case SceneType::kClear: {
        // クリア画面からタイトルへ
        scene_ = SceneType::kTitle;

        TitleScene *titleScene = new TitleScene();
        titleScene->Initialize(this);
        currentScene_ = titleScene;
        break;
    }
    }
}