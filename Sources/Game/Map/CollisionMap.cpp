#include "CollisionMap.h"

#include <algorithm>
#include <cmath>

using namespace KamataEngine;


// マップチップとの当たり判定

void CollisionMap::CheckMap(
    CollisionMapInfo &info, const Vector3 &position,
    float width, float height) {

    // マップが設定されていなければ何もしない
    if (!mapChipField_) {
        return;
    }

    // 元の移動量を保存
    const Vector3 requestedMove = info.move;

    // X方向だけ移動して、左右の壁を判定
    Vector3 xMove = {
        requestedMove.x,
        0.0f,
        0.0f
    };

    // X方向の判定ではY方向に移動させない
    info.move.x = requestedMove.x;
    info.move.y = 0.0f;

    CheckRight(info, position, xMove, width, height);
    CheckLeft(info, position, xMove, width, height);

    // X方向の判定結果を反映した仮の座標
    Vector3 positionAfterX = position;
    positionAfterX.x += info.move.x;


    // Y方向だけ移動して、天井と床を判定
    Vector3 yMove = {
        0.0f,
        requestedMove.y,
        0.0f
    };

    // Y方向の判定では、X方向の確定結果を維持する
    info.move.y = requestedMove.y;

    CheckTop(info, positionAfterX, yMove, width, height);
    CheckBottom(info, positionAfterX, yMove, width, height);
}



