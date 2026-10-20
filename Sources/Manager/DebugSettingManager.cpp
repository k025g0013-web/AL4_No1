#include <Windows.h>
#include "DebugSettingManager.h"

#include "Manager/StageManager.h"

void LoadDebugSettings(StageManager* stageManager) {
	std::ifstream file("debugSettings.ini");
	if (!file.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;

		std::stringstream lineStream(line);
		std::string key, value;

		if (lineStream >> key >> value) {
			if (key == "InitialStage") {
				stageManager->SetCurrentStageIndexByName(value);
			}
		}
	}
}