#include <cmath>
#include <regex>
#include <string>
#include <vector>

const int BASE = 10;

int logarifm10(int number) {
    int result = 1;
    while (number > 10) {
        number /= 10;
        result += 1;
    }
    return result;
}

const int NUMBER_OF_ZEROS = logarifm10(BASE);

const std::regex LINE_FORMAT = std::regex("^0+");

struct bigint {
private:
    std::vector<unsigned int> numbers;

public:
    bigint(const unsigned int number_ = 0) {
        unsigned int number = number_;
        while (number >= BASE) {
            numbers.push_back(number % BASE);
            number /= BASE;
        }
        numbers.push_back(number);
    }

    explicit bigint(const std::string &line_) {
        std::string line = std::regex_replace(line_, LINE_FORMAT, "");
        if (line.empty()) {
            line = "0";
        }

        for (int i = static_cast<int>(line.size()); i > 0;
             i -= NUMBER_OF_ZEROS) {
            if (i < NUMBER_OF_ZEROS) {
                numbers.push_back(std::stoi(line.substr(0, i)));
                break;
            } else {
                numbers.push_back(
                    std::stoi(line.substr(i - NUMBER_OF_ZEROS, NUMBER_OF_ZEROS))
                );
            }
        }
    }

    [[nodiscard]] int int_size() const {
        return static_cast<int>(this->numbers.size());
    }

    [[nodiscard]] std::string to_string() const {
        std::stringstream result;
        const int size = int_size();
        for (int i = size - 1; i >= 0; i--) {
            std::string const line = std::to_string(numbers[i]);
            if (i + 1 != size) {
                std::string line_of_zeros;
                for (int j = 0;
                     j < (NUMBER_OF_ZEROS - static_cast<int>(line.size()));
                     j++) {
                    line_of_zeros += "0";
                }

                result << line_of_zeros + line;
            } else {
                result << line;
            }
        }
        return result.str();
    }

    explicit operator unsigned int() const {
        unsigned int result = 0;
        unsigned int current_base = 1;
        for (int i = 0; i < int_size(); i++) {
            result += numbers[i] * current_base;
            current_base *= BASE;
        }
        return result;
    }

    friend bool operator==(const bigint &a, const bigint &b);

    friend bool operator!=(const bigint &a, const bigint &b);

    friend bool operator>(const bigint &a, const bigint &b);

    friend bool operator>=(const bigint &a, const bigint &b);

    friend bool operator<(const bigint &a, const bigint &b);

    friend bool operator<=(const bigint &a, const bigint &b);

    friend bigint operator+(const bigint &a, const bigint &b);

    friend bigint &operator+=(bigint &a, const bigint &b);

    friend bigint operator-(const bigint &a, const bigint &b);

    friend bigint &operator-=(bigint &a, const bigint &b);

    friend bigint &operator--(bigint &a);

    friend bigint &operator++(bigint &a);

    friend bigint operator--(bigint &a, int);

    friend bigint operator++(bigint &a, int);

    friend std::ostream &operator<<(std::ostream &out, const bigint &a);
};

bool operator==(const bigint &a, const bigint &b) {
    return (a.numbers == b.numbers);
}

bool operator!=(const bigint &a, const bigint &b) {
    return !(a == b);
}

bool operator>(const bigint &a, const bigint &b) {
    if (a.numbers.size() != b.numbers.size()) {
        return a.numbers.size() > b.numbers.size();
    }
    for (int i = a.int_size() - 1; i >= 0; i--) {
        if (a.numbers[i] > b.numbers[i]) {
            return true;
        }
        if (a.numbers[i] < b.numbers[i]) {
            return false;
        }
    }
    return false;
}

bool operator<(const bigint &a, const bigint &b) {
    if (a.numbers.size() != b.numbers.size()) {
        return a.numbers.size() < b.numbers.size();
    }
    for (int i = a.int_size() - 1; i >= 0; i--) {
        if (a.numbers[i] < b.numbers[i]) {
            return true;
        }
        if (a.numbers[i] > b.numbers[i]) {
            return false;
        }
    }
    return false;
}

bool operator>=(const bigint &a, const bigint &b) {
    return !(a < b);
}

bool operator<=(const bigint &a, const bigint &b) {
    return !(a > b);
}

bigint &operator+=(bigint &a, const bigint &b) {
    int flag = 0;
    // cppcheck-suppress unreadVariable
    unsigned int summ = 0;
    int len_a = a.int_size();
    const int len_b = b.int_size();
    if (len_b > len_a) {
        std::vector<int> vector_of_zeros(len_b - len_a, 0);
        a.numbers.insert(
            std::end(a.numbers), std::begin(vector_of_zeros),
            std::end(vector_of_zeros)
        );
    }
    len_a = a.int_size();
    for (int i = 0; i < len_a; i++) {
        if (i < len_b) {
            summ = (a.numbers[i] + b.numbers[i]) + flag;
        } else {
            summ = a.numbers[i] + flag;
        }
        flag = static_cast<int>(summ) / BASE;
        a.numbers[i] = (summ % BASE);
    }
    if (flag != 0) {
        a.numbers.push_back(flag);
    }
    return a;
}

bigint operator+(const bigint &a, const bigint &b) {
    bigint result = a;
    result += b;
    return result;
}

bigint &operator++(bigint &a) {
    a += bigint(1);
    return a;
}

bigint operator++(bigint &a, int) {
    bigint result = a;
    ++a;
    return result;
}

bigint &operator-=(bigint &a, const bigint &b) {
    int flag = 0;
    // cppcheck-suppress unreadVariable
    int dif = 0;
    const int len_a = a.int_size();
    const int len_b = b.int_size();
    for (int i = 0; i < len_a; i++) {
        if (i < len_b) {
            dif = static_cast<int>(a.numbers[i]) -
                  static_cast<int>(b.numbers[i]) - flag;
        } else {
            dif = static_cast<int>(a.numbers[i]) - flag;
        }
        if (dif >= 0) {
            a.numbers[i] = dif;
            flag = 0;
        } else {
            flag = 1;
            a.numbers[i] = BASE + dif;
        }
    }
    int i = len_a - 1;
    while (a.numbers[i] == 0 && i > 0) {
        a.numbers.pop_back();
        i -= 1;
    }

    return a;
}

bigint operator-(const bigint &a, const bigint &b) {
    bigint result = a;
    result -= b;
    return result;
}

bigint &operator--(bigint &a) {
    a -= bigint(1);
    return a;
}

bigint operator--(bigint &a, int) {
    bigint result = a;
    --a;
    return result;
}

std::ostream &operator<<(std::ostream &out, const bigint &a) {
    out << a.to_string();
    return out;
}

std::istream &operator>>(std::istream &in, bigint &a) {
    std::string line;
    in >> line;
    a = bigint(line);
    return in;
}
