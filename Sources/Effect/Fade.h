#pragma once
#include "KamataEngine.h"

class Fade {
public:
	enum class Status { 
		None, 
		FadeIn,
		FadeOut,
	};

public:
	Fade();
	~Fade();

	void Initialize();
	void Update();
	void Draw();

	// フェード
	void Start(Status status, float duration);
	void Stop();

	// 終了判定
	bool IsFinished() const;

private:
	// テクスチャ
	uint32_t textureHandle_ = 0; 
	KamataEngine::Sprite* sprite_ = nullptr;

	// 状態
	Status status_ = Status::None;

	// フェード
	float duration_ = 0.0f;	// 持続時間
	float counter_ = 0.0f;	// 経過時間
};