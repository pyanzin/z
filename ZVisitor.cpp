#include "ZVisitor.h"
#include <cstdio>

ZAst* ZVisitor::visit(ZAst* ast) {
    printf("ZAst visited");
    return ast;
}

ZAst* ZVisitor::visit(ZArg* ast) {
    printf("ZArg visited");
    return (ZAst*)ast;
}
