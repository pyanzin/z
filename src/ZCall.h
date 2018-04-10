#pragma once
#include "ZExpr.h"
#include <string>
#include <vector>
#include "ZBasicType.h"
#include "SymbolRef.h"
#include "ZVisitor.h"

class ZCall : public ZExpr {
public:
    ZCall(ZExpr* callee, std::vector<ZExpr*>& args, std::vector<ZType*>* typeParams, SymbolRef* ref) : args(args) {
        this->callee = callee;
        _typeArgs = typeParams;
        adopt(callee);
        for (auto arg : args)
            adopt(arg);
        setRef(ref);
    }

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    };

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);

        if (callee == oldChild) {
            callee = dynamic_cast<ZExpr*>(newChild);
            return;
        }

        for (int i = 0; i < args.size(); ++i) {
            auto arg = args[i];
            if (arg == oldChild) {
                args[i] = dynamic_cast<ZExpr*>(newChild);
                return;
            }
        }
        throw exception("wrong call to replaceChild in ZCall");
    }

    std::vector<ZExpr*>& getArgs() {
        return args;
    }

    std::vector<ZType*>* getTypeArgs() {
        return _typeArgs;
    }

    ZExpr* callee;
    std::vector<ZExpr*>& args;
    std::vector<ZType*>* _typeArgs;

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);

        stream << "call : " << getType()->toString() << "\n";
        callee->dump(stream, depth + 1);
        for (ZExpr* expr : args)
            expr->dump(stream, depth + 1);
    };
};
