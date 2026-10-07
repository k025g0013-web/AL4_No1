#pragma once
#include "KamataEngine.h"

class GuardEffect {
public:
	enum class Behavior {
		kUnknown,
		kSpread,
		kFade,
		kDead,
	};

public:
	// ライフサイクル
	GuardEffect();
	~GuardEffect();

	// 基本関数
	void Initialize(const KamataEngine::Vector3& position);
	void Update();
	void Draw();

	// setter
	static void SetModel(KamataEngine::Model* model) { model_ = model; }
	static void SetCamera(KamataEngine::Camera* camera) { camera_ = camera; }

	// インスタンス生成・初期化
	static GuardEffect* Create(const KamataEngine::Vector3& position);

	void BehaviorSpreadInitialize();
	
	void BehaviorSpreadUpdate();

	void BehaviorFadeInitialize();
	
	void BehaviorFadeUpdate();

	// デスフラグの取得
	bool IsDead() const { return isDead_; }

private:
	// モデル
	static KamataEngine::Model* model_;
	static KamataEngine::Camera* camera_;

	// 円のWorldTransform
	KamataEngine::WorldTransform circleWorldTransform_;

	// 振る舞い
	Behavior behavior_ = Behavior::kSpread;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	float counter_ = 0.0f;

	float spreadDuration_ = 8.0f / 60.0f;
	float fadeDuration_ = 12.0f / 60.0f;

	float alpha_ = 1.0f;

	bool isDead_ = false;
};