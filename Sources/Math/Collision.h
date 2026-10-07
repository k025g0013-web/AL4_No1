#pragma once

#include "KamataEngine.h"
#include "MapChipField.h"
#include "CollisionMapInfo.h"

#include <array>

class Collision {
public:
    Collision() = default;
    ~Collision() = default;

    // マップチップを設定
    void SetMapChipField(MapChipField *mapChipField) {
        mapChipField_ = mapChipField;
    }

    // マップとの衝突判定
    void CheckMap(
        CollisionMapInfo &info,
        const KamataEngine::Vector3 &position,
        float width,
        float height
    );

private:
    // 上方向
    void CheckTop(
        CollisionMapInfo &info,
        const KamataEngine::Vector3 &position,
        const KamataEngine::Vector3 &move,
        float width,
        float height
    );

    // 下方向
    void CheckBottom(
        CollisionMapInfo &info,
        const KamataEngine::Vector3 &position,
        const KamataEngine::Vector3 &move,
        float width,
        float height
    );

    // 右方向
    void CheckRight(
        CollisionMapInfo &info,
        const KamataEngine::Vector3 &position,
        const KamataEngine::Vector3 &move,
        float width,
        float height
    );

    // 左方向
    void CheckLeft(
        CollisionMapInfo &info,
        const KamataEngine::Vector3 &position,
        const KamataEngine::Vector3 &move,
        float width,
        float height
    );

    // 指定した角の座標を取得
    KamataEngine::Vector3 CornerPosition(
        const KamataEngine::Vector3 &center,
        Corner corner,
        float width,
        float height
    ) const;

    // 指定座標のマップチップがブロックか
    bool IsBlock(
        const KamataEngine::Vector3 &position
    ) const;

    // 指定座標のマップチップ種類を取得
    MapChipType GetMapChipType(
        const KamataEngine::Vector3 &position
    ) const;

    // 移動量から探索する分割数を取得
    uint32_t CalculateCheckCount(const KamataEngine::Vector3 &move) const;

private:
    MapChipField *mapChipField_ = nullptr;

    // めり込み防止用の微小値
    static inline const float kBlank = 0.0001f;
};