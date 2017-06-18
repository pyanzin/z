#pragma once
#include <string>
#include "ZExpr.h"

class ZString : public ZExpr{
public:
    ZString(std::string& value) : _value(value) { }

    std::string& getValue() {
        return _value;
    }

private:
    std::string& _value;
};
