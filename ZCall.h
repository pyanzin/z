#pragma once
#include "ZExpr.h"
#include <string>
#include <vector>

class ZCall : public ZExpr {
public:
    ZCall(ZAst* callee, std::vector<ZExpr*>& args) {
        _callee = callee;
        _args = args;
    }

    ZAst* getCallee() {
        return _callee;
    }

    std::vector<ZExpr*>& getArgs() {
        return _args;
    }

private:
    ZAst* _callee;
    std::vector<ZExpr*> _args;
};
