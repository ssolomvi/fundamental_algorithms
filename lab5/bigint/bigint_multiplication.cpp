#include "bigint_multiplication.h"

#pragma region multiplication
bigint *bigint_column_multiplication::multiply(const bigint *const left_multiplier,
                                               const bigint *const right_multiplier) const
{
    if (!left_multiplier || !right_multiplier) {
        return nullptr;
    }

    auto * left = const_cast<bigint *>(left_multiplier), * right = const_cast<bigint *>(right_multiplier);
    bool left_sign = left->get_sign(), right_sign = right->get_sign();
    bool result_sign = (left_sign + right_sign) % 2;

    if (left_sign) {
        left->change_sign();
    }

    if (right_sign) {
        right->change_sign();
    }

    bigint * multiplying_result = new bigint_impl();
    multiplying_result->reallocate_digits_array((left->get_count_of_digits() + right->get_count_of_digits()));

    unsigned iter_over_right_num, iter_over_left_num, iter_for_multiply;
    size_t sum_result = 0, additional = 0, left_len = left->get_count_of_digits() - 1, right_len = right->get_count_of_digits() - 1;
    long long radix = INT_MAX;
    int right_last_digit = (*(right->get_ptr_last_digit())), left_last_digit = (*(left->get_ptr_last_digit()));

    // multiplying "digits"
    if (right_last_digit != 0) {
        // do the first multiply
        // right number last digit multiply another last digit
        for (iter_for_multiply = 0; iter_for_multiply < right_last_digit; iter_for_multiply++) {
            sum_result += left_last_digit;
            if (sum_result >= radix) {
                ++additional;
                sum_result -= radix;
            }
        }
        (*(multiplying_result->get_ptr_last_digit())) = sum_result;
        (*(multiplying_result->get_ptr_count_of_digits())) = 1;

        // right number last digit multiply right number digit array
        for (iter_over_left_num = 0; iter_over_left_num < left_len; iter_over_left_num++) {
            sum_result = additional;
            additional = 0;

            unsigned left_digit = (*(left->get_ptr_digit_with_index(iter_over_left_num)));

            if (left_digit != 0) {
                for (iter_for_multiply = 0; iter_for_multiply < right_last_digit; iter_for_multiply++) {
                    sum_result += left_digit;
                    if (sum_result >= radix) {
                        ++additional;
                        sum_result -= radix;
                    }
                }
            }

            (*(multiplying_result->get_ptr_digit_with_index(iter_over_left_num))) = sum_result;
            ++(*(multiplying_result->get_ptr_count_of_digits()));
        }

        if (additional) {
            (*(multiplying_result->get_ptr_digit_with_index(iter_over_left_num))) = additional;
            ++(*(multiplying_result->get_ptr_count_of_digits()));
            additional = 0;
        }
    }

    sum_result = 0;

    for (iter_over_right_num = 0; iter_over_right_num < right_len; iter_over_right_num++) {
        // do the first multiply
        // right number last digit multiply another last digit
        unsigned right_digit = (*(right->get_ptr_digit_with_index(iter_over_right_num)));
        if (right_digit == 0) {
            continue;
        }

        for (iter_for_multiply = 0; iter_for_multiply < right_digit; iter_for_multiply++) {
            sum_result += left_last_digit;
            if (sum_result >= radix) {
                ++additional;
                sum_result -= radix;
            }
        }

        if ((iter_over_right_num + 1) > (multiplying_result->get_count_of_digits()) - 1) {
            (*(multiplying_result->get_ptr_count_of_digits())) = iter_over_right_num + 2;
        }

        sum_result += (*(multiplying_result->get_ptr_digit_with_index(iter_over_right_num)));
        if (sum_result >= radix) {
            ++additional;
            sum_result -= radix;
        }
        (*(multiplying_result->get_ptr_digit_with_index(iter_over_right_num))) = (sum_result);

        // right number last digit multiply right number digit array
        for (iter_over_left_num = 0; iter_over_left_num < left_len; iter_over_left_num++) {
            sum_result = additional;
            additional = 0;

            unsigned left_digit = (*(left->get_ptr_digit_with_index(iter_over_left_num)));

            if (left_digit != 0) {
                for (iter_for_multiply = 0; iter_for_multiply < right_last_digit; iter_for_multiply++) {
                    sum_result += left_digit;
                    if (sum_result >= radix) {
                        ++additional;
                        sum_result -= radix;
                    }
                }
            }

            if ((iter_over_right_num + iter_over_left_num + 2) > (multiplying_result->get_count_of_digits() - 1)) {
                (*(multiplying_result->get_ptr_count_of_digits())) = (iter_over_right_num + iter_over_left_num + 3);
            }

            sum_result += (*(multiplying_result->get_ptr_digit_with_index(iter_over_right_num + iter_over_left_num + 1)));
            if (sum_result >= radix) {
                ++additional;
                sum_result -= radix;
            }
            (*(multiplying_result->get_ptr_digit_with_index(iter_over_right_num + iter_over_left_num + 1))) = sum_result;
        }

        if (additional) {
            (*(multiplying_result->get_ptr_digit_with_index(iter_over_left_num + iter_over_left_num + 1))) = additional;
            ++(*(multiplying_result->get_ptr_count_of_digits()));
            additional = 0;
        }
    }

    multiplying_result->reallocate_digits_array(multiplying_result->get_count_of_digits());

    if (left_sign) {
        left->change_sign();
    }

    if (right_sign) {
        right->change_sign();
    }

    if (result_sign) {
        multiplying_result->change_sign();
    }

    return multiplying_result;
}

