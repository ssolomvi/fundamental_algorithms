#ifndef BIGINT_H
#define BIGINT_H

#include "../logger/logger_holder.h"
#include "../allocator/memory_holder.h"
#include <cmath>
#include <stack>

class bigint
        : protected logger_holder,
          protected memory_holder
{
public:
    class parameter_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit parameter_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

protected:
    size_t _count_of_digits;
    int _first_digit; // has sign in it
    unsigned int *_digits;     // extra digits, begins with last number digit, ends with the greatest number digit
    logger * _logger;
    memory * _allocator;

#pragma region summing

public:
    [[nodiscard]] size_t get_count_of_digits() const {
        return _count_of_digits;
    }

    size_t * get_ptr_count_of_digits() {
        return &_count_of_digits;
    }

    [[nodiscard]] bool get_sign() const
    {
        return ((this->_first_digit) >> (sizeof(this->_first_digit) * 8 - 1));
    }

    void change_sign()
    {
        _first_digit = ~(this->_first_digit) + 1;
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
        if (new_count_of_digits == 0 && _count_of_digits > 1) {
            deallocate_with_guard(_digits);
            _digits = nullptr;
            return;
        }

        if (new_count_of_digits == _count_of_digits) {
            return;
        }

        unsigned * new_digits_ptr = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (new_count_of_digits - 1)));
        unsigned count_of_digits_in_array = (_count_of_digits > 0 ?
                _count_of_digits - 1 : 0);
        if (_count_of_digits > 1) {
            if (new_count_of_digits > _count_of_digits) {
                memcpy(new_digits_ptr, _digits, sizeof(unsigned) * count_of_digits_in_array);
            }
            else {
                memcpy(new_digits_ptr, _digits, sizeof(unsigned) * (new_count_of_digits - 1));
            }
        }

        if (new_count_of_digits > count_of_digits_in_array) {
            unsigned i;
            for (i = count_of_digits_in_array; i < (new_count_of_digits - 1); i++) {
                new_digits_ptr[i] = 0;
            }
        }

        deallocate_with_guard(_digits);
        _digits = new_digits_ptr;
    }

    void mult_by_pow_base(size_t power) {
        // shift digits to right by power, making new digits == 0
        // we believe that this number is positive
        if (power <= 0) {
            return;
        }

        size_t tmp = power - 1;

        unsigned * new_digits = reinterpret_cast<unsigned *>(allocate_with_guard(_count_of_digits + tmp));
        new_digits[tmp] = _first_digit;
        if (_digits != nullptr) {
            memcpy(new_digits + power, _digits, sizeof(unsigned) * _count_of_digits - 1);
            deallocate_with_guard(_digits);
        }

        _first_digit = 0;
        unsigned i;
        for (i = 0; i < tmp; i++) {
            _digits[i] = 0;
        }
        _digits = new_digits;
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
    virtual bigint *sum(bigint const * const summand) const = 0;

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
    virtual bigint *subtraction(bigint const * const subtrahend) const = 0;

    bigint *operator-(bigint const * const subtrahend) const
    {
        return subtraction(subtrahend);
    }
#pragma endregion

#pragma region logic operators
public:
    // <
    virtual bool lower_than(bigint const & other) const = 0;

    bool operator<(bigint const & other) const
    {
        return lower_than(other);
    }

public:
    // >
    virtual bool greater_than(bigint const & other) const = 0;
    bool operator>(bigint const & other) const
    {
        return greater_than(other);
    }

public:
    // <=
    virtual bool lower_than_or_equal_to(bigint const & other) const = 0;

    bool operator<=(bigint const & other) const
    {
        return lower_than_or_equal_to(other);
    }

public:
    // >=
    virtual bool greater_than_or_equal_to(bigint const & other) const = 0;

    bool operator>=(bigint const & other) const
    {
        return greater_than_or_equal_to(other);
    }

public:
    // ==
    virtual bool equals(bigint const & other) const = 0;

    bool operator==(bigint const & other) const
    {
        return equals(other);
    }

public:
    // !=
    virtual bool not_equals(bigint const & other) const = 0;

    bool operator!=(bigint const & other) const
    {
        return not_equals(other);
    }
#pragma endregion

public:
    [[nodiscard]] virtual bigint * make_a_copy() const = 0;

#pragma region rule 5
    explicit bigint(std::string & from, logger * logger = nullptr, memory * allocator = nullptr)
            : _logger(logger), _allocator(allocator), _digits(nullptr), _count_of_digits(0), _first_digit(0)
    {

    }

    explicit bigint(logger * logger = nullptr, memory * allocator = nullptr)
            : _logger(logger), _allocator(allocator), _digits(nullptr), _count_of_digits(0), _first_digit(0)
    {

    }

    // copy constructor
    bigint(bigint const &obj)
            : bigint(obj._logger, obj._allocator)
    {
        _count_of_digits = obj._count_of_digits;
        _first_digit = obj._first_digit;
        _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
        memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
    }

    // move constructor
    bigint(bigint &&obj) noexcept
            : bigint(obj._logger, obj._allocator)
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
        if (_digits != nullptr) {
            deallocate_with_guard(_digits);
        }
    }

#pragma endregion

#pragma region memory and logger holder contracts
protected:
    [[nodiscard]] memory *get_memory() const noexcept override
    {
        return _allocator;
    }

    [[nodiscard]] logger *get_logger() const noexcept override
    {
        return _logger;
    }
#pragma endregion
};

#endif //BIGINT_H
