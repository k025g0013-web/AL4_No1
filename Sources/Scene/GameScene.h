#pragma once
#include "KamataEngine.h"
#include <vector>

#include "Scene.h"

#include "Effect/DeathParticles.h"
#include "Effect/Fade.h"
#include "Effect/HitEffect.h"
#include "Effect/GuardEffect.h"
#include "Effect/Skydome.h"

#include "Object/Player.h"
#include "Object/Enemy/Enemy.h"
#include "Object/Enemy/ShieldEnemy.h"

#include "MapChipField.h"
#include "CameraController.h"

class StageManager;

class GameScene final : public Scene {
public:
	GameScene();  // コンストラクタ
	~GameScene() override; // デストラクタ

	void Initialize(StageManager *stageManager); // 初期化
	void Update()override;     // 更新
	void Draw()override;       // 描画

	// 表示ブロックの生成
	void GenerateBlocks();

	// カメラとブロック行列の更新
	void UpdateCameraAndTransforms();

	// 全ての当たり判定を行う
	void CheckAllCollision();

	// ゲームのフェーズ(型)
	enum class Phase {
		kFadeIn,	// フェードイン
		kPlay,		// ゲームプレイ
		kDeath,		// デス演出
		kFadeOut,	// フェードアウト
	};

	// ゲームの現在フェーズ(変数)
	Phase phase_ = Phase::kFadeIn;

	// フェーズの切り替え
	void ChangePhase();

	// ヒットエフェクトを生成
	void CreateHitEffect(const KamataEngine::Vector3 &position);

	// ガードエフェクトを生成
	void CreateGuardEffect(const KamataEngine::Vector3 &position);

	// 終了フラグのgetter
	bool IsFinished() const override { return finished_; };

	// リロード要求フラグのgetter
	bool hasReloadRequested() const { return reloadRequested_; };

private:
	// ステージマネージャ参照用のポインタ
	StageManager *stageManager_ = nullptr;

	// カメラ
	KamataEngine::Camera camera_;

	// 3Dモデルデータ
	KamataEngine::Model *modelBlock_ = nullptr;

	// 可変個配列
	std::vector<std::vector<KamataEngine::WorldTransform *>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera *debugCamera_ = nullptr;

	// プレイヤー
	Player *player_ = nullptr;
	KamataEngine::Model *modelPlayer_ = nullptr;

	Player *playerAttack_ = nullptr;
	KamataEngine::Model *modelPlayerAttack_ = nullptr;

	// エネミー
	std::list<Enemy *> enemies_;
	KamataEngine::Model *modelEnemy_ = nullptr;

	// 盾エネミー
	std::list<ShieldEnemy *> shieldEnemies_;
	KamataEngine::Model *modelShieldEnemy_ = nullptr;

	// スカイドーム
	Skydome *skydome_ = nullptr;
	KamataEngine::Model *modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField *mapChipField_ = nullptr;

	// カメラコントローラー
	CameraController *cameraController_ = nullptr;

	// デス演出
	DeathParticles *deathParticles_ = nullptr;
	KamataEngine::Model *modelDeathParticles_ = nullptr;

	// フェード
	Fade *fade_ = nullptr;
	float duration_ = 1.0f;

	// ヒットエフェクト
	std::list<HitEffect *> hitEffects_;
	KamataEngine::Model *modelHitEffect_ = nullptr;

	// ガードエフェクト
	std::list<GuardEffect *> guardEffects_;
	KamataEngine::Model *modelGuardEffect_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	// リロード要求フラグ
	bool reloadRequested_ = false;
};