#pragma once
#include "ZAst.h"
#include <vector>

class ZFunc;

class ZModule : public ZAst {
public:
    void addFunction(ZFunc* function) {
        functions.push_back(function);
    }

private:
    std::vector<ZFunc*> functions;
};
