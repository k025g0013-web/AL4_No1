#include "Collision.h"

#include <algorithm>
#include <cmath>

using namespace KamataEngine;


// マップチップとの当たり判定
void Collision::CheckMap(
    CollisionMapInfo &info,
    const Vector3 &position,
    float width,
    float height) {

    // マップが設定されていなければ何もしない
    if (!mapChipField_) {
        return;
    }

    const Vector3 move = info.move;

    CheckTop(info, position, move, width, height);
    CheckBottom(info, position, move, width, height);
    CheckRight(info, position, move, width, height);
    CheckLeft(info, position, move, width, height);
}


// 上方向の当たり判定
void Collision::CheckTop(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    if (move.y < 0.0f) {
        return;
    }

    uint32_t checkCount =
        CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {

        float t =
            static_cast<float>(i) /
            static_cast<float>(checkCount);

        Vector3 checkCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 leftTop =
            CornerPosition(
                checkCenter,
                kLeftTop,
                width,
                height
            );

        Vector3 rightTop =
            CornerPosition(
                checkCenter,
                kRightTop,
                width,
                height
            );

        bool leftHit =
            IsBlock(leftTop);

        bool rightHit =
            IsBlock(rightTop);

        if (!leftHit && !rightHit) {
            continue;
        }

        Vector3 hitPosition =
            leftHit ? leftTop : rightTop;

        MapChipField::IndexSet indexSet =
            mapChipField_->GetMapChipIndexSetByPosition(
                hitPosition
            );

        MapChipField::Rect rect =
            mapChipField_->GetRectByIndex(
                indexSet.xIndex,
                indexSet.yIndex
            );

        float newMoveY =
            (rect.bottom - position.y)
            - (height / 2.0f)
            - kBlank;

        info.move.y =
            (std::max)(0.0f, newMoveY);

        info.ceiling = true;

        return;
    }
}


// 下方向の当たり判定
void Collision::CheckBottom(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    if (move.y > 0.0f) {
        return;
    }

    uint32_t checkCount =
        CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {

        float t =
            static_cast<float>(i) /
            static_cast<float>(checkCount);

        Vector3 checkCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 leftBottom =
            CornerPosition(
                checkCenter,
                kLeftBottom,
                width,
                height
            );

        Vector3 rightBottom =
            CornerPosition(
                checkCenter,
                kRightBottom,
                width,
                height
            );

        bool leftHit =
            IsBlock(leftBottom);

        bool rightHit =
            IsBlock(rightBottom);

        if (!leftHit && !rightHit) {
            continue;
        }

        Vector3 hitPosition =
            leftHit ? leftBottom : rightBottom;

        MapChipField::IndexSet indexSet =
            mapChipField_->GetMapChipIndexSetByPosition(
                hitPosition
            );

        MapChipField::Rect rect =
            mapChipField_->GetRectByIndex(
                indexSet.xIndex,
                indexSet.yIndex
            );

        float newMoveY =
            (rect.top - position.y)
            + (height / 2.0f)
            + kBlank;

        info.move.y =
            (std::min)(0.0f, newMoveY);

        info.landing = true;

        return;
    }
}


// 右方向の当たり判定
void Collision::CheckRight(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    if (move.x < 0.0f) {
        return;
    }

    uint32_t checkCount =
        CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {

        float t =
            static_cast<float>(i) /
            static_cast<float>(checkCount);

        Vector3 checkCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 rightTop =
            CornerPosition(
                checkCenter,
                kRightTop,
                width,
                height
            );

        Vector3 rightBottom =
            CornerPosition(
                checkCenter,
                kRightBottom,
                width,
                height
            );

        bool topHit =
            IsBlock(rightTop);

        bool bottomHit =
            IsBlock(rightBottom);

        if (!topHit && !bottomHit) {
            continue;
        }

        Vector3 hitPosition =
            topHit ? rightTop : rightBottom;

        MapChipField::IndexSet indexSet =
            mapChipField_->GetMapChipIndexSetByPosition(
                hitPosition
            );

        MapChipField::Rect rect =
            mapChipField_->GetRectByIndex(
                indexSet.xIndex,
                indexSet.yIndex
            );

        float newMoveX =
            (rect.left - position.x)
            - (width / 2.0f)
            - kBlank;

        info.move.x =
            (std::min)(info.move.x, newMoveX);

        info.wall = true;

        return;
    }
}

// 左方向の当たり判定
void Collision::CheckLeft(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    if (move.x > 0.0f) {
        return;
    }

    uint32_t checkCount =
        CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {

        float t =
            static_cast<float>(i) /
            static_cast<float>(checkCount);

        Vector3 checkCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 leftTop =
            CornerPosition(
                checkCenter,
                kLeftTop,
                width,
                height
            );

        Vector3 leftBottom =
            CornerPosition(
                checkCenter,
                kLeftBottom,
                width,
                height
            );

        bool topHit =
            IsBlock(leftTop);

        bool bottomHit =
            IsBlock(leftBottom);

        if (!topHit && !bottomHit) {
            continue;
        }

        Vector3 hitPosition =
            topHit ? leftTop : leftBottom;

        MapChipField::IndexSet indexSet =
            mapChipField_->GetMapChipIndexSetByPosition(
                hitPosition
            );

        MapChipField::Rect rect =
            mapChipField_->GetRectByIndex(
                indexSet.xIndex,
                indexSet.yIndex
            );

        float newMoveX =
            (rect.right - position.x)
            + (width / 2.0f)
            + kBlank;

        info.move.x =
            (std::max)(info.move.x, newMoveX);

        info.wall = true;

        return;
    }
}


// 指定した角の座標を計算
Vector3 Collision::CornerPosition(
    const Vector3 &center,
    Corner corner,
    float width,
    float height) const {

    Vector3 offsetTable[kNumCorner] = {
        {+width / 2.0f, -height / 2.0f, 0.0f},
        {-width / 2.0f, -height / 2.0f, 0.0f},
        {+width / 2.0f, +height / 2.0f, 0.0f},
        {-width / 2.0f, +height / 2.0f, 0.0f},
    };

    Vector3 offset =
        offsetTable[static_cast<uint32_t>(corner)];

    return {
        center.x + offset.x,
        center.y + offset.y,
        center.z + offset.z
    };
}


// 指定座標のマップチップがブロックか
bool Collision::IsBlock(const Vector3 &position) const {

    return GetMapChipType(position)
        == MapChipType::kBlock;
}


// 指定座標のマップチップ種類を取得
MapChipType Collision::GetMapChipType(
    const Vector3 &position) const {

    if (!mapChipField_) {
        return MapChipType::kBlank;
    }

    MapChipField::IndexSet indexSet =
        mapChipField_->GetMapChipIndexSetByPosition(position);

    return mapChipField_->GetMapChipTypeByIndex(
        indexSet.xIndex,
        indexSet.yIndex
    );
}


// 移動量から探索回数を計算
uint32_t Collision::CalculateCheckCount(
    const Vector3 &move) const {

    float blockWidth =
        mapChipField_->GetBlockWidth();

    float blockHeight =
        mapChipField_->GetBlockHeight();

    float maxMove =
        (std::max)(
            std::abs(move.x),
            std::abs(move.y)
            );

    float checkSize =
        (std::min)(
            blockWidth,
            blockHeight
            );

    return (std::max)(
        1u,
        static_cast<uint32_t>(
            std::ceil(maxMove / checkSize)
            )
        );
}