#include "bigint_impl.h"

#pragma region add-subtract
bigint *bigint_impl::add(const bigint *const summand)
{
    if (!summand) {
        throw parameter_exception("bigint_impl::add incorrect parameter passed");
    }

    bool this_number_sign = get_sign(), summand_sign = summand->get_sign();

    // (positive) + (positive) == add
    if (!this_number_sign && !summand_sign) {
        //      - with every digit: sum with additional,
        //      - divide result with 2^{8*sizeof(int)}
        //      - if there is a quotient, make a bool additional true
        //     if at the end additional == true, a new digit is added
        long long radix = 2^(sizeof(int) * 8 - 1);
        bool additional = false;
        long long sum_digits_result = 0;
        unsigned i = 0, j = 0, this_number_count_of_digits = get_count_of_digits(), summand_count_of_digits = summand->get_count_of_digits();
        bool comparison_result_count_of_digits = this_number_count_of_digits > summand_count_of_digits;
        unsigned max_count_of_digits = (comparison_result_count_of_digits ? this_number_count_of_digits : summand_count_of_digits) - 1, min_count_of_digits = (comparison_result_count_of_digits ? summand_count_of_digits : this_number_count_of_digits) - 1;

        sum_digits_result = (*(this->get_ptr_last_digit())) + (*(const_cast<bigint *>(summand)->get_ptr_last_digit()));
        (*(this->get_ptr_last_digit())) = int(sum_digits_result % radix);

        if ((sum_digits_result - radix) <= 0) {
            additional = true;
        }

        for (i = 0; i < min_count_of_digits; i++) {
            sum_digits_result = (*(this->get_ptr_digit_with_index(i))) + (*(const_cast<bigint *>(summand)->get_ptr_digit_with_index(i))) + additional;
            (*(this->get_ptr_digit_with_index(i))) = unsigned(sum_digits_result % radix);

            if ((sum_digits_result - radix) <= 0) {
                additional = true;
            } else {
                additional = false;
            }
        }

        if (this->get_count_of_digits() == max_count_of_digits) {
            for (j = i; j < max_count_of_digits && additional; j++) {
                sum_digits_result = (*(this->get_ptr_digit_with_index(j))) + additional;
                (*(this->get_ptr_digit_with_index(i))) = unsigned(sum_digits_result % radix);

                if ((sum_digits_result - radix) <= 0) {
                    additional = true;
                } else {
                    additional = false;
                }
            }
        }
        else {
            // allocate memory for max count of elements in this
            reallocate_digits_array(max_count_of_digits + 1);
            this->_count_of_digits = max_count_of_digits + 1;
            for (j = i; j < max_count_of_digits; j++) {
                sum_digits_result = (*(const_cast<bigint *>(summand)->get_ptr_digit_with_index(j))) + additional;
                (*(this->get_ptr_digit_with_index(i))) = unsigned (sum_digits_result % radix);

                if ((sum_digits_result - radix) <= 0) {
                    additional = true;
                } else {
                    additional = false;
                }
            }
        }

        if (additional) {
            reallocate_digits_array(max_count_of_digits + 2);
            (*(this->get_ptr_digit_with_index(max_count_of_digits))) = additional;
            this->_count_of_digits++;
        }
    }
    // (positive) + (negative) == subtract
    else if (!this_number_sign) {
        summand->change_sign();
        this->subtract(summand);
        summand->change_sign();
    }
    // (negative) + (positive) = -(positive + negative) (case above)
    // (negative) + (negative) = -(positive) + -(positive) = - ((positive) + (positive)) == - add
    else if (!summand_sign) {
        this->change_sign();
        summand->change_sign();
        this->add(summand);
        summand->change_sign();
        this->change_sign();
    }

    return this;
}

