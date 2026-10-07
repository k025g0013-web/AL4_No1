#pragma once
#include <sstream>
#include <fstream>
#include <cassert>
#include <vector>
#include <string>

struct StageData {
	std::string name{};
	int32_t timeLimit{};
};

class StageManager {
public:
	void LoadStageData();

	void SetCurrentStageIndex(int32_t index) {
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()) && "indexが正常な範囲である");
		assert(index && "");
		currentStageIndex_ = index;
	}

	void SetCurrentStageIndexByName(const std::string& name);

	int32_t GetCurrentStageIndex() const { return currentStageIndex_; }

	const StageData& GetStageData(int32_t index) const { 
		assert(index >= 0 && index < static_cast<int32_t>(stageDatas_.size()) && "indexが正常な範囲である");
		return stageDatas_[index];
	}

	const StageData& GetCurrentStageData() const { 
		return GetStageData(currentStageIndex_);
	}

private:
	// 全ステージデータ
	std::vector<StageData> stageDatas_;

	// 現在のステージ番号
	int32_t currentStageIndex_ = 0;
};