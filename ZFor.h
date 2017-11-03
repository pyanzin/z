#pragma once
#include "ZAst.h"
#include "ZExpr.h"

class ZFor : public ZAst {
public:

	ZFor(ZAst* body, ZAst* pre = nullptr, ZExpr* cond = nullptr, ZExpr* post = nullptr) {
		_body = body;
		_pre = pre;
		_cond = cond;
		_post = post;
	}

    void replaceChild(ZAst* oldChild, ZAst* newChild) override {
        if (_body == oldChild) {
            _body = static_cast<ZExpr*>(newChild);
            return;
        }
        
        if (_pre == oldChild) {
            _pre = newChild;
            return;
        }

        adopt(newChild);
        if (_cond == oldChild) {
            _cond = static_cast<ZExpr*>(newChild);
            return;
        }

        if (_post == oldChild) {
            _post = static_cast<ZExpr*>(newChild);
            return;
        }

        throw exception("wrong call to replaceChild in ZFor");
    }

	ZAst* getBody() {
		return _body;
	}

	ZAst* getPre() {
		return _pre;
	}

	ZExpr* getCond() {
		return _cond;
	}

	ZExpr* getPost() {
		return _post;
	}

    void accept(ZVisitor* visitor) override {
        visitor->visit(this);
    }

	void dump(std::ostream& stream, unsigned depth) override {
		dumpTab(stream, depth);
		stream << "for\n";
		_pre->dump(stream, depth + 1);
		_cond->dump(stream, depth + 1);
		_post->dump(stream, depth + 1);

		dumpTab(stream, depth);
		stream << "do\n";
		_body->dump(stream, depth + 1);
	}

private:
	ZAst* _body;
	ZAst* _pre;
	ZExpr* _cond;
	ZExpr* _post;
};
