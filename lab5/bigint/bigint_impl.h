#ifndef BIGINT_IMPL_H
#define BIGINT_IMPL_H

#include "bigint.h"

class bigint_multiplication;
class bigint_division;

class bigint_impl final : public bigint {
#pragma region add-subtract
public:
    bigint *add(bigint const *const summand) override;

    virtual bigint *sum(bigint const *const summand) const override {
        // todo: ask if this is a new bigint*
        bigint_impl *to_return = new bigint_impl();
        (*to_return) = (*this);
        to_return->add(summand);
        return reinterpret_cast<bigint *>(to_return);
    }

    virtual bigint *subtraction(bigint const *const subtrahend) const override {
        bigint_impl *to_return = new bigint_impl();
        (*to_return) = (*this);
        to_return->subtract(subtrahend);
        return to_return;
    }

    bigint *subtract(bigint const *const subtrahend) override;

#pragma endregion

#pragma region logic operators

    bool lower_than(bigint const & other) const override;

    bool greater_than(bigint const & other) const override;

    bool lower_than_or_equal_to(bigint const & other) const override;

    bool greater_than_or_equal_to(bigint const & other) const override;

    bool equals(bigint const &other) const override;

    bool not_equals(bigint const &other) const override;

#pragma endregion

public:
    bool split(bigint_impl *bi_front, bigint_impl *bi_back, size_t split_digit) const;

    void init();

    std::string bi_to_string();

    friend std::ostream &operator<<(std::ostream &out, bigint_impl const &value);

    friend std::istream &operator>>(std::istream &in, bigint_impl **value);

    [[nodiscard]] bigint *make_a_copy() const override
    {
        bigint_impl * to_return = new bigint_impl();
        (*to_return) = *(const_cast<bigint_impl *>(this));
        return reinterpret_cast<bigint *>(to_return);
    }
#pragma region rule 5
public:
    explicit bigint_impl(size_t number, logger * logger = nullptr, memory * allocator = nullptr)
        : bigint(logger, allocator)
    {
        _count_of_digits = 1;
        size_t quotient = number / INT_MAX;
        _first_digit = number % INT_MAX;

        if (quotient != 0) {
            if (quotient < INT_MAX) {
                _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned)));
                _digits[0] = quotient;
                _count_of_digits = 2;
            }
            else {
                _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * 2));
                _digits[0] = quotient % INT_MAX;
                _digits[1] = quotient / INT_MAX;
                _count_of_digits = 3;
            }
        }
    };

    explicit bigint_impl(int number, logger * logger = nullptr, memory * allocator = nullptr)
            : bigint(logger, allocator)
    {
        _first_digit = number;
        _count_of_digits++;
    }

    explicit bigint_impl(std::string & from, const bigint_multiplication *const multiplication_impl, logger * logger = nullptr, memory * allocator = nullptr);

    explicit bigint_impl(logger * logger = nullptr, memory * allocator = nullptr)
    : bigint(logger, allocator)
    {

    }

    // copy constructor
    bigint_impl(bigint_impl const &obj)
    : bigint_impl(obj._logger, obj._allocator)
    {
        if (obj._count_of_digits > 1) {
            _digits = reinterpret_cast<unsigned *>(this->allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
            memcpy(_digits, obj._digits, (obj._count_of_digits - 1) * sizeof(unsigned));
        }

        _count_of_digits = obj._count_of_digits;
        _first_digit = obj._first_digit;
    }

//    explicit bigint_impl(int const &obj)
//    : bigint_impl()
//    {
//        _count_of_digits++;
//        _first_digit = obj;
//    }

    // move constructor
    bigint_impl(bigint_impl &&obj) noexcept
    : bigint(obj._logger, obj._allocator)
    {
        if (obj._count_of_digits != 1) {
            _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
            memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
            deallocate_with_guard(obj._digits);
        }

        _count_of_digits = obj._count_of_digits;
        obj._count_of_digits = 0;

        _first_digit = obj._first_digit;
        obj._first_digit = 0;

        obj._logger = nullptr;
        obj._allocator = nullptr;
    }

    // copy assignment (оператор присваивания)
    bigint_impl &operator=(bigint_impl const &obj)
    {
        if (this == &obj)
        {
            return *this;
        }

        if (_count_of_digits > 1) {
            deallocate_with_guard(_digits);
        }

        _allocator = obj._allocator;

        if (obj._count_of_digits > 1) {
            _digits = reinterpret_cast<unsigned *>(::operator new(sizeof(unsigned) * (obj._count_of_digits - 1)));
            // todo: ask Ilya
//            _digits = reinterpret_cast<unsigned *>(this->allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
            memcpy(_digits, obj._digits, (obj._count_of_digits - 1) * sizeof(unsigned));
        }

        _count_of_digits = obj._count_of_digits;

        _first_digit = obj._first_digit;

        _logger = obj._logger;
        return *this;
    }

    bigint_impl &operator=(int const &obj)
    {
        if (this->_first_digit == obj && _digits == nullptr) {
            return *this;
        }

        _count_of_digits = 1;
        _first_digit = obj;
        if (_digits) {
            deallocate_with_guard(_digits);
        }
        return *this;
    }

    // move assignment (оператор присваивания перемещением)
    bigint_impl &operator=(bigint_impl &&obj) noexcept
    {
        if (_count_of_digits > 1) {
            deallocate_with_guard(_digits);
        }
        _allocator = obj._allocator;
        obj._allocator = nullptr;
        if (obj._count_of_digits > 1) {
            _digits = reinterpret_cast<unsigned *>(allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
            memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
            deallocate_with_guard(obj._digits);
        }

        _count_of_digits = obj._count_of_digits;
        obj._count_of_digits = 0;

        _first_digit = obj._first_digit;
        obj._first_digit = 0;

        _logger = obj._logger;
        obj._logger = nullptr;
        return *this;
    }

    ~bigint_impl() override = default;

#pragma endregion

};

#endif //BIGINT_IMPL_H
