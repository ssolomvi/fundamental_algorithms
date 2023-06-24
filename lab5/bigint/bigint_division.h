#ifndef BIGINT_DIVISION_H
#define BIGINT_DIVISION_H

#include "bigint.h"
#include "bigint_multiplication.h"

class bigint_division {
public:
    class div_by_zero_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit div_by_zero_exception(std::string message)
                : _message(std::move(message)) {

        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }
    };

    virtual bigint* divide(bigint const * const dividend, bigint const * const divider,
                           bigint_multiplication const * const multiplication_impl) const = 0;
};

//class bigint_newton_division final
//        : public bigint_division
//{
//    bigint* divide(bigint const * const dividend, bigint const * const divider,
//                   bigint_multiplication const * const multiplication_impl) const override {return nullptr;}
//};

class bigint_burnikel_ziegler_division final
        : public  bigint_division
{
public:
    bigint* divide(bigint const * const dividend, bigint const * const divider,
                   bigint_multiplication const * const multiplication_impl) const override;

    void split_for_AHigh_ALow(bigint_impl * to_split, bigint_impl ** AH, bigint_impl ** AL, size_t half_size) const;

    void fix_reminder(int multiplier, bigint_impl * B, bigint_impl ** remainder, bigint_impl ** quotient, bigint_multiplication const * const multiplication_impl) const;
    // return quotient and reminder
    std::pair<bigint_impl *, bigint_impl *> divide_with_remainder
    (bigint const * const dividend, bigint const * const divider, bigint_multiplication const * const multiplication_impl) const;

private:
    std::pair<bigint_impl *, bigint_impl *> div_two_digits_by_one(bigint_impl * AHigh, bigint_impl * ALow, bigint_impl * B, bigint_multiplication const * const multiplication_impl) const;

    std::pair<bigint_impl *, bigint_impl *> div_three_halves_by_two(bigint_impl * a1, bigint_impl * a2, bigint_impl * a3, bigint_impl * b1, bigint_impl * b2, bigint_multiplication const * const multiplication_impl) const;
};

#endif //BIGINT_DIVISION_H
