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

    fade_.Initialize();
    fade_.Start(Fade::Status::FadeIn, fadeDuration_);
    transitionState_ = TransitionState::FadeIn;
}

void SceneManager::Update() {
    if (currentScene_ == nullptr) {
        return;
    }

    // シーン更新
    currentScene_->Update();

    // フェード更新
    fade_.Update();

    switch (transitionState_) {
    case TransitionState::None:
        // シーン終了を検知したらフェードアウト開始
        if (currentScene_->IsFinished()) {
            fade_.Start(Fade::Status::FadeOut, fadeDuration_);
            transitionState_ = TransitionState::FadeOut;
        }
        break;

    case TransitionState::FadeOut:
        // フェードアウト完了後にシーンを切り替える
        if (fade_.IsFinished()) {
            ChangeScene();

            // 新しいシーンを黒い画面から表示する
            fade_.Start(Fade::Status::FadeIn, fadeDuration_);
            transitionState_ = TransitionState::FadeIn;
        }
        break;

    case TransitionState::FadeIn:
        // フェードイン完了
        if (fade_.IsFinished()) {
            transitionState_ = TransitionState::None;
        }
        break;
    }
}

void SceneManager::Draw() {
    if (currentScene_) {
        currentScene_->Draw();
    }
    fade_.Draw();
}

void SceneManager::ChangeScene() {
    if (currentScene_ == nullptr) {
        return;
    }

    // ゲームシーンがクリアされたかを取得
    bool isCleared = false;

    if (scene_ == SceneType::kGame) {
        GameScene *gameScene =
            dynamic_cast<GameScene *>(currentScene_);

        if (gameScene != nullptr) {
            isCleared = gameScene->IsCleared();
        }
    }

    // 現在のシーンを破棄
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
        if (isCleared) {
            scene_ = SceneType::kClear;

            ClearScene *clearScene = new ClearScene();
            clearScene->Initialize();
            currentScene_ = clearScene;
        } else {
            scene_ = SceneType::kGameOver;

            GameOverScene *gameOverScene = new GameOverScene();
            gameOverScene->Initialize();
            currentScene_ = gameOverScene;
        }
        break;
    }

    case SceneType::kGameOver:
    case SceneType::kClear: {
        scene_ = SceneType::kTitle;

        TitleScene *titleScene = new TitleScene();
        titleScene->Initialize(this);
        currentScene_ = titleScene;
        break;
    }
    }
}