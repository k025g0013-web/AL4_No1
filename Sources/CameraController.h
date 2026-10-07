#pragma once
#include "KamataEngine.h"
#include "Object/Player.h"

struct Rect {
	float left = 0.0f;		// 左端
	float right = 1.0f;		// 右端
	float bottom = 0.0f;	// 上端
	float top = 1.0f;		// 下端
};

class Player;

class CameraController {
	enum class Mode {
		kFollow,		// プレイヤー追従
		kForcedScroll,	// 強制スクロール
	};

public:
	CameraController(); // コンストラクタ
	~CameraController(); // デストラクタ

	void Initialize(); // 初期化
	void Update();       // 更新

	// 外部からポインターをセット
	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	// カメラ移動範囲を取得
	void SetMovableArea(Rect area) { movableArea_ = area; }

	// 線形補間
	KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& start, const KamataEngine::Vector3& end, float t);

	// ワールドトランスフォームを取得
	const KamataEngine::Vector3& GetCameraTranslation() const { return camera_.translation_; }

	// カメラモード
	void SetMode(Mode mode) { mode_ = mode; }
	Mode GetMode() const { return mode_; }

private:
	// カメラ
	KamataEngine::Camera camera_;

	// モード
	Mode mode_ = Mode::kFollow;

	// 強制スクロール速度
	KamataEngine::Vector3 scrollSpeed_ = {0.05f, 0.0f, 0.0f};

	// 追尾対象
	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差（オフセット）
	KamataEngine::Vector3 targetOffset_ = {0, 0, -30.0f};

	// カメラ移動範囲
	Rect movableArea_ = {0, 100, 0, 100};

	// カメラの目標位置
	KamataEngine::Vector3 targetPosition_;

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f;

	// 速度掛け率
	static inline const float kVelocityBias = 15.5f;

	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin_ = {-25.0f, 25.0f, -25.0f, 25.0f};
};
