#pragma once

class ZVisitor;

class ZAst {
public:
    virtual void accept(ZVisitor* visitor);
};

