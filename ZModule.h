#pragma once
#include "ZAst.h"
#include <vector>

class ZFunc;

class ZModule : public ZAst {
public:
    void addFunction(ZFunc* function) {
        _functions.push_back(function);
    }

	std::vector<ZFunc*>& getFunctions() {
		return _functions;
    }
private:
    std::vector<ZFunc*> _functions;
};
