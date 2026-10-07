#pragma once
#include "KamataEngine.h"
#include <array>
#include <cmath>
#include <numbers>

class DeathParticles {
public:
	DeathParticles();  // コンストラクタ
	~DeathParticles(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position); // 初期化
	void Update();                                                                                                    // 更新
	void Draw();                                                                                                      // 描画

	bool IsFinished() const { return finished_; };

private:
	// モデル
	KamataEngine::Model* model_ = nullptr;
	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	// ワールド変換データ
	std::array<KamataEngine::WorldTransform, kNumParticles> worldTransform_;

	// 存続時間 (消滅までの時間) <秒>
	static inline const float kDuration = 2.0f;
	// 移動の速さ
	static inline const float kSpeed = 0.1f;
	// 分裂した1個分の角度
	static inline const float kAngleUint = 2.0f * std::numbers::pi_v<float> / 8.0f;

	// 終了フラグ
	bool finished_ = false;
	// 経過時間カウント
	float counter_ = 0.0f;

	// 色変更オブジェクト
	KamataEngine::ObjectColor objectColor_;
	// 色の数値
	KamataEngine::Vector4 color_;
};