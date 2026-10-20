#include "StageManager.h"

void StageManager::LoadStageData() { 
	// ステージデータのパス
	const std::string filePath = "Resources/stageDatas.csv";

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream stageDatasCsv;
	stageDatasCsv << file.rdbuf();
	file.close();
	assert(file && "ステージデータファイルが存在しません");

	// ステージデータを最終行まで1行ずつ読み込む
	std::string line;
	while (std::getline(stageDatasCsv, line)) {
		std::stringstream lineStream(line);

		StageData stageData;
		std::string word;

		// ステージ名
		if (std::getline(lineStream, word, ',')) {
			stageData.name = word;
		}

		// 制限時間
		if (std::getline(lineStream, word, ',')) {
			stageData.timeLimit = std::stoi(word);
		}

		// ステージデータテーブルに格納する
		stageDatas_.push_back(stageData);
	}
}

void StageManager::SetCurrentStageIndexByName(const std::string& name) {
	for (size_t i = 0; i < stageDatas_.size(); ++i) {
		if (stageDatas_[i].name == name) {
			currentStageIndex_ = static_cast<int32_t>(i);
			return;
		}
	}
	assert(false && "指定されたファイル名は存在しません");
}