bigint *bigint_karatsuba_multiplication::multiply(const bigint *const left_multiplier,
                                                  const bigint *const right_multiplier) const
{
    if (left_multiplier == nullptr || right_multiplier == nullptr) {
        return nullptr;
    }

    size_t left_m_size = left_multiplier->get_count_of_digits(), right_m_size = right_multiplier->get_count_of_digits();

    bigint_impl * left_m = reinterpret_cast<bigint_impl *>(const_cast<bigint *>(left_multiplier));
    bigint_impl * right_m = reinterpret_cast<bigint_impl *>(const_cast<bigint *>(right_multiplier));

    if (left_m_size == 0 || right_m_size == 0
    || (left_m_size == 1 && (*(left_m->get_ptr_last_digit())) == 0)
    || (right_m_size == 1 && (*(right_m->get_ptr_last_digit())) == 0)) {
        bigint_impl * to_return = new bigint_impl();
        (*(to_return->get_ptr_count_of_digits())) = 1;
        return to_return;
    }

    if (left_m_size == 1 || right_m_size == 1) {
        bigint_multiplication * column_mult = new bigint_column_multiplication();
        bigint * to_return = column_mult->multiply(left_multiplier, right_multiplier);
        delete column_mult;
        return to_return;
    }

    // A = [A0 = (Am-1, ..., A0) ; A1 = (An-1, ..., Am)]
    // B = [B0 = (Bm-1, ..., B0) ; B1 = (Bn-1, ..., Bm)]
    // A*B = A0*B0 + {(A0+A1)(B0+B1) - A0B0 - A1B1} * base^m + A1B1 * base^{2m}
    // Let A0B0 = Z0, (A0+A1)(B0+B1) = Z1, A1B1 = Z2;

    // Determine the size of the half-numbers
    size_t max_size = (left_multiplier->get_count_of_digits() > right_multiplier->get_count_of_digits())
            ? left_multiplier->get_count_of_digits()
            : right_multiplier->get_count_of_digits();
    size_t half_size = max_size / 2; // m

    // Split the digit sequences about the middle
    auto * bi_left_A1 = new bigint_impl(), * bi_left_A0 = new bigint_impl();  // A0, A1
    auto * bi_right_B1 = new bigint_impl(), * bi_right_B0 = new bigint_impl();   // B0, B1
    // |123456789|
    reinterpret_cast<bigint_impl *>(const_cast<bigint *>(left_multiplier))->split(bi_left_A1, bi_left_A0, half_size);
    reinterpret_cast<bigint_impl *>(const_cast<bigint *>(right_multiplier))->split(bi_right_B1, bi_right_B0, half_size);

    bigint * z0 = this->multiply(bi_left_A0, bi_right_B0); // A0B0
    bigint * z1 = this->multiply(bi_left_A0->sum(bi_left_A1), bi_right_B0->sum(bi_right_B1));  // (A0+A1)(B0+B1)
    bigint * z2 = this->multiply(bi_left_A1, bi_right_B1); // A1B1

    // The next step is this calculation:
    // return (z2*base^(2*m))+((z1-z2-z0)*base^(m))+(z0)
    // So we will start with most complex bit - z1-z2-z0 * base^(halfsize)
    // We will do the sums, then move z1 left by halfsize bytes
    z1->subtract(z2);
    z1->subtract(z0);
    z1->mult_by_pow_base(half_size);
    // Then move z2 left by max_size bytes
    z2->mult_by_pow_base(half_size * 2);
    // Then we add z0, z1 and z2 together
    bigint * return_value = z2->sum(z1);
    return_value->add(z0);

    delete bi_left_A1;
    delete bi_right_B1;
    delete bi_left_A0;
    delete bi_right_B0;
//    delete z0;
//    delete z1;
//    delete z2;

    return return_value;
}

bigint *bigint_schonhage_strassen_multiplication::multiply(const bigint *const left_multiplier,
                                                                        const bigint *const right_multiplier) const {
    return nullptr;
}

#pragma endregion
