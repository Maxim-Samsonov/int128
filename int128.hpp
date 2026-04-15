#pragma once
#include <string>
#include <iostream>
#include <cstdint>
#include <algorithm>

using namespace std;

class Int128 {
public:
    uint64_t low;
    uint64_t high;

    Int128() {
        low = 0;
        high = 0;
    }

    Int128(int64_t value) {
        if (value >= 0) {
            low = value;
            high = 0;
        }
        else {
            int64_t abs_val = -value;
            low = ~abs_val + 1;
            high = 0xFFFFFFFFFFFFFFFF;
        }
    }

    Int128(string str) {
        bool minus = false;
        int start = 0;

        if (str[0] == '-') {
            minus = true;
            start = 1;
        }

        low = 0;
        high = 0;

        for (int i = start; i < str.length(); i++) {
            int digit = str[i] - '0';

            Int128 temp = *this;
            for (int j = 0; j < 9; j++) {
                *this = *this + temp;
            }

            *this = *this + Int128((int64_t)digit);
        }

        if (minus) {
            *this = -*this;
        }
    }

    operator int64_t() {
        if (high == 0 && low <= 9223372036854775807) {
            return (int64_t)low;
        }
        if (high == 0xFFFFFFFFFFFFFFFF) {
            return -(int64_t)(~low + 1);
        }
        throw "Number too big for int64_t";
    }

    operator double() {
        string s = str();
        double result = 0;
        bool minus = false;

        if (s[0] == '-') {
            minus = true;
            s = s.substr(1);
        }

        for (int i = 0; i < s.length(); i++) {
            result = result * 10 + (s[i] - '0');
        }

        if (minus) result = -result;
        return result;
    }

    string str() {
        if (low == 0 && high == 0) {
            return "0";
        }

        Int128 temp = *this;
        bool minus = false;

        if (high & 0x8000000000000000) {
            minus = true;
            temp = -temp;
        }

        string result = "";

        while (!(temp.low == 0 && temp.high == 0)) {
            Int128 rem = temp;
            Int128 div_result;

            uint64_t remainder = 0;
            for (int i = 63; i >= 0; i--) {
                remainder = (remainder << 1) | ((temp.high >> i) & 1);
                if (remainder >= 10) {
                    div_result.high |= (1ULL << i);
                    remainder -= 10;
                }
            }
            for (int i = 63; i >= 0; i--) {
                remainder = (remainder << 1) | ((temp.low >> i) & 1);
                if (remainder >= 10) {
                    div_result.low |= (1ULL << i);
                    remainder -= 10;
                }
            }

            result += (char)('0' + remainder);
            temp = div_result;
        }

        if (minus) {
            result += '-';
        }

        reverse(result.begin(), result.end());
        return result;
    }

    Int128 operator-() {
        Int128 result;

        result.low = ~low;
        result.high = ~high;

        result.low++;
        if (result.low == 0) {
            result.high++;
        }

        return result;
    }

    Int128 operator+(Int128 other) {
        Int128 result;
        result.low = low + other.low;
        result.high = high + other.high;

        if (result.low < low) {
            result.high++;
        }

        return result;
    }

    Int128 operator-(Int128 other) {
        return *this + (-other);
    }

    Int128 operator*(Int128 other) {
        if ((low == 0 && high == 0) || (other.low == 0 && other.high == 0)) {
            return Int128(0);
        }

        bool sign1 = (high & 0x8000000000000000) != 0;
        bool sign2 = (other.high & 0x8000000000000000) != 0;
        bool result_sign = sign1 != sign2;

        Int128 a = sign1 ? -*this : *this;
        Int128 b = sign2 ? -other : other;

        Int128 result(0);

        while (!(b.low == 0 && b.high == 0)) {
            if (b.low & 1) {
                result = result + a;
            }
            a = a + a;
            b.low = (b.low >> 1) | (b.high << 63);
            b.high = b.high >> 1;
        }

        if (result_sign) {
            result = -result;
        }

        return result;
    }

    Int128 operator/(Int128 other) {
        if (other.low == 0 && other.high == 0) {
            throw "Division by zero!";
        }

        bool sign1 = (high & 0x8000000000000000) != 0;
        bool sign2 = (other.high & 0x8000000000000000) != 0;
        bool result_sign = sign1 != sign2;

        Int128 a = sign1 ? -*this : *this;
        Int128 b = sign2 ? -other : other;

        Int128 result(0);
        Int128 current(0);

        for (int i = 127; i >= 0; i--) {
            current.low = (current.low << 1) | (current.high >> 63);
            current.high = current.high << 1;

            if (i >= 64) {
                if ((a.high >> (i - 64)) & 1) {
                    current.low |= 1;
                }
            }
            else {
                if ((a.low >> i) & 1) {
                    current.low |= 1;
                }
            }

            if (current.high > b.high ||
                (current.high == b.high && current.low >= b.low)) {

                uint64_t old_low = current.low;
                current.low = current.low - b.low;
                current.high = current.high - b.high;
                if (current.low > old_low) {
                    current.high--;
                }

                if (i >= 64) {
                    result.high |= (1ULL << (i - 64));
                }
                else {
                    result.low |= (1ULL << i);
                }
            }
        }

        if (result_sign) {
            result = -result;
        }

        return result;
    }

    bool operator==(Int128 other) {
        return low == other.low && high == other.high;
    }

    bool operator!=(Int128 other) {
        return !(*this == other);
    }

    bool operator<(Int128 other) {
        bool sign1 = (high & 0x8000000000000000) != 0;
        bool sign2 = (other.high & 0x8000000000000000) != 0;

        if (sign1 != sign2) {
            return sign1;
        }

        if (high != other.high) {
            return sign1 ? high > other.high : high < other.high;
        }

        return sign1 ? low > other.low : low < other.low;
    }

    bool operator>(Int128 other) {
        return other < *this;
    }

    bool operator<=(Int128 other) {
        return *this < other || *this == other;
    }

    bool operator>=(Int128 other) {
        return *this > other || *this == other;
    }

    Int128& operator+=(Int128 other) {
        *this = *this + other;
        return *this;
    }

    Int128& operator-=(Int128 other) {
        *this = *this - other;
        return *this;
    }

    Int128& operator*=(Int128 other) {
        *this = *this * other;
        return *this;
    }

    Int128& operator/=(Int128 other) {
        *this = *this / other;
        return *this;
    }
};

ostream& operator<<(ostream& os, Int128 num) {
    os << num.str();
    return os;
}
