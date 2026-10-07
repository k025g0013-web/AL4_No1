#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>

class Skydome {
public:
	Skydome();  // コンストラクタ
	~Skydome(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera); // 初期化
	void Update();											                   // 更新
	void Draw();                                                               // 描画

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
};