#pragma once

#include "ZExpr.h"
#include "ZVisitor.h"
#include "ZArg.h"
#include "ZFuncType.h"

class ZLambda : public ZExpr {
public:
    ZLambda(std::vector<ZArg*>* args, ZExpr* body) {
        _args = args;
        _body = body;
    }

    ZType* getType() override {
        std::vector<ZType*>* argTypes = new std::vector<ZType*>();
        for (ZArg* arg : *_args) 
            argTypes->push_back(arg->getType());
        
        ZType* retType = _body->getType();

        return new ZFuncType(retType, *argTypes);
    }

    void setType(ZType* type) override {
        ZFuncType* funcType = dynamic_cast<ZFuncType*>(type);

        if (!funcType)
            throw std::exception(string("Unable to assign type " + type->toString() + " to lambda").c_str());

        for (int i = 0; i < _args->size(); ++i)
            (*_args)[i]->setType(funcType->getParamTypes()[i]);

        _body->setType(funcType->getRetType());
    }

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
private:
    vector<ZArg*>* _args;
    ZExpr* _body;

};
