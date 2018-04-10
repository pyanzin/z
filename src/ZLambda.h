#pragma once

#include "ZExpr.h"
#include "ZVisitor.h"
#include "ZArg.h"
#include "ZFuncType.h"
#include "SymbolRef.h"

class ZLambda : public ZExpr {
public:
    ZLambda(std::vector<ZArg*>* args, ZType* retType, ZAst* body, SymbolRef* ref) {
        _args = args;
        _retType = retType;
        _body = body;

        adopt(_body);
        for (auto arg : *_args)
            adopt(arg);
        setRef(ref);
    }

    ZType* getType() override {
        std::vector<ZType*>* argTypes = new std::vector<ZType*>();
        for (ZArg* arg : *_args)
            argTypes->push_back(arg->getType());

        return new ZFuncType(_retType, *argTypes);
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
            stream << (*(*i)->getName()) << ": " << (*i)->getType()->toString();
            if (!isLast)
                stream << ", ";
        }


        stream << ") : " << getReturnType()->toString() << "\n";


        _body->dump(stream, depth + 1);
    }

    vector<ZArg*>* getArgs() {
        return _args;
    }

    ZAst* getBody() {
        return _body;
    }

    ZType* getReturnType() {
        return _retType;
    }

    void setRetType(ZType* retType) {
        _retType = retType;
    }

private:
    vector<ZArg*>* _args;
    ZType* _retType;
    ZAst* _body;
};
