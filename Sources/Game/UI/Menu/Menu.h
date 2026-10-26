#pragma once
#include "KamataEngine.h"

class Menu {
public:
	virtual ~Menu() = default;

	virtual void Initialize() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
    // 共通SEの初期化
    static void InitializeSE();

    // 選択音・決定音
    static void PlaySelectSE();
    static void PlayDecideSE();

private:
    // 全メニューで共有する音声データハンドル
    static uint32_t selectSEHandle_;
    static uint32_t decideSEHandle_;

    // 二重読み込み防止
    static bool isSEInitialized_;
};