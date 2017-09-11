Compiler for some statically typed language.

extern def print(s: String): None

def main(): None = {
    var i = 0;
    while (i < 100) {
        print(i);
        i = i + 1;
    };
}
