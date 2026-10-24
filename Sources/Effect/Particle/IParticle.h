#pragma once
#include "KamataEngine.h"

#include <array>
#include <numbers>
#include <cstdint>

class IParticle {
public:
	virtual ~IParticle() = default; 

	// 初期化
	virtual void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	// 生成
	virtual void Emit(const KamataEngine::Vector3& position) = 0;
	
	// 更新
	virtual void Update() = 0;
	
	// 描画
	virtual void Draw();

	bool IsFinished() const { return finished_; };

protected:
	// パーティクルのワールド変換
	static inline const uint32_t kNumParticles = 8;
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransform_;

	// モデル・カメラ・トランスフォーム
	KamataEngine::Model *model_ = nullptr;
	KamataEngine::Camera *camera_ = nullptr;

	// 寿命と移動速度
	static inline const float kDuration = 2.0f;
	static inline const float kSpeed = 0.1f;
	static inline const float kAngleUnit = 2.0f * std::numbers::pi_v<float> / kNumParticles;

	// 終了フラグと経過時間
	bool finished_ = true;
	float counter_ = 0.0f;

	// 色
	KamataEngine::ObjectColor objectColor_;
	KamataEngine::Vector4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };
};