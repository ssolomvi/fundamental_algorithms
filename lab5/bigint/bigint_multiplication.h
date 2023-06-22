#ifndef BIGINT_MULTIPLICATION_H
#define BIGINT_MULTIPLICATION_H

#include "bigint_impl.h"

class bigint_multiplication {
public:
    virtual bigint *multiply(bigint const * const left_multiplier,
                             bigint const * const right_multiplier) const = 0;
};

class bigint_column_multiplication final
        : public bigint_multiplication
{
    bigint *multiply(bigint const * const left_multiplier,
                     bigint const * const right_multiplier) const override;
};

class bigint_karatsuba_multiplication final
        : public  bigint_multiplication
{
    bigint *multiply(bigint const *const left_multiplier,
                     bigint const * const right_multiplier) const override;
};

class bigint_schonhage_strassen_multiplication final
        : public bigint_multiplication
{
    bigint *multiply(bigint const * const left_multiplier,
                     bigint const * const right_multiplier) const override;
};

#endif //BIGINT_MULTIPLICATION_H
