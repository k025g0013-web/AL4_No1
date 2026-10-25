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
    if (!currentScene_) {
        return;
    }

    // フェード中もシーンの処理を進める
    currentScene_->Update();

    // フェード更新
    fade_.Update();

    switch (transitionState_) {
    case TransitionState::None:
        // シーン終了時にフェードアウト開始
        if (currentScene_->IsFinished()) {
            fade_.Start(Fade::Status::FadeOut, fadeDuration_);
            transitionState_ = TransitionState::FadeOut;
        }
        break;

    case TransitionState::FadeOut:
        if (fade_.IsFinished()) {
            // フェードアウト完了後にシーン切り替え
            ChangeScene();

            // 新しいシーンのフェードイン開始
            fade_.Start(Fade::Status::FadeIn, fadeDuration_);
            transitionState_ = TransitionState::FadeIn;
        }
        break;

    case TransitionState::FadeIn:
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

    // ゲームシーンからのポーズメニュー要求を確認
    if (scene_ == SceneType::kGame) {
        auto *gameScene = dynamic_cast<GameScene *>(currentScene_);

        if (gameScene != nullptr) {
            const auto request = gameScene->GetPauseRequest();

            // リセット
            if (request == GameScene::PauseRequest::Reset) {
                delete currentScene_;
                currentScene_ = nullptr;

                auto *newGameScene = new GameScene();
                newGameScene->Initialize(stageManager_);
                currentScene_ = newGameScene;

                // シーンの種類はゲームのまま
                scene_ = SceneType::kGame;
                return;
            }

            // タイトルへ戻る
            if (request == GameScene::PauseRequest::ReturnToTitle) {
                delete currentScene_;
                currentScene_ = nullptr;

                auto *titleScene = new TitleScene();
                titleScene->Initialize(this);
                currentScene_ = titleScene;

                scene_ = SceneType::kTitle;
                return;
            }
        }
    }

    // ゲームクリア判定
    bool isCleared = false;

    if (scene_ == SceneType::kGame) {
        auto *gameScene = dynamic_cast<GameScene *>(currentScene_);

        if (gameScene != nullptr) {
            isCleared = gameScene->IsCleared();
        }
    }

    // ゲームオーバーメニューの選択項目
    int gameOverSelection = 1;

    if (scene_ == SceneType::kGameOver) {
        auto *gameOverScene =
            dynamic_cast<GameOverScene *>(currentScene_);

        if (gameOverScene != nullptr) {
            gameOverSelection = gameOverScene->GetSelectedItem();
        }
    }

    // 現在のシーンを破棄
    delete currentScene_;
    currentScene_ = nullptr;

    // 次のシーンを生成
    switch (scene_) {
    case SceneType::kTitle: {
        scene_ = SceneType::kGame;

        auto *gameScene = new GameScene();
        gameScene->Initialize(stageManager_);
        currentScene_ = gameScene;
        break;
    }

    case SceneType::kGame: {
        if (isCleared) {
            scene_ = SceneType::kClear;

            auto *clearScene = new ClearScene();
            clearScene->Initialize();
            currentScene_ = clearScene;
        } else {
            scene_ = SceneType::kGameOver;

            auto *gameOverScene = new GameOverScene();
            gameOverScene->Initialize();
            currentScene_ = gameOverScene;
        }
        break;
    }

    case SceneType::kGameOver: {
        if (gameOverSelection == 0) {
            // ゲームを再スタート
            scene_ = SceneType::kGame;

            auto *gameScene = new GameScene();
            gameScene->Initialize(stageManager_);
            currentScene_ = gameScene;
        } else {
            // タイトルへ戻る
            scene_ = SceneType::kTitle;

            auto *titleScene = new TitleScene();
            titleScene->Initialize(this);
            currentScene_ = titleScene;
        }
        break;
    }

    case SceneType::kClear: {
        // クリア後はタイトルへ戻る
        scene_ = SceneType::kTitle;

        auto *titleScene = new TitleScene();
        titleScene->Initialize(this);
        currentScene_ = titleScene;
        break;
    }
    }
}