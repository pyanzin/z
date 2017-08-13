#pragma once

#include <string>
#include <tuple>

class SourceRange {
public:
	SourceRange(std::string* src, int start = -1, int end = -1) {
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

	std::tuple<int, int> getRowCol(int position) {
		int row = 1;
		int col = 1;

		int i = 0;
		while (i++ != position)
			if ((*_src)[i] == '\n') {
				++row;
				col = 1;
			} else {
				++col;
			}		

		return std::make_tuple(row, col);
	}

    void setStart(int n) {
        _start = n;
    }

    void setEnd(int n) {
        _end = n;
	};

private:
	int _start;
	int _end;
	std::string* _src;
};
