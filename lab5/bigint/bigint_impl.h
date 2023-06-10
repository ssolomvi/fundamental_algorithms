#ifndef BIGINT_IMPL_H
#define BIGINT_IMPL_H

#include "bigint.h"

class bigint_impl : public bigint
{
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
};

#endif //BIGINT_IMPL_H
