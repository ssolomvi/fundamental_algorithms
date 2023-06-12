#ifndef BIGINT_H
#define BIGINT_H

#include "../logger/logger_holder.h"
#include "../allocator/memory_holder.h"

class bigint
        : protected logger_holder,
          protected memory_holder
{
public:
#pragma region multiplication
    class bigint_multiplication {
        virtual bigint *multiply(bigint const * const left_multiplier,
                                 bigint const * const right_multiplier) const = 0;
    };
    friend class bigint_multiplication;
#pragma endregion

#pragma region division
    class bigint_division {
        virtual bigint* divide(bigint const * const dividend, bigint const * const divider,
                               bigint_multiplication const * const multiplication_impl) const = 0;
    };
    friend class bigint_division;
#pragma endregion

public:
    class parameter_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit parameter_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }
    };

protected:
    size_t _count_of_digits;
    int _first_digit; // has sign in it
    unsigned int *_digits;     // extra digits, begins with last number digit, ends with the greatest number digit
    bigint_multiplication * _multiplication;
    bigint_division * _division;
    logger * _logger;
    memory * _allocator;

#pragma region summing

public:
    bigint_multiplication * get_multiplication()
    {
        return _multiplication;
    };

    bigint_division * get_division()
    {
        return _division;
    }

    bigint_multiplication * set_multiplication(bigint_multiplication * new_multiplication)
    {
        auto * tmp = _multiplication;
        _multiplication = new_multiplication;
        return tmp;
    }

    bigint_division * set_division(bigint_division * new_division)
    {
        auto * tmp = _division;
        _division = new_division;
        return tmp;
    }

    size_t get_count_of_digits() const {
        return _count_of_digits;
    }

    size_t * get_ptr_count_of_digits() {
        return &_count_of_digits;
    }

    bool get_sign() const
    {
        return ((this->_first_digit) >> (sizeof(this->_first_digit) * 8 - 1));
    }

    void change_sign() const
    {
        ~(this->_first_digit) + 1;
    }

    int * get_ptr_last_digit()
    {
        return &_first_digit;
    }

    unsigned int * get_ptr_digit_with_index(unsigned index) {
        return &(_digits[index]);
    }

    void reallocate_digits_array(size_t new_count_of_digits)
    {
        if (new_count_of_digits == 0) {
            deallocate_with_guard(_digits);
            _digits = nullptr;
            return;
        }

        if (new_count_of_digits == _count_of_digits) {
            return;
        }

        unsigned * new_digits_ptr = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (new_count_of_digits - 1)));
        unsigned count_of_digits_in_array = _count_of_digits - 1;
        if (_count_of_digits != 1) {
            if (new_count_of_digits > _count_of_digits) {
                memcpy(new_digits_ptr, _digits, sizeof(unsigned) * count_of_digits_in_array);
            }
            else {
                memcpy(new_digits_ptr, _digits, sizeof(unsigned) * (new_count_of_digits - 1));
            }
        }

        if (new_count_of_digits > count_of_digits_in_array) {
            unsigned i = count_of_digits_in_array;
            for (i; i < new_count_of_digits; i++) {
                new_digits_ptr[i] = 0;
            }
        }

        deallocate_with_guard(_digits);
        _digits = new_digits_ptr;
    }

public:
    // +=
    virtual bigint *add(bigint const * const summand) = 0;

    bigint *operator+=(bigint const * const summand)
    {
        return add(summand);
    }

public:
    // +
    virtual bigint *sum(bigint const * const summand) const
    {
        bigint *to_return{const_cast<bigint *>(this)};
        to_return->add(summand);
        return to_return;
    }

    bigint *operator+(bigint const * const summand) const
    {
        return sum(summand);
    }

#pragma endregion

#pragma region substracting
public:
    // -=
    virtual bigint *subtract(bigint const * const subtrahend) = 0;

    bigint *operator-=(bigint const * const subtrahend)
    {
        return subtract(subtrahend);
    }

public:
    // -
    virtual bigint *subtraction(bigint const * const subtrahend) const
    {
        bigint *to_return{const_cast<bigint *>(this)};
        to_return->subtract(subtrahend);
        return to_return;
    }

    bigint *operator-(bigint const * const subtrahend) const
    {
        return subtraction(subtrahend);
    }
#pragma endregion

