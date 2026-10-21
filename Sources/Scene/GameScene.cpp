#include "GameScene.h"
#include "Manager/StageManager.h"

#include "Math/UpdateWorldTransform.h"
#include "Game/Map/CollisionMap.h"

#include "Object/Enemy/NormalEnemy.h"
#include "Object/Enemy/ShieldEnemy.h"

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete modelBlock_;
	for (std::vector<WorldTransform *> &worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform *worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete cameraManager_;

	delete player_;
	delete playerAttack_;

	for (Enemy *enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (ShieldEnemy *shieldEnemy : shieldEnemies_) {
		delete shieldEnemy;
	}
	shieldEnemies_.clear();

	delete skydome_;

	if (deathParticles_ != nullptr) {
		delete deathParticles_;
	}

	delete mapChipField_;

	delete fade_;

	delete effectManager_;
	effectManager_ = nullptr;

	delete modelHitEffect_;
	delete modelGuardEffect_;

	delete goal_;
}

#pragma region 初期化・更新・描画
void GameScene::Initialize(StageManager *stageManager) {
	// 引数をメンバポインタに記録する
	stageManager_ = stageManager;

	// シーンの状態を初期化
	finished_ = false;
	isCleared_ = false;
	phase_ = Phase::kFadeIn;


	// カメラマネージャーを生成
	cameraManager_ = new CameraManager();
	cameraManager_->Initialize(nullptr);

	// 3Dモデルデータの生成
	modelBlock_ = Model::Create();
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelPlayerAttack_ = Model::CreateFromOBJ("attack", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelShieldEnemy_ = Model::CreateFromOBJ("shieldEnemy", true);
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelHitEffect_ = Model::CreateFromOBJ("hitEffect", true);
	modelGuardEffect_ = Model::CreateFromOBJ("guardEffect", true);
	modelGoal_ = Model::CreateFromOBJ("goal", true);

	effectManager_ = new EffectManager();
	effectManager_->Initialize(
		modelHitEffect_,
		modelGuardEffect_,
		&cameraManager_->GetCamera()
	);

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	const StageData &stageData = stageManager_->GetCurrentStageData();
	std::string stageFileName = "Resources/fields/" + stageData.name + ".csv";
	mapChipField_->LoadMapChipCsv(stageFileName);

	// CSVの配置を読み込んでブロック、プレイヤー、エネミーを生成・配置
	GenerateBlocks();

	// プレイヤー生成後に追従対象を設定
	cameraManager_->SetTarget(player_);
	cameraManager_->Reset();

	// スカイドーム
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &cameraManager_->GetCamera());

	// フェード
	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, duration_);
}

void GameScene::Update() {

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_Q)) {
		cameraManager_->SetDebugCameraActive(!cameraManager_->IsDebugCameraActive());
	}

#endif

#ifdef USE_IMGUI
	ImGui::Begin("stage");

	if (ImGui::Button("Reload")) {
		reloadRequested_ = true;
	}

	ImGui::End();
#endif

	// フェードの更新
	fade_->Update();

	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		// intentional fall-through (kFadeInの時もPlayと同じ更新処理に流す)
		[[fallthrough]];

	case Phase::kPlay:
		// スカイドーム・プレイヤー
		skydome_->Update();
		player_->Update();

		// ゴール更新
		goal_->Update();


		// Rキーでリセット（リロード）
		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			// キャラクター等の再配置を含めて再初期化
			for (Enemy *enemy : enemies_) {
				delete enemy;
			}
			enemies_.clear();

			for (ShieldEnemy *shieldEnemy : shieldEnemies_) {
				delete shieldEnemy;
			}
			shieldEnemies_.clear();

			Initialize(stageManager_);
		}

		// キャラクター・エフェクト更新
		for (Enemy *enemy : enemies_)
			enemy->Update();
		for (ShieldEnemy *shieldEnemy : shieldEnemies_)
			shieldEnemy->Update();

		effectManager_->Update();

		// カメラ・行列更新
		UpdateCameraAndTransforms();

		// 当たり判定とフェーズ遷移チェック
		CheckAllCollision();
		ChangePhase();
		break;

	case Phase::kDeath:
		// デス演出中の更新
		skydome_->Update();
		for (Enemy *enemy : enemies_)
			enemy->Update();
		for (ShieldEnemy *shieldEnemy : shieldEnemies_)
			shieldEnemy->Update();

		// ゴール更新
		goal_->Update();

		effectManager_->Update();

		if (deathParticles_ != nullptr)
			deathParticles_->Update();

		UpdateCameraAndTransforms();

		if (deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, duration_);
		}
		break;

	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		// 必要なオブジェクトの背景更新のみ記述（省略）
		break;
	}

	// 死んだエネミーの自動削除
	enemies_.remove_if([](Enemy *enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
		});
	shieldEnemies_.remove_if([](ShieldEnemy *se) {
		if (se->GetIsDead()) {
			delete se;
			return true;
		}
		return false;
		});
}

