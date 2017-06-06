#pragma once

class ZAst;
class ZArg;

class ZVisitor {
public:
    ZAst* visit(ZAst* ast);

    ZAst* visit(ZArg* ast);
};
