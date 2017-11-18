#include "ZExpr.h"

class ZSizeOf : public ZExpr {
public:
    ZSizeOf(ZType* ztype) {
        _ztype = ztype;
        setType(Int);
    }

    ZType* getWrappedType() {
        return _ztype;
    }
private:
    ZType* _ztype;
};
