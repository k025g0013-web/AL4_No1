#include "MapChipField.h"
#include <fstream>
#include <sstream>

using namespace KamataEngine;

// マップチップテーブル
namespace {
std::map<char, MapChipType> mapChipTypeTable = {
    {'B', MapChipType::kBlock },
    {'P', MapChipType::kPlayer},
    {'E', MapChipType::kEnemy },
};
}

// コンストラクタ
MapChipField::MapChipField() {};
// デストラクタ
MapChipField::~MapChipField() {};

void MapChipField::ResetMapChipData() {
	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipDataUint>& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
};

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	ResetMapChipData();

	std::ifstream file;
	file.open(filePath);
#ifdef _DEBUG
	assert(file.is_open());
#endif 

	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		if (!getline(mapChipCsv, line)) {
			break; // CSVの行数が足りない場合は終了
		}

		std::istringstream lineStream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			// カンマ区切りで取得。残りの文字がなければ空文字になる
			if (!std::getline(lineStream, word, ',')) {
				word = "";
			}

			// 【修正】word[kChipType] をキーにしてテーブルに含まれるか判定
			if (word.empty() || !mapChipTypeTable.contains(word[kChipType])) {
				mapChipData_.data[i][j].type = MapChipType::kBlank;
				mapChipData_.data[i][j].subID = 0;
				continue;
			}

			// マップチップタイプを設定
			mapChipData_.data[i][j].type = mapChipTypeTable[word[kChipType]];

			// サブIDを設定
			if (word.size() <= kChipSubID) {
				mapChipData_.data[i][j].subID = 0;
				continue;
			}
			mapChipData_.data[i][j].subID = static_cast<uint8_t>(word[kChipSubID] - '0');
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical) {
		return MapChipType::kBlank;
	}
	return mapChipData_.data[yIndex][xIndex].type;
}

uint8_t MapChipField::GetMapChipSubIDByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= kNumBlockHorizontal || yIndex >= kNumBlockVirtical) {
		return 0;
	}
	return mapChipData_.data[yIndex][xIndex].subID;
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {
	return Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0); 
}

uint32_t MapChipField::GetNumBlockVirtical() { return kNumBlockVirtical; }
uint32_t MapChipField::GetNumBlockHorizontal() { return kNumBlockHorizontal; }

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>((position.y + (kBlockHeight / 2)) / kBlockHeight);
	return indexSet;
}

MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect{};
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}