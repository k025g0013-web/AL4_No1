#pragma once

#include "KamataEngine.h"
#include "Effect/Effect.h"

class GuardEffect : public Effect {
public:
	enum class Behavior {
		kUnknown,
		kSpread,
		kFade,
		kDead,
	};

	GuardEffect();
	~GuardEffect() override;

	void Initialize(const KamataEngine::Vector3 &position);
	void Update() override;
	void Draw() override;

	static void SetModel(KamataEngine::Model *model) {
		model_ = model;
	}

	static void SetCamera(KamataEngine::Camera *camera) {
		camera_ = camera;
	}

	static GuardEffect *Create(const KamataEngine::Vector3 &position);

	void BehaviorSpreadInitialize();
	void BehaviorSpreadUpdate();
	void BehaviorFadeInitialize();
	void BehaviorFadeUpdate();

	bool IsDead() const override {
		return isDead_;
	}

private:
	static KamataEngine::Model *model_;
	static KamataEngine::Camera *camera_;

	KamataEngine::WorldTransform circleWorldTransform_;

	Behavior behavior_ = Behavior::kSpread;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	float counter_ = 0.0f;
	float spreadDuration_ = 8.0f / 60.0f;
	float fadeDuration_ = 12.0f / 60.0f;
	float alpha_ = 1.0f;

	bool isDead_ = false;
};