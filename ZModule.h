#pragma once
#include "ZAst.h"
#include <vector>
#include "ZFunc.h"

class ZFunc;

class ZModule : public ZAst {
public:
	ZModule(std::string& name) : name(name) { }

    void addFunction(ZFunc* function) {
        functions.push_back(function);
    }

	std::vector<ZFunc*>& getFunctions() {
		return functions;
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "module " << name.c_str() << "\n";
		for (ZFunc* func : functions)
			func->dump(stream, depth + 1);
		
    }

	std::string& name;
    std::vector<ZFunc*> functions;
};
