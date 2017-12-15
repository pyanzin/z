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
		while (i < argc) {
			bool isLastArg = i == argc - 1;
			char* param = args[i];
			if (param[0] != '-')
				sources.push_back(std::string(param));
			else if (strcmp(args[i], "-o0") == 0)
				optLevel = 0;
			else if (strcmp(args[i], "-o1") == 0)
				optLevel = 1;
			else if (strcmp(args[i], "-dump") == 0) {
				if (isLastArg)
					addError("-dump argument requires a value after it");
				else {
					char* phases = args[++i];
					int phaseI = 0;
					while (char phaseChar = phases[phaseI++]) {
						switch (phaseChar) {
						case '0':
							dumpParserTree = true;
							break;
						case '1':
							dumpTypingTree = true;
							break;
						case '2':
							dumpLlvmBeforeOpt = true;
							break;
						case '3':
							dumpLlvmAfterOpt = true;
							break;
						default:
							addError(std::string("Unknown phase for -dump: '") + phaseChar + "'");
						}
					}
				}
			}
			++i;
		}

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