#pragma region logic operators
public:
    // <
    virtual bool lower_than(bigint const * const other) const = 0;

    bool operator<(bigint const * const other) const
    {
        return lower_than(other);
    }

public:
    // >
    virtual bool greater_than(bigint const * const other) const = 0;
    bool operator>(bigint const * const other) const
    {
        return greater_than(other);
    }

public:
    // <=
    virtual bool lower_than_or_equal_to(bigint const * const other) const = 0;

    bool operator<=(bigint const * const other) const
    {
        return lower_than_or_equal_to(other);
    }

public:
    // >=
    virtual bool greater_than_or_equal_to(bigint const * const other) const = 0;

    bool operator>=(bigint const * const other) const
    {
        return greater_than_or_equal_to(other);
    }

public:
    // ==
    virtual bool equals(bigint const * const other) const = 0;

    bool operator==(bigint const * const other) const
    {
        return equals(other);
    }

public:
    // !=
    virtual bool not_equals(bigint const * const other) const = 0;

    bool operator!=(bigint const * const other) const
    {
        return not_equals(other);
    }
#pragma endregion

    friend std::ostream &operator<<(std::ostream &out, const bigint &value);

    friend std::istream &operator>>(std::istream &in, const bigint &value);

#pragma region rule 5
    explicit bigint(char * from, bigint_multiplication * multiplication = nullptr, bigint_division * division = nullptr, logger * logger = nullptr, memory * allocator = nullptr)
            : _logger(logger), _allocator(allocator), _digits(nullptr), _count_of_digits(0), _first_digit(0), _multiplication(multiplication), _division(division)
    {
        // todo:
    }

    explicit bigint(bigint_multiplication * multiplication = nullptr, bigint_division * division = nullptr, logger * logger = nullptr, memory * allocator = nullptr)
            : _logger(logger), _allocator(allocator), _digits(nullptr), _count_of_digits(0), _first_digit(0), _multiplication(multiplication), _division(division)
    {

    }

    // copy constructor
    bigint(bigint const &obj)
            : bigint(obj._multiplication, obj._division, obj._logger, obj._allocator)
    {
        _count_of_digits = obj._count_of_digits;
        _first_digit = obj._first_digit;
        _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
        memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
    }

    // move constructor
    bigint(bigint &&obj) noexcept
            : bigint(obj._multiplication, obj._division, obj._logger, obj._allocator)
    {
        _count_of_digits = obj._count_of_digits;
        obj._count_of_digits = 0;

        _first_digit = obj._first_digit;
        obj._first_digit = 0;

        _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
        memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
        deallocate_with_guard(obj._digits);

        obj._logger = nullptr;
        obj._allocator = nullptr;

        obj._multiplication = nullptr;
        obj._division = nullptr;
    }

    // copy assignment (оператор присваивания)
    bigint &operator=(bigint const &obj)
    {
        if (this == &obj)
        {
            return *this;
        }

        _count_of_digits = obj._count_of_digits;

        _first_digit = obj._first_digit;

        deallocate_with_guard(_digits);
        _allocator = obj._allocator;
        _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
        memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));

        _logger = obj._logger;
        return *this;
    }

    // move assignment (оператор присваивания перемещением)
    bigint &operator=(bigint &&obj) noexcept
    {
        _count_of_digits = obj._count_of_digits;
        obj._count_of_digits = 0;

        _first_digit = obj._first_digit;
        obj._first_digit = 0;

        deallocate_with_guard(_digits);
        _allocator = obj._allocator;
        obj._allocator = nullptr;
        _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
        memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
        deallocate_with_guard(obj._digits);

        _logger = obj._logger;
        obj._logger = nullptr;
        return *this;
    }

    ~bigint() override {
        _count_of_digits = 0;
        _first_digit = 0;
        deallocate_with_guard(_digits);
    }

#pragma endregion

#pragma region memory and logger holder contracts
public:
    memory *get_memory() const noexcept override
    {
        return _allocator;
    }

    logger *get_logger() const noexcept override
    {
        return _logger;
    }
#pragma endregion
};

inline std::ostream &operator<<(std::ostream &out, const bigint &value) {
    return out;
}

inline std::istream &operator>>(std::istream &in, const bigint &value) {
    return in;
}

#endif //BIGINT_H