// 上方向の当たり判定
void CollisionMap::CheckTop(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    // 上昇していない場合は天井判定をしない
    if (move.y <= 0.0f) {
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
void CollisionMap::CheckBottom(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    // 下方向に移動していない場合は着地判定しない
    if (move.y >= 0.0f) {
        return;
    }

    uint32_t checkCount = CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {

        float t =
            static_cast<float>(i) /
            static_cast<float>(checkCount);

        float previousT =
            static_cast<float>(i - 1) /
            static_cast<float>(checkCount);

        // ひとつ前の探索位置
        Vector3 previousCenter = {
            position.x + move.x * previousT,
            position.y + move.y * previousT,
            position.z + move.z * previousT
        };

        // 現在の探索位置
        Vector3 checkCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 previousLeftBottom =
            CornerPosition(
                previousCenter,
                kLeftBottom,
                width,
                height
            );

        Vector3 previousRightBottom =
            CornerPosition(
                previousCenter,
                kRightBottom,
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

        Vector3 rightBottom =
            CornerPosition(
                checkCenter,
                kRightBottom,
                width,
                height
            );

        // 左右それぞれの足元について、ブロック上面を通過したか確認
        auto CheckLanding = [&](const Vector3 &previousBottom,
            const Vector3 &currentBottom) -> bool {

                if (!IsBlock(currentBottom)) {
                    return false;
                }

                MapChipField::IndexSet indexSet =
                    mapChipField_->GetMapChipIndexSetByPosition(
                        currentBottom
                    );

                MapChipField::Rect rect =
                    mapChipField_->GetRectByIndex(
                        indexSet.xIndex,
                        indexSet.yIndex
                    );

                // 前の足元はブロック上面より上、
                // 現在の足元はブロック上面以下の場合だけ着地とする
                bool crossedTop =
                    previousBottom.y > rect.top &&
                    currentBottom.y <= rect.top;

                if (!crossedTop) {
                    return false;
                }

                // ブロック上面に足元が来るように移動量を補正
                float newMoveY =
                    rect.top - position.y
                    + height / 2.0f
                    + kBlank;

                // 元の移動量より下へ動かさず、上方向にも移動させない
                info.move.y = (std::clamp)(newMoveY, move.y, 0.0f);

                // 着地判定
                info.landing = true;

                return true;
            };

        if (CheckLanding(previousLeftBottom, leftBottom)) {
            return;
        }

        if (CheckLanding(previousRightBottom, rightBottom)) {
            return;
        }
    }
}


// 右方向の当たり判定
void CollisionMap::CheckRight(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    if (move.x <= 0.0f) {
        return;
    }

    uint32_t checkCount = CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {
        float previousT =
            static_cast<float>(i - 1) / checkCount;
        float t =
            static_cast<float>(i) / checkCount;

        Vector3 previousCenter = {
            position.x + move.x * previousT,
            position.y + move.y * previousT,
            position.z + move.z * previousT
        };

        Vector3 currentCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 previousRightTop =
            CornerPosition(previousCenter, kRightTop, width, height);
        Vector3 previousRightBottom =
            CornerPosition(previousCenter, kRightBottom, width, height);

        Vector3 rightTop =
            CornerPosition(currentCenter, kRightTop, width, height);
        Vector3 rightBottom =
            CornerPosition(currentCenter, kRightBottom, width, height);

        auto CheckSide = [&](const Vector3 &previousPoint,
            const Vector3 &currentPoint,
            bool isBottom) -> bool {

                if (!IsBlock(currentPoint)) {
                    return false;
                }

                auto indexSet =
                    mapChipField_->GetMapChipIndexSetByPosition(currentPoint);

                auto rect = mapChipField_->GetRectByIndex(
                    indexSet.xIndex, indexSet.yIndex);

                // 前の位置からブロックの左側面を横切ったか
                bool crossedSide =
                    previousPoint.x < rect.left &&
                    currentPoint.x >= rect.left;

                if (!crossedSide) {
                    return false;
                }

                // 下側の角が床の上面を横切った場合は、
                // 壁衝突ではなく着地候補として扱う
                if (isBottom &&
                    previousPoint.y > rect.top &&
                    currentPoint.y <= rect.top) {
                    return false;
                }

                // プレイヤー本体がブロックの高さと重なっているか
                bool overlapsY =
                    currentCenter.y + height / 2.0f > rect.bottom &&
                    currentCenter.y - height / 2.0f < rect.top;

                if (!overlapsY) {
                    return false;
                }

                float newMoveX =
                    rect.left - position.x - width / 2.0f - kBlank;

                info.move.x =
                    (std::clamp)(newMoveX, 0.0f, move.x);

                info.wall = true;
                return true;
            };

        if (CheckSide(previousRightTop, rightTop, false)) {
            return;
        }

        if (CheckSide(previousRightBottom, rightBottom, true)) {
            return;
        }
    }
}

// 左方向の当たり判定
void CollisionMap::CheckLeft(
    CollisionMapInfo &info,
    const Vector3 &position,
    const Vector3 &move,
    float width,
    float height) {

    if (move.x >= 0.0f) {
        return;
    }

    uint32_t checkCount = CalculateCheckCount(move);

    for (uint32_t i = 1; i <= checkCount; ++i) {
        float previousT =
            static_cast<float>(i - 1) / checkCount;
        float t =
            static_cast<float>(i) / checkCount;

        Vector3 previousCenter = {
            position.x + move.x * previousT,
            position.y + move.y * previousT,
            position.z + move.z * previousT
        };

        Vector3 currentCenter = {
            position.x + move.x * t,
            position.y + move.y * t,
            position.z + move.z * t
        };

        Vector3 previousLeftTop =
            CornerPosition(previousCenter, kLeftTop, width, height);
        Vector3 previousLeftBottom =
            CornerPosition(previousCenter, kLeftBottom, width, height);

        Vector3 leftTop =
            CornerPosition(currentCenter, kLeftTop, width, height);
        Vector3 leftBottom =
            CornerPosition(currentCenter, kLeftBottom, width, height);

        auto CheckSide = [&](const Vector3 &previousPoint,
            const Vector3 &currentPoint,
            bool isBottom) -> bool {

                if (!IsBlock(currentPoint)) {
                    return false;
                }

                auto indexSet =
                    mapChipField_->GetMapChipIndexSetByPosition(currentPoint);

                auto rect = mapChipField_->GetRectByIndex(
                    indexSet.xIndex, indexSet.yIndex);

                // 前の位置からブロックの右側面を横切ったか
                bool crossedSide =
                    previousPoint.x > rect.right &&
                    currentPoint.x <= rect.right;

                if (!crossedSide) {
                    return false;
                }

                // 下側の角が床の上面を横切った場合は、
                // 壁衝突ではなく着地候補として扱う
                if (isBottom &&
                    previousPoint.y > rect.top &&
                    currentPoint.y <= rect.top) {
                    return false;
                }

                bool overlapsY =
                    currentCenter.y + height / 2.0f > rect.bottom &&
                    currentCenter.y - height / 2.0f < rect.top;

                if (!overlapsY) {
                    return false;
                }

                float newMoveX =
                    rect.right - position.x + width / 2.0f + kBlank;

                info.move.x =
                    (std::clamp)(newMoveX, move.x, 0.0f);

                info.wall = true;
                return true;
            };

        if (CheckSide(previousLeftTop, leftTop, false)) {
            return;
        }

        if (CheckSide(previousLeftBottom, leftBottom, true)) {
            return;
        }
    }
}


// 指定した角の座標を計算
Vector3 CollisionMap::CornerPosition(
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
bool CollisionMap::IsBlock(const Vector3 &position) const {

    return GetMapChipType(position)
        == MapChipType::kBlock;
}


// 指定座標のマップチップ種類を取得
MapChipType CollisionMap::GetMapChipType(
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
uint32_t CollisionMap::CalculateCheckCount(
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