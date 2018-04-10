#pragma once

#include <string>
#include <tuple>

using namespace std;

class SourceRange {
public:
    SourceRange(string* src, int start = -1, int end = -1) {
        _start = start;
        _end = end;
        _src = src;
    }

    int getStart() {
        return _start;
    }

    int getEnd() {
        return _end;
    }

    tuple<int, int> getRowCol(int position) {
        int row = 1;
        int col = 1;

        int i = 0;
        while (i++ != position)
            if ((*_src)[i] == '\n') {
                ++row;
                col = 1;
            }
            else {
                ++col;
            }

        return make_tuple(row, col);
    }

    std::string getPosition() {
        auto rowCol = getRowCol(_start);
        return to_string(get<0>(rowCol)) + ":" + to_string(get<1>(rowCol));
    }

    void setStart(int n) {
        _start = n;
    }

    void setEnd(int n) {
        _end = n;
    }

private:
    int _start;
    int _end;
    std::string* _src;
};
