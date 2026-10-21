#include "ClearScene.h"
#include "KamataEngine.h"

using namespace KamataEngine;

ClearScene::ClearScene() {}

ClearScene::~ClearScene() {
    delete fade_;
    fade_ = nullptr;
}

void ClearScene::Initialize() {
    finished_ = false;
    isFadeOutStarted_ = false;

    fade_ = new Fade();
    fade_->Initialize();
    fade_->Start(Fade::Status::FadeIn, duration_);
}

void ClearScene::Update() {
    if (!fade_) {
        return;
    }

    fade_->Update();

    // フェードイン終了後、Spaceキーでフェードアウト開始
    if (!isFadeOutStarted_) {
        if (fade_->IsFinished() &&
            Input::GetInstance()->TriggerKey(DIK_SPACE)) {
            isFadeOutStarted_ = true;
            fade_->Start(Fade::Status::FadeOut, duration_);
        }
    }
    // フェードアウト完了でシーン遷移
    else if (fade_->IsFinished()) {
        finished_ = true;
    }
}

void ClearScene::Draw() {
    if (fade_) {
        fade_->Draw();
    }
}