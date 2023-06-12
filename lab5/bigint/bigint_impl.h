#ifndef BIGINT_IMPL_H
#define BIGINT_IMPL_H

#include "bigint.h"

class bigint_impl final : public bigint
{
#pragma region multiplication
    class bigint_column_multiplication final
            : public bigint_multiplication
    {
        bigint *multiply(bigint const * const left_multiplier,
                         bigint const * const right_multiplier) const override;
    };

    class bigint_karatsuba_multiplication final
            : public  bigint_multiplication
    {
        bigint *multiply(bigint const * const left_multiplier,
                         bigint const * const right_multiplier) const override {return nullptr;}
    };

    class bigint_schonhage_strassen_multiplication final
            : public bigint_multiplication
    {
        bigint *multiply(bigint const * const left_multiplier,
                         bigint const * const right_multiplier) const override {return nullptr;}
    };
#pragma endregion

#pragma region division
    class bigint_newton_division final
            : public bigint_division
    {
        bigint* divide(bigint const * const dividend, bigint const * const divider,
                       bigint_multiplication const * const multiplication_impl) const override {return nullptr;}
    };

    class bigint_burnikel_ziegler_division final
            : public  bigint_division
    {
        bigint* divide(bigint const * const dividend, bigint const * const divider,
                       bigint_multiplication const * const multiplication_impl) const override {return nullptr;}
    };
#pragma endregion


#pragma region add-subtract
    bigint *add(bigint const * const summand) override;

    bigint *subtract(bigint const * const subtrahend) override;

#pragma endregion

#pragma region logic operators

    bool lower_than(bigint const * const other) const override;
    bool greater_than(bigint const * const other) const override;
    bool lower_than_or_equal_to(bigint const * const other) const override;
    bool greater_than_or_equal_to(bigint const * const other) const override;
    bool equals(bigint const * const other) const override;
    bool not_equals(bigint const * const other) const override;

#pragma endregion

#pragma region rule 5
    explicit bigint_impl(char * from, bigint_multiplication * multiplication = nullptr, bigint_division * division = nullptr, logger * logger = nullptr, memory * allocator = nullptr)
    : bigint(from, multiplication, division, logger, allocator)
    {
        // todo:
    }

    explicit bigint_impl(bigint_multiplication * multiplication = nullptr, bigint_division * division = nullptr, logger * logger = nullptr, memory * allocator = nullptr)
    : bigint(multiplication, division, logger, allocator)
    {

    }

    // copy constructor
    bigint_impl(bigint_impl const &obj)
    : bigint_impl(obj._multiplication, obj._division, obj._logger, obj._allocator)
    {
        _count_of_digits = obj._count_of_digits;
        _first_digit = obj._first_digit;
        _digits = reinterpret_cast<unsigned *>(this->allocate_with_guard(sizeof(unsigned) * (obj._count_of_digits - 1)));
        memcpy(_digits, obj._digits, (_count_of_digits - 1) * sizeof(unsigned));
    }

    // move constructor
    bigint_impl(bigint_impl &&obj) noexcept
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
    bigint_impl &operator=(bigint_impl const &obj)
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
    bigint_impl &operator=(bigint_impl &&obj) noexcept
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

    ~bigint_impl() override {
        _count_of_digits = 0;
        _first_digit = 0;
        deallocate_with_guard(_digits);
    }

#pragma endregion

};

#endif //BIGINT_IMPL_H
