#pragma once

#include "ZExpr.h"
#include "ZVisitor.h"
#include "ZArg.h"
#include "ZFuncType.h"
#include "SymbolRef.h"

class ZLambda : public ZExpr {
public:
    ZLambda(std::vector<ZArg*>* args, ZExpr* body, SymbolRef* ref) {
        _args = args;
        _body = body;
        _ref = ref;

        adopt(_body);
        for (auto arg : *_args)
            adopt(arg);
    }

    ZType* getType() override {
        std::vector<ZType*>* argTypes = new std::vector<ZType*>();
        for (ZArg* arg : *_args) 
            argTypes->push_back(arg->getType());
        
        ZType* retType = _body->getType();

        return new ZFuncType(retType, *argTypes);
    }

    void setType(ZType* type) override;

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    }

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        if (_body == oldChild) {
            _body = static_cast<ZExpr*>(newChild);
            return;
        }

        throw exception("wrong call to replaceChild in ZLambda");
    }
    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);

        stream << "lambda " << "(";
        for (auto i = _args->begin(); i < _args->end(); ++i) {
            bool isLast = i == _args->end() - 1;
            stream << (*i)->getName() << ": " << (*i)->getType()->toString();
            if (!isLast)
                stream << ", ";
        }

        stream << ")\n";

        _body->dump(stream, depth + 1);
    }

    vector<ZArg*>* getArgs() {
        return _args;
    }

    ZExpr* getBody() {
        return _body;
    }

    ZType* getReturnType() {
        return _body->getType();
    }

    SymbolRef* getRef() {
        return _ref;
    }
private:
    vector<ZArg*>* _args;
    ZExpr* _body;
    SymbolRef* _ref;

};