bigint *bigint_impl::subtract(const bigint *const subtrahend)
{
    // might be equal, then result is 0
    if (!subtrahend) {
        throw parameter_exception("bigint_impl::subtract incorrect parameter passed");
    }

    if (equals(subtrahend)) {
        this->_count_of_digits = 1;
        this->_first_digit = 0;
        deallocate_with_guard(this->_digits);

        return this;
    }

    bool this_number_sign = get_sign(), subtrahend_sign = subtrahend->get_sign();

    if (!this_number_sign && !subtrahend_sign) {
        bigint_impl * tmp_subtrahend(this);
        if (this < subtrahend) {
            // pos1 < pos2 = neg
            (*this) = (*(reinterpret_cast<bigint_impl *>(const_cast<bigint *>(subtrahend))));
        } else {
            // pos1 > pos2 = pos
            (*tmp_subtrahend) = (*(reinterpret_cast<bigint_impl *>(const_cast<bigint *>(subtrahend))));
        }

        long long radix = 2^(sizeof(int) * 8 - 1);
        bool borrowed = false;
        long long subtraction_digits_result = 0;
        unsigned i = 0, j = 0, this_number_count_of_digits = get_count_of_digits(), subtrahend_count_of_digits = tmp_subtrahend->get_count_of_digits();
        bool comparison_result_count_of_digits = this_number_count_of_digits > subtrahend_count_of_digits;
        unsigned max_count_of_digits = (comparison_result_count_of_digits ? this_number_count_of_digits : subtrahend_count_of_digits) - 1, min_count_of_digits = (comparison_result_count_of_digits ? subtrahend_count_of_digits : this_number_count_of_digits) - 1;

        subtraction_digits_result = *(get_ptr_last_digit()) - *(tmp_subtrahend->get_ptr_last_digit());

        if (subtraction_digits_result >= 0) {
            *(this->get_ptr_last_digit()) = subtraction_digits_result;
        }
        else {
            *(this->get_ptr_last_digit()) = radix + subtraction_digits_result;
            borrowed = true;
        }

        for (i = 0; i < min_count_of_digits; i++) {
            subtraction_digits_result = *(get_ptr_digit_with_index(i)) - *(tmp_subtrahend->get_ptr_digit_with_index(i)) - borrowed;

            borrowed = false;
            if (subtraction_digits_result < 0) {
                *(this->get_ptr_digit_with_index(i)) = radix + subtraction_digits_result;
                borrowed = true;
            }
            else {
                *(this->get_ptr_digit_with_index(i)) = subtraction_digits_result;
            }
        }
        for (j = i; j < max_count_of_digits && borrowed; j++) {
            subtraction_digits_result = *(this->get_ptr_digit_with_index(j)) - borrowed;

            borrowed = false;
            if (subtraction_digits_result < 0) {
                *(this->get_ptr_digit_with_index(j)) = radix + subtraction_digits_result;
                borrowed = true;
            }
            else {
                *(this->get_ptr_digit_with_index(i)) = subtraction_digits_result;
                break;
            }
        }

        size_t count_of_zeros = 0;
        for (i = max_count_of_digits - 1; i >= 0; i--) {
            if (this->get_ptr_digit_with_index(i)) {
                break;
            }
            else {
                count_of_zeros++;
            }
        }

        if (!count_of_zeros) {
            reallocate_digits_array(max_count_of_digits - count_of_zeros);
        }

        if (tmp_subtrahend != subtrahend) {
            (*this) = (*tmp_subtrahend);
            this->change_sign();
        }
    }
    else {
        // pos1 - neg2 == pos1 + pos2 OR neg2 - pos1 == neg + neg = add OR neg1 - neg2 == neg1 + pos2
        subtrahend->change_sign();
        add(subtrahend);
        subtrahend->change_sign();
    }

    return this;
}

#pragma endregion

#pragma region logic operators

