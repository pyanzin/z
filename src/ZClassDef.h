#pragma once
#include "ZAst.h"
#include "ZFunc.h"
#include "ZVisitor.h"
#include "ZClassType.h";

class ZClassDef : ZAst {
public:
    ZClassDef(std::string name, ZClassType* type, std::vector<ZFunc*>& methods) : methods(methods) {
        this->name = name;
        _type = type;
    }

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);

        stream << "class(" << name << ") : " << "\n";
        stream << "methods" << "\n";
        for (auto method : methods)
            method->dump(stream, depth + 1);
    }

    ZType* getType() {
        return _type;
    }

    string name;
    std::vector<ZFunc*> methods;

private:
    ZType* _type;
};
