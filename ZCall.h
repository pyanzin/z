#pragma once
#include "ZExpr.h"
#include <string>
#include <vector>
#include "ZBasicTypes.h"
#include "SymbolRef.h"

class ZCall : public ZExpr {
public:
	ZCall(ZExpr* callee, std::vector<ZExpr*>& args, std::vector<ZType*>* typeParams, SymbolRef* ref) : args(args) {
		this->callee = callee;
		_typeArgs = typeParams;
		adopt(callee);
		for (auto arg : args)
			adopt(arg);
    }

	void accept(ZVisitor* visitor) override;

	void replaceChild(ZAst* oldChild, ZAst* newChild) override {
		adopt(newChild);
		for (int i = 0; i < args.size(); ++i) {
			auto arg = args[i];
			if (arg == oldChild) {
				args[i] = static_cast<ZExpr*>(newChild);
				return;
			}
		}
		throw exception("wrong call to replaceChild in ZCall");
	}

    std::vector<ZExpr*>& getArgs() {
        return args;
    }

	SymbolRef* getRef() {
		return _ref;
	}

	std::vector<ZType*>* getTypeArgs() {
		return _typeArgs;
	}

    ZExpr* callee;
    std::vector<ZExpr*>& args;
	std::vector<ZType*>* _typeArgs;
	SymbolRef* _ref;

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);

		stream << "call : " << getType()->toString() << "\n";
		callee->dump(stream, depth + 1);
		for (ZExpr* expr : args)
			expr->dump(stream, depth + 1);		
	};
};
