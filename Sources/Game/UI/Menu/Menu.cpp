#include "Menu.h"

#include "audio/Audio.h"

// 静的メンバーの定義
uint32_t Menu::selectSEHandle_ = 0;
uint32_t Menu::decideSEHandle_ = 0;
bool Menu::isSEInitialized_ = false;

void Menu::InitializeSE() {
    // すでに読み込み済みなら何もしない
    if (isSEInitialized_) {
        return;
    }

    // SEを読み込む
    selectSEHandle_ =
        KamataEngine::Audio::GetInstance()->LoadWave("seSelect.mp3");

    decideSEHandle_ =
        KamataEngine::Audio::GetInstance()->LoadWave("seDecide.mp3");

    isSEInitialized_ = true;
}

void Menu::PlaySelectSE() {
    if (!isSEInitialized_) {
        InitializeSE();
    }

    KamataEngine::Audio::GetInstance()->PlayWave(
        selectSEHandle_, false, 0.7f);
}

void Menu::PlayDecideSE() {
    if (!isSEInitialized_) {
        InitializeSE();
    }

    KamataEngine::Audio::GetInstance()->PlayWave(
        decideSEHandle_, false, 1.0f);
}