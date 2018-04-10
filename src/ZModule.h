#pragma once
#include "ZAst.h"
#include <vector>
#include "ZFunc.h"
#include "ZClassDef.h"

class ZFunc;

class ZModule : public ZAst {
public:
    ZModule(std::string& name) : name(name) {
    }

    void addFunction(ZFunc* function) {
        functions.push_back(function);
        adopt(function);
    }

    std::vector<ZFunc*>& getFunctions() {
        return functions;
    }

    ZFunc* findFunc(string& name) {
        for (ZFunc* func : functions)
            if (*func->getName() == name)
                return func;
        return nullptr;
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);

        stream << "module " << name.c_str() << "\n";
        for (ZClassDef* c : classes)
            c->dump(stream, depth + 1);

        for (ZFunc* func : functions)
            func->dump(stream, depth + 1);
    }

    void ZModule::addClass(ZClassDef* zclassdef) {
        classes.push_back(zclassdef);
    }

    std::string& name;
    std::vector<ZFunc*> functions;
    std::vector<ZClassDef*> classes;
};
