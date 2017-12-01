#include <string>
#include "SourceRange.h"

class ParserError {
public:
    ParserError(std::string text, SourceRange* sourceRange) : text(text) {
        this->sourceRange = sourceRange;
    }
    std::string text;
    SourceRange* sourceRange;
};
