#pragma once

#include <string>

class CompilerException {
public:
    CompilerException(std::string text) {
        this->text = text;
    }

    std::string text;
};