void GameScene::Draw() {
	Model::PreDraw(); // 開始

	// スカイドーム
	skydome_->Draw();

	switch (phase_) {
	case Phase::kFadeIn:
	case Phase::kPlay:
		// プレイヤー
		player_->Draw();

		break;
	case Phase::kFadeOut:
	case Phase::kDeath:
		// 死亡時パーティクル
		if (deathParticles_ != nullptr) {
			deathParticles_->Draw();
		}
		break;
	};

	// エネミー
	for (Enemy *enemy : enemies_) {
		enemy->Draw();
		enemy->SetMapChipField(mapChipField_);
	}

	// 盾エネミー
	for (ShieldEnemy *shieldEnemy : shieldEnemies_) {
		shieldEnemy->Draw();
		shieldEnemy->SetMapChipField(mapChipField_);
	}

	// ゴール
	goal_->Draw();


	// エフェクト
	effectManager_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform *> &worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform *worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, cameraManager_->GetCamera());
		}
	}

	// フェード
	fade_->Draw();

	Model::PostDraw(); // 終了
}
#pragma endregion

void GameScene::GenerateBlocks() {
	uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 配列の要素数を拡張
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal, nullptr);
	}

	// マップ全体の解析ループ
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			MapChipType type = mapChipField_->GetMapChipTypeByIndex(j, i);
			Vector3 position = mapChipField_->GetMapChipPositionByIndex(j, i);

			switch (type) {
			case MapChipType::kBlock: {
				// ブロックの生成
				WorldTransform *worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransform->translation_ = position;

				worldTransformBlocks_[i][j] = worldTransform;
				break;
			}
			case MapChipType::kPlayer: {
				// プレイヤーの位置をCSVの場所に初期化
				player_ = new Player;
				player_->Initialize(modelPlayer_, modelPlayerAttack_, &cameraManager_->GetCamera(), position);
				player_->SetMapChipField(mapChipField_);
				break;
			}
			case MapChipType::kEnemy: {
				// サブIDなどに応じて通常敵と盾敵を分岐生成
				uint8_t subID = mapChipField_->GetMapChipSubIDByIndex(j, i);
				switch (subID) {
				case 0: {
					NormalEnemy *newEnemy = new NormalEnemy();
					newEnemy->Initialize(modelEnemy_, &cameraManager_->GetCamera(), position);
					newEnemy->SetGameScene(this);
					enemies_.push_back(newEnemy);
					break;
				}
				case 1: {
					ShieldEnemy *newShieldEnemy = new ShieldEnemy();
					newShieldEnemy->Initialize(modelShieldEnemy_, &cameraManager_->GetCamera(), position);
					newShieldEnemy->SetGameScene(this);
					shieldEnemies_.push_back(newShieldEnemy);
					break;
				}
				}
				break;
			}
			case MapChipType::kGoal: {
				goal_ = new Goal();
				goal_->Initialize(modelGoal_, &cameraManager_->GetCamera(), position);
				break;
			}
			}
		}
	}
}

void GameScene::UpdateCameraAndTransforms() {
	cameraManager_->Update();

	for (auto &worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform *worldTransformBlock : worldTransformBlockLine) {
			if (worldTransformBlock) {
				UpdateWorldTransform(*worldTransformBlock);
			}
		}
	}
}

void GameScene::CheckAllCollision() {
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = GetAABB(player_->GetWorldTransform());

	// 自キャラと敵弾全ての当たり判定
	for (Enemy *enemy : enemies_) {
		if (enemy->GetIsCollisionDisabled()) {
			continue;
		}

		// 敵弾の座標
		aabb2 = GetAABB(enemy->GetWorldTransform());

		// AABB同士の交差判定
		if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
			(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
			(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
			// 自キャラの衝突時関数を呼び出す
			player_->OnCollision(enemy);
			// 敵の衝突時関数を呼び出す
			enemy->OnCollision(player_);
		}
	}

	// 自キャラと敵弾全ての当たり判定
	for (ShieldEnemy *shieldEnemy : shieldEnemies_) {
		if (shieldEnemy->GetIsCollisionDisabled()) {
			continue;
		}

		// 敵弾の座標
		aabb2 = GetAABB(shieldEnemy->GetWorldTransform());

		// AABB同士の交差判定
		if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
			(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
			(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
			// 自キャラの衝突時関数を呼び出す
			player_->OnCollision(shieldEnemy);
			// 敵の衝突時関数を呼び出す
			shieldEnemy->OnCollision(player_);
		}
	}
#pragma endregion
	// プレイヤーとゴールの当たり判定
	if (player_ && goal_ && phase_ == Phase::kPlay) {
		const Vector3 playerPosition = player_->GetWorldPosition();

		if (goal_->CheckCollision(playerPosition, 2.0f, 2.0f)) {
			// クリアフラグを設定
			isCleared_ = true;

			// フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, duration_);
		}
	}
};

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		// 自キャラがデス状態
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3 &deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生、初期化
			modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticles_, &cameraManager_->GetCamera(), deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		break;
	}
};
