#pragma once
#include "KamataEngine.h"
#include "Scene.h"

#include "Manager/EffectManager.h"

#include "Effect/Fade.h"
#include "Effect/Particle/DeathParticles.h"
#include "Effect/Particle/AttackParticles.h"

#include "Object/Player/Player.h"
#include "Object/Enemy/Enemy.h"
#include "Object/Enemy/ShieldEnemy.h"
#include "Object/Goal/Goal.h"
#include "Object/Skydome/Skydome.h"

#include "Game/Map/MapChipField.h"
#include "Manager/CameraManager.h"

#include <vector>

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

	// 終了フラグのgetter
	bool IsFinished() const override { return finished_; };

	// リロード要求フラグのgetter
	bool hasReloadRequested() const { return reloadRequested_; };

	EffectManager *GetEffectManager() const {
		return effectManager_;
	}

	bool IsCleared() const { return isCleared_; }

private:
	// ステージマネージャ参照用のポインタ
	StageManager *stageManager_ = nullptr;

	// カメラ
	CameraManager *cameraManager_ = nullptr;

	// 3Dモデルデータ
	KamataEngine::Model *modelBlock_ = nullptr;

	// 可変個配列
	std::vector<std::vector<KamataEngine::WorldTransform *>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

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

	// ゴール
	Goal *goal_ = nullptr;
	KamataEngine::Model *modelGoal_ = nullptr;

	// スカイドーム
	Skydome *skydome_ = nullptr;
	KamataEngine::Model *modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField *mapChipField_ = nullptr;

	// デス演出
	DeathParticles *deathParticles_ = nullptr;
	KamataEngine::Model *modelDeathParticles_ = nullptr;

	// 攻撃ダッシュパーティクル
	std::vector<AttackParticles *> attackParticles_;
	KamataEngine::Model *modelAttackParticles_ = nullptr;

	// パーティクル生成間隔
	float attackParticleEmitTimer_ = 0.0f;
	static constexpr float kAttackParticleEmitInterval = 0.06f;

	// フェード
	Fade *fade_ = nullptr;
	float duration_ = 1.0f;

	// エフェクトマネージャ
	EffectManager *effectManager_ = nullptr;

	// エフェクトモデル
	KamataEngine::Model *modelHitEffect_ = nullptr;
	KamataEngine::Model *modelGuardEffect_ = nullptr;

	// 終了フラグ
	bool finished_ = false;

	// リロード要求フラグ
	bool reloadRequested_ = false;

	// ゴール到達フラグ
	bool isCleared_ = false;

};