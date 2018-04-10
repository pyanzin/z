#ifndef ZBLOCK
#define ZBLOCK

#include "ZExpr.h"
#include <vector>

class ZBlock : public ZAst {
public:
    ZBlock(std::vector<ZAst*>* stmts) {
        _statements = *stmts;
        for (ZAst* stmt : *stmts)
            adopt(stmt);
    }

    void accept(ZVisitor* visitor) override;

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        adopt(newChild);
        for (int i = 0; i < _statements.size(); ++i) {
            auto stmt = _statements[i];
            if (stmt == oldChild) {
                _statements[i] = newChild;
                return;
            }
        }
        throw exception("wrong call to replaceChild in ZBlock");
    }

    void dump(std::ostream& stream, unsigned depth) override {
        dumpTab(stream, depth);

        stream << "block\n";
        for (ZAst* stmt : _statements)
            stmt->dump(stream, depth + 1);
    }

    std::vector<ZAst*>& getStatements() {
        return _statements;
    }

    std::vector<ZAst*> _statements;
};

#endif
