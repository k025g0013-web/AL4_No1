#pragma once

#include "KamataEngine.h"
#include <3d\Model.h>

#include "MapChipField.h"
#include "Math/Collision.h"
#include "LRDirection.h"
#include "CollisionMapInfo.h"

#include <numbers>

class GameScene;
class Player;

class Enemy {
public:
    // 状態変化
    enum class Behavior {
        kUnknown,
        kWalk,
        kDeath,
        kGuard,
    };

    // 歩行アニメーションの種類
    enum class WalkAnimationType {
        kXRotate,
        kZRotate,
    };

public:
    virtual ~Enemy() = default;

    virtual void Initialize(
        KamataEngine::Model *model,
        KamataEngine::Camera *camera,
        const KamataEngine::Vector3 &position
    );

    virtual void Update() = 0;
    virtual void Draw();
    virtual void Move(bool applyGravity);
    virtual void OnCollision(Player *player);

    void SetMapChipField(MapChipField *mapChipField) {
        mapChipField_ = mapChipField;
        collision_.SetMapChipField(mapChipField_);
    }

    void SetGameScene(GameScene *gameScene) {
        gameScene_ = gameScene;
    }

    KamataEngine::Vector3 GetWorldPosition() const;

    const KamataEngine::WorldTransform &
        GetWorldTransform() const {
        return worldTransform_;
    }

    bool GetIsDead() const {
        return isDead_;
    }

    virtual bool GetIsCollisionDisabled() const {
        return behavior_ == Behavior::kDeath || isDead_;
    }

protected:
    void BehaviorWalkInitialize();
    void BehaviorWalkUpdate();
    virtual void ApplyWalkAnimation(float degree);

    void BehaviorDeathInitialize();
    void BehaviorDeathUpdate();
    void SetDeathVelocity(const Player *player);
    
    bool IsFrontBlocked() const;
    void Turn();
    void UpdateTurnAnimation();


    void InstallationStateSwitching(const CollisionMapInfo &info);

protected:
    /* 振る舞い
    --------------------*/
    // 現在の振る舞い
    Behavior behavior_ = Behavior::kWalk;
    // 振る舞い変更リクエスト
    Behavior behaviorRequest_ = Behavior::kUnknown;

    /* ワールド変換
    --------------------*/
    // ワールド変換データ
    KamataEngine::WorldTransform worldTransform_;

    /* モデル・カメラ
    --------------------*/
    // モデル
    KamataEngine::Model *model_ = nullptr;
    // カメラ
    KamataEngine::Camera *camera_ = nullptr;

    /* マップチップ
    --------------------*/
    // マップチップフィールド
    MapChipField *mapChipField_ = nullptr;

    /* 衝突判定
    --------------------*/
    // マップとの衝突判定
    Collision collision_;

    /* 移動
    --------------------*/
    // 移動速度
    KamataEngine::Vector3 velocity_{};
    // 左右の向き
    LRDirection direction_ = LRDirection::kLeft;
    // 歩行速度
    static inline const float kWalkSpeed = 0.02f;
    // 重力
    static inline const float kGravity = 0.01f;

    /* 接地判定
    --------------------*/
    // 接地しているか
    bool onGround_ = false;

    /* 当たり判定サイズ
    --------------------*/
    // 横幅
    static inline const float kWidth = 1.9f;
    // 高さ
    static inline const float kHeight = 1.9f;
    // 隙間
    static inline const float kBlank = 0.0001f;
    // 着地時の減衰率
    static inline const float kAttenuationLanding = 0.1f;

    /* 歩行アニメーション
    --------------------*/
    // 歩行アニメーションの種類
    WalkAnimationType walkAnimationType_ = WalkAnimationType::kXRotate;
    // 回転角度の開始値
    static inline const float kWalkMotionAngleStart = -15.0f;
    // 回転角度の変化量
    static inline const float kWalkMotionAngleEnd = 30.0f;
    // アニメーション時間
    static inline const float kWalkMotionTime = 1.0f;
    // 経過時間
    float walkTimer_ = 0.0f;

    /* 死亡
    --------------------*/
    // 死亡アニメーションの経過時間
    uint32_t deathTimer_ = 0;
    // 死亡アニメーションの時間
    static inline const uint32_t kDeathTime = 60;
    // 死亡しているか
    bool isDead_ = false;
    // 死亡時の吹き飛び方向
    float deathDirection_ = 1.0f;
    // 死亡時の回転速度
    KamataEngine::Vector3 deathRotationVelocity_{};


    /* ゲームシーン
    --------------------*/
    // ゲームシーン
    GameScene *gameScene_ = nullptr;

    /* 回転アニメーション
    --------------------*/
    // フラグ
    bool isTurning_ = false;
    // 経過時間
    float turnTimer_ = 0.0f;
    // 時間
    static constexpr float kTurnTime = 0.25f;
    // 開始時の角度
    float turnStartRotationY_ = 0.0f;
    // 角度（180度）
    static constexpr float kTurnAngle = static_cast<float>(std::numbers::pi);
};