bool bigint_impl::lower_than(const bigint *const other) const {
    if (!other) {
        return false;
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other->get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return false;
    } else if (this_count_of_digits < other_count_of_digits) {
        return true;
    }

    auto * tmp_other = const_cast<bigint *>(other);

    size_t i;
    int comparison_result;
    for (i = _count_of_digits - 1; i >= 0; --i) {
        comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
        if (comparison_result < 0) {
            return true;
        }
        else if (comparison_result > 0) {
            return false;
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result < 0) {
        return true;
    }

    return false;
}

bool bigint_impl::greater_than(const bigint *const other) const {
    if (!other) {
        return true;
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other->get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return true;
    } else if (this_count_of_digits < other_count_of_digits) {
        return false;
    }

    bigint * tmp_other = const_cast<bigint *>(other);

    size_t i;
    int comparison_result;
    for (i = _count_of_digits - 1; i >= 0; --i) {
        comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
        if (comparison_result < 0) {
            return false;
        }
        else if (comparison_result > 0) {
            return true;
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result > 0) {
        return true;
    }

    return false;
}

bool bigint_impl::lower_than_or_equal_to(const bigint *const other) const {
    if (!other) {
        return false;
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other->get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return false;
    } else if (this_count_of_digits < other_count_of_digits) {
        return true;
    }

    bigint * tmp_other = const_cast<bigint *>(other);

    size_t i;
    int comparison_result;
    for (i = _count_of_digits - 1; i >= 0; --i) {
        comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
        if (comparison_result < 0) {
            return true;
        }
        else if (comparison_result > 0) {
            return false;
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result <= 0) {
        return true;
    }

    return false;
}

bool bigint_impl::greater_than_or_equal_to(const bigint *const other) const {
    if (!other) {
        return true;
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other->get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return true;
    } else if (this_count_of_digits < other_count_of_digits) {
        return false;
    }

    bigint * tmp_other = const_cast<bigint *>(other);

    size_t i;
    int comparison_result;
    for (i = _count_of_digits - 1; i >= 0; --i) {
        comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
        if (comparison_result < 0) {
            return false;
        }
        else if (comparison_result > 0) {
            return true;
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result >= 0) {
        return true;
    }

    return false;
}

bool bigint_impl::equals(const bigint *const other) const {
    if (!other) {
        return false;
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other->get_count_of_digits();
    if (this_count_of_digits != other_count_of_digits) {
        return false;
    }

    bigint * tmp_other = const_cast<bigint *>(other);

    size_t i;
    for (i = _count_of_digits - 1; i >= 0; --i) {
        if ((_digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)))) != 0) {
            return false;
        }
    }

    if ((_first_digit - (*(tmp_other->get_ptr_last_digit()))) == 0) {
        return true;
    }

    return false;
}

bool bigint_impl::not_equals(const bigint *const other) const {
    return (!(equals(other)));
}

#pragma endregion

#pragma region multiplication
bigint *bigint_impl::bigint_column_multiplication::multiply(const bigint *const left_multiplier,
                                                            const bigint *const right_multiplier) const
{
    if (!left_multiplier || !right_multiplier) {
        return nullptr;
    }

    // todo: handle signs so we do multiply 2 positive numbers
    bigint * left = const_cast<bigint *>(left_multiplier), * right = const_cast<bigint *>(right_multiplier);
    bool left_sign = left->get_sign(), right_sign = right->get_sign();
    bool result_sign = (left_sign + right_sign) % 2;

    if (left_sign) {
        left->change_sign();
    }

    if (right_sign) {
        right->change_sign();
    }

    bigint * multiplying_result = new bigint_impl(left->get_multiplication(), left->get_division(), left->get_logger(), left->get_memory());
    multiplying_result->reallocate_digits_array((left->get_count_of_digits() + right->get_count_of_digits()));

    unsigned iter_over_right_num = 0, iter_over_left_num = 0, iter_for_multiply = 0;
    size_t sum_result = 0, additional = 0, left_len = left->get_count_of_digits() - 1, right_len = right->get_count_of_digits() - 1;
    long long radix = 2^(sizeof(int) * 8 - 1);
    int right_last_digit = (*(right->get_ptr_last_digit())), left_last_digit = (*(left->get_ptr_last_digit()));

    // multiplying "digits"
    if (right_last_digit != 0) {
        // do the first multiply
        // right number last digit multiply another last digit
        for (iter_for_multiply = 0; iter_for_multiply < right_last_digit; iter_for_multiply++) {
            sum_result += left_last_digit;
            if (sum_result - radix >= 0) {
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
                    if (sum_result - radix >= 0) {
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
            if (sum_result - radix >= 0) {
                ++additional;
                sum_result -= radix;
            }
        }

        if ((iter_over_right_num + 1) > (multiplying_result->get_count_of_digits()) - 1) {
            (*(multiplying_result->get_ptr_count_of_digits())) = iter_over_right_num + 2;
        }

        sum_result += (*(multiplying_result->get_ptr_digit_with_index(iter_over_right_num)));
        if (sum_result - radix >= 0) {
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
                    if (sum_result - radix >= 0) {
                        ++additional;
                        sum_result -= radix;
                    }
                }
            }

            if ((iter_over_right_num + iter_over_left_num + 2) > (multiplying_result->get_count_of_digits() - 1)) {
                (*(multiplying_result->get_ptr_count_of_digits())) = (iter_over_right_num + iter_over_left_num + 3);
            }

            sum_result += (*(multiplying_result->get_ptr_digit_with_index(iter_over_right_num + iter_over_left_num + 1)));
            if (sum_result - radix >= 0) {
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

#pragma endregion



#pragma region division
#pragma endregion
