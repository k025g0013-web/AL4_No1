#pragma once

#include "KamataEngine.h"
#include <3d\Model.h>

#include "MapChipField.h"
#include "Math/Collision.h"
#include "LRDirection.h"
#include "CollisionMapInfo.h"

class GameScene;
class Player;

class Enemy {
public:
    enum class Behavior {
        kUnknown,
        kWalk,
        kDeath,
        kGuard,
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
    virtual void Move();
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
    void BehaviorDeathInitialize();
    void BehaviorDeathUpdate();

    bool IsFrontBlocked() const;

    void InstallationStateSwitching(
        const CollisionMapInfo &info
    );

    void Turn();

protected:
    Behavior behavior_ = Behavior::kWalk;
    Behavior behaviorRequest_ = Behavior::kUnknown;

    KamataEngine::WorldTransform worldTransform_;

    KamataEngine::Model *model_ = nullptr;
    KamataEngine::Camera *camera_ = nullptr;

    MapChipField *mapChipField_ = nullptr;

    // マップとの衝突判定
    Collision collision_;

    KamataEngine::Vector3 velocity_{};

    LRDirection direction_ = LRDirection::kLeft;

    static inline const float kWalkSpeed = 0.02f;
    static inline const float kGravity = 0.01f;

    bool onGround_ = false;

    static inline const float kWidth = 1.9f;
    static inline const float kHeight = 1.9f;
    static inline const float kBlank = 0.0001f;
    static inline const float kAttenuationLanding = 0.1f;

    static inline const float kWalkMotionAngleStart = -15.0f;
    static inline const float kWalkMotionAngleEnd = 30.0f;
    static inline const float kWalkMotionTime = 1.0f;

    float walkTimer_ = 0.0f;

    uint32_t deathTimer_ = 0;
    static inline const uint32_t kDeathTime = 60;

    bool isDead_ = false;

    GameScene *gameScene_ = nullptr;
};