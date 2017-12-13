#include <iostream>
#include "ZCompilerSettings.h"
#include "ZCompiler.h"

int main(int argc, char* args[]) {
	ZCompilerSettings* settings = new ZCompilerSettings(argc, args);

	if (settings->hasErrors()) {
		for (std::string error : settings->errors)
			std::cout << error << "\n";
		return 1;
	}

	ZCompiler* instance = new ZCompiler(settings);
	instance->run();
   
    return 0;
}
