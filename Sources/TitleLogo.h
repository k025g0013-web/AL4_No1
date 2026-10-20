#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>

class TitleLogo {
public:
	TitleLogo();  // コンストラクタ
	~TitleLogo(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera); // 初期化
	void Update();                                                             // 更新
	void Draw();                                                               // 描画

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 上下移動用のタイマー
	float moveTimer_ = 0.0f;
};
