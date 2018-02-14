#include "ZExpr.h"

class ZSizeOf : public ZExpr {
public:
    ZSizeOf(ZType* ztype, bool isComplex = false) {
        _ztype = ztype;
        this->isComplex = isComplex;
        setType(Int);
    }

    ZType* getWrappedType() {
        return _ztype;
    }

    bool isComplex;
private:
    ZType* _ztype;
};
