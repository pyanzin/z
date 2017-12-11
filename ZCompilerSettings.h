#pragma once
#include <string>
#include <vector>

class ZCompilerSettings {
public:
	ZCompilerSettings(int argc, char* args[]) {
		if (argc <= 1) {
			addError("No input files");
			return;
		}
		int i = 1;

	}

	bool hasErrors() {
		return errors.size() > 0;
	}

	void addError(std::string error) {
		errors.push_back(error);
	}

	int optLevel = 1;
	bool dumpParserTree = false;
	bool dumpTypingTree = false;
	bool dumpLlvmBeforeOpt = false;
	bool dumpLlvmAfterOpt = false;

	std::vector<std::string> sources;

	std::vector<std::string> errors;
};
