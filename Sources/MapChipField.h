#pragma once
#include "KamataEngine.h"
#include <map>
#include <vector>

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
	kPlayer,// プレイヤー
	kEnemy,	// 敵
};

struct MapChipDataUint {
	MapChipType type;
	uint8_t subID;
};

// マップチップデータ構造体
struct MapChipData {
	std::vector<std::vector<MapChipDataUint>> data;
};

// マップチップテーブル
namespace {
std::map<char, MapChipType> mapChipTable = {
    {'B', MapChipType::kBlock },
    {'P', MapChipType::kPlayer},
    {'E', MapChipType::kEnemy },
};
}

enum MapChipCharIndex { 
	kChipType = 0, 
	kChipSubID = 1 
};

class MapChipField {
public:
	// 座標からマップチップ番号を計算
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};


	MapChipField();  // コンストラクタ
	~MapChipField(); // デストラクタ

	// リセット
	void ResetMapChipData();

	// 読み込み
	void LoadMapChipCsv(const std::string& filePath);

	// マップチップ種別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	
	// マップチップ座標の取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex); 

	// マップチップサブIDの取得
	uint8_t GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex);

	// 縦方向の個数を取得
	uint32_t GetNumBlockVirtical();

	// 横方向の個数を取得
	uint32_t GetNumBlockHorizontal();

	// 座標からマップチップ番号を計算
	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	// ブロックの範囲取得
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	// ブロックのサイズ取得
	float GetBlockWidth() const { return kBlockWidth; }
	float GetBlockHeight() const { return kBlockHeight; }

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	// ブロックの偶数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータ構造体
	MapChipData mapChipData_;
};