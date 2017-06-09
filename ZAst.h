#pragma once
#include "ZVisitor.h"

class ZAst {
public:
    virtual void accept(ZVisitor* visitor);
};

