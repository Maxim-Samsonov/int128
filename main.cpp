#include "expression.hpp"
#include <iostream>

int main() {
    auto expr = make_unique<Const>(Int128(2)) * make_unique<Variable>("x") + make_unique<Const>(Int128(1));

    map<string, Int128> vars;
    vars["x"] = Int128(100);
    vars["y"] = Int128(42);

    Int128 result = expr->eval(vars);
    cout << *expr << " = " << result << endl;

    auto expr2 = expr->clone();

    Int128 a = Int128(-123);
    Int128 b = Int128("456");
    Int128 c = a + b;
    cout << a << " + " << b << " = " << c << endl;

    return 0;
}
