#pragma once
#include "int128.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

class Expression {
public:
    virtual ~Expression() {}
    virtual Int128 eval(map<string, Int128>& vars) = 0;
    virtual unique_ptr<Expression> clone() = 0;
    virtual string to_string() = 0;
};

class Const : public Expression {
private:
    Int128 value;

public:
    Const(Int128 val) : value(val) {}

    Int128 eval(map<string, Int128>& vars) override {
        return value;
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Const>(value);
    }

    string to_string() override {
        return value.str();
    }
};

class Variable : public Expression {
private:
    string name;

public:
    Variable(string n) : name(n) {}

    Int128 eval(map<string, Int128>& vars) override {
        return vars[name];
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Variable>(name);
    }

    string to_string() override {
        return name;
    }
};

class Negate : public Expression {
private:
    unique_ptr<Expression> expr;

public:
    Negate(unique_ptr<Expression> e) : expr(move(e)) {}

    Int128 eval(map<string, Int128>& vars) override {
        return -expr->eval(vars);
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Negate>(expr->clone());
    }

    string to_string() override {
        return "-" + expr->to_string();
    }
};

class Add : public Expression {
private:
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;

public:
    Add(unique_ptr<Expression> l, unique_ptr<Expression> r)
        : left(move(l)), right(move(r)) {
    }

    Int128 eval(map<string, Int128>& vars) override {
        return left->eval(vars) + right->eval(vars);
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Add>(left->clone(), right->clone());
    }

    string to_string() override {
        return "(" + left->to_string() + " + " + right->to_string() + ")";
    }
};

class Subtract : public Expression {
private:
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;

public:
    Subtract(unique_ptr<Expression> l, unique_ptr<Expression> r)
        : left(move(l)), right(move(r)) {
    }

    Int128 eval(map<string, Int128>& vars) override {
        return left->eval(vars) - right->eval(vars);
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Subtract>(left->clone(), right->clone());
    }

    string to_string() override {
        return "(" + left->to_string() + " - " + right->to_string() + ")";
    }
};

class Multiply : public Expression {
private:
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;

public:
    Multiply(unique_ptr<Expression> l, unique_ptr<Expression> r)
        : left(move(l)), right(move(r)) {
    }

    Int128 eval(map<string, Int128>& vars) override {
        return left->eval(vars) * right->eval(vars);
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Multiply>(left->clone(), right->clone());
    }

    string to_string() override {
        return "(" + left->to_string() + " * " + right->to_string() + ")";
    }
};

class Divide : public Expression {
private:
    unique_ptr<Expression> left;
    unique_ptr<Expression> right;

public:
    Divide(unique_ptr<Expression> l, unique_ptr<Expression> r)
        : left(move(l)), right(move(r)) {
    }

    Int128 eval(map<string, Int128>& vars) override {
        return left->eval(vars) / right->eval(vars);
    }

    unique_ptr<Expression> clone() override {
        return make_unique<Divide>(left->clone(), right->clone());
    }

    string to_string() override {
        return "(" + left->to_string() + " / " + right->to_string() + ")";
    }
};

unique_ptr<Expression> operator+(unique_ptr<Expression> l, unique_ptr<Expression> r) {
    return make_unique<Add>(move(l), move(r));
}

unique_ptr<Expression> operator-(unique_ptr<Expression> l, unique_ptr<Expression> r) {
    return make_unique<Subtract>(move(l), move(r));
}

unique_ptr<Expression> operator*(unique_ptr<Expression> l, unique_ptr<Expression> r) {
    return make_unique<Multiply>(move(l), move(r));
}

unique_ptr<Expression> operator/(unique_ptr<Expression> l, unique_ptr<Expression> r) {
    return make_unique<Divide>(move(l), move(r));
}

unique_ptr<Expression> operator-(unique_ptr<Expression> e) {
    return make_unique<Negate>(move(e));
}

ostream& operator<<(ostream& os, Expression& expr) {
    os << expr.to_string();
    return os;
}
