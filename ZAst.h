#pragma once
#include "ZVisitor.h"

class ZAst {
public:
    void accept(ZVisitor* visitor);
};

