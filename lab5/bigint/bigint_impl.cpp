#include "bigint_division.h"
#include "bigint_impl.h"


#pragma region add-subtract
bigint *bigint_impl::add(const bigint *const summand)
{
    if (!summand) {
        throw parameter_exception("bigint_impl::add incorrect parameter passed");
    }

    bool this_number_sign = get_sign();
    bool summand_sign = summand->get_sign();

    // (positive) + (positive) == add
    if (!this_number_sign && !summand_sign) {
        //      - with every digit: sum with additional,
        //      - divide result with 2^{8*sizeof(int) - 1}
        //      - if there is a quotient, make a bool additional true
        //     if at the end additional == true, a new digit is added
        long long radix = INT_MAX;
        bool additional = false;
        long long sum_digits_result = 0;
        long long i, j, this_number_count_of_digits = get_count_of_digits(), summand_count_of_digits = summand->get_count_of_digits();
        bool comparison_result_count_of_digits = this_number_count_of_digits > summand_count_of_digits;
        unsigned max_count_of_digits = (comparison_result_count_of_digits ? this_number_count_of_digits : summand_count_of_digits) - 1;
        unsigned min_count_of_digits = (comparison_result_count_of_digits ? summand_count_of_digits : this_number_count_of_digits) - 1;

        sum_digits_result = (long long)(*(this->get_ptr_last_digit())) + (long long)(*(const_cast<bigint *>(summand)->get_ptr_last_digit()));
        (*(this->get_ptr_last_digit())) = int(sum_digits_result % radix);

        if (sum_digits_result >= radix) {
            additional = true;
        }

        for (i = 0; i < min_count_of_digits; i++) {
            sum_digits_result = (*(this->get_ptr_digit_with_index(i))) + (*(const_cast<bigint *>(summand)->get_ptr_digit_with_index(i))) + additional;
            (*(this->get_ptr_digit_with_index(i))) = unsigned(sum_digits_result % radix);

            if (sum_digits_result <= radix) {
                additional = true;
            } else {
                additional = false;
            }
        }

        if (this->get_count_of_digits() - 1 == max_count_of_digits) {
            for (j = i; j < max_count_of_digits; j++) {
                sum_digits_result = (*(this->get_ptr_digit_with_index(j))) + additional;
                (*(this->get_ptr_digit_with_index(i))) = unsigned(sum_digits_result % radix);

                if (sum_digits_result <= radix) {
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

                if (sum_digits_result <= radix) {
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
        const_cast<bigint *>(summand)->change_sign();
        this->subtract(summand);
        const_cast<bigint *>(summand)->change_sign();
    }
    // (negative) + (positive) = -(positive + negative) (case above)
    // (negative) + (negative) = -(positive) + -(positive) = - ((positive) + (positive)) == - add
//    else if (!summand_sign) {
    else {
        this->change_sign();
        const_cast<bigint *>(summand)->change_sign();
        this->add(summand);
        const_cast<bigint *>(summand)->change_sign();
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

    if (equals((*subtrahend))) {
        this->_count_of_digits = 1;
        this->_first_digit = 0;
        deallocate_with_guard(this->_digits);

        return this;
    }

    bigint_impl bi_0(int(0));
    if ((*subtrahend) == bi_0) {
        return this;
    }

    if ((*this) == bi_0) {
        // this = -subtrahend
        (*this) = (*reinterpret_cast<bigint_impl *>(const_cast<bigint *>(subtrahend)));
        this->change_sign();
        return this;
    }

#pragma region handle signs
    bool this_number_sign = get_sign(), subtrahend_sign = subtrahend->get_sign();

    if (!this_number_sign && !subtrahend_sign) {
        bigint_impl * tmp_subtrahend = new bigint_impl();
        if ((*this) < (*subtrahend)) {
            // pos1 < pos2 = neg
            (*tmp_subtrahend) = (*this);
            (*this) = (*(reinterpret_cast<bigint_impl *>(const_cast<bigint *>(subtrahend))));
        } else {
            // pos1 > pos2 = pos
            (*tmp_subtrahend) = (*(reinterpret_cast<bigint_impl *>(const_cast<bigint *>(subtrahend))));
        }
#pragma endregion

        long long radix = INT_MAX;
        bool borrowed = false;
        long long subtraction_digits_result = 0;
        unsigned i = 0, j = 0, this_number_count_of_digits = get_count_of_digits(), subtrahend_count_of_digits = tmp_subtrahend->get_count_of_digits();
        bool comparison_result_count_of_digits = this_number_count_of_digits > subtrahend_count_of_digits;
        unsigned max_count_of_digits = (comparison_result_count_of_digits ? this_number_count_of_digits : subtrahend_count_of_digits) - 1, min_count_of_digits = (comparison_result_count_of_digits ? subtrahend_count_of_digits : this_number_count_of_digits) - 1;

#pragma region substracting first digits of numbers
        subtraction_digits_result = *(get_ptr_last_digit()) - *(tmp_subtrahend->get_ptr_last_digit());
        if (subtraction_digits_result >= 0) {
            *(this->get_ptr_last_digit()) = subtraction_digits_result;
        }
        else {
            *(this->get_ptr_last_digit()) = radix + subtraction_digits_result;
            borrowed = true;
        }
#pragma endregion

#pragma region substracting digits arrays
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
        // if one number is longer than another
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
#pragma endregion

#pragma region get rid of extra zeros
        long long count_of_zeros = 0;
        for (i = max_count_of_digits; i > 0; i--) {
            if ((*(this->get_ptr_digit_with_index(i - 1)))) {
                break;
            }
            else {
                count_of_zeros++;
            }
        }

        if (count_of_zeros) {
            reallocate_digits_array(max_count_of_digits - count_of_zeros);
            _count_of_digits -= count_of_zeros;
        }
#pragma endregion

        if (tmp_subtrahend == subtrahend) {
            (*this) = (*tmp_subtrahend);
            this->change_sign();
        }
    }
    else {
        // pos1 - neg2 == pos1 + pos2 OR neg2 - pos1 == neg + neg = add OR neg1 - neg2 == neg1 + pos2
        const_cast<bigint *>(subtrahend)->change_sign();
        add(subtrahend);
        const_cast<bigint *>(subtrahend)->change_sign();
    }

    return this;
}

#pragma endregion

#pragma region logic operators

bool bigint_impl::lower_than(const bigint & other) const {
    bool this_sign = this->get_sign(), other_sign = other.get_sign();
    if (this_sign && !other_sign) {
        return true;
    }
    if (!this_sign && other_sign) {
        return false;
    }
    if (this_sign) {
        reinterpret_cast<bigint_impl *>(const_cast<bigint_impl *>(this))->change_sign();
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other.get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return false;
    } else if (this_count_of_digits < other_count_of_digits) {
        return true;
    }

    bigint_impl * tmp_other = reinterpret_cast<bigint_impl *>(other.make_a_copy());
    if (other_sign) {
        tmp_other->change_sign();
    }

    long long i;
    int comparison_result;
    if (_count_of_digits > 1) {
        for (i = _count_of_digits - 1; i >= 0; --i) {
            comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
            if (comparison_result < 0) {
                delete tmp_other;
                return true;
            }
            else if (comparison_result > 0) {
                delete tmp_other;
                return false;
            }
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result < 0) {
        delete tmp_other;
        return true;
    }
    delete tmp_other;

    if (this_sign) {
        reinterpret_cast<bigint_impl *>(const_cast<bigint_impl *>(this))->change_sign();
    }
    return false;
}

bool bigint_impl::greater_than(const bigint & other) const {
    bool this_sign = this->get_sign(), other_sign = other.get_sign();
    if (this_sign && !other_sign) {
        return false;
    }
    if (!this_sign && other_sign) {
        return true;
    }
    if (this_sign) {
        reinterpret_cast<bigint_impl *>(const_cast<bigint_impl *>(this))->change_sign();
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other.get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return true;
    } else if (this_count_of_digits < other_count_of_digits) {
        return false;
    }

    bigint_impl * tmp_other = reinterpret_cast<bigint_impl *>(other.make_a_copy());
    if (other_sign) {
        tmp_other->change_sign();
    }

    long long i;
    int comparison_result;
    if (_count_of_digits > 1) {
        for (i = _count_of_digits - 1; i >= 0; --i) {
            comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
            if (comparison_result < 0) {
                delete tmp_other;
                return false;
            }
            else if (comparison_result > 0) {
                delete tmp_other;
                return true;
            }
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result > 0) {
        delete tmp_other;
        return true;
    }

    if (this_sign) {
        reinterpret_cast<bigint_impl *>(const_cast<bigint_impl *>(this))->change_sign();
    }
    delete tmp_other;
    return false;
}

bool bigint_impl::lower_than_or_equal_to(const bigint & other) const {
    bool this_sign = this->get_sign(), other_sign = other.get_sign();
    if (this_sign && !other_sign) {
        return true;
    }
    if (!this_sign && other_sign) {
        return false;
    }
    if (this_sign) {
        (const_cast<bigint_impl *>(this))->change_sign();
    }

    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other.get_count_of_digits();
    if (this_count_of_digits > other_count_of_digits) {
        return false;
    } else if (this_count_of_digits < other_count_of_digits) {
        return true;
    }

    bigint_impl * tmp_other = reinterpret_cast<bigint_impl *>(other.make_a_copy());
    if (other_sign) {
        tmp_other->change_sign();
    }

    long long i;
    int comparison_result;
    if (_count_of_digits > 1) {
        for (i = _count_of_digits - 1; i >= 0; --i) {
            comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
            if (comparison_result < 0) {
                delete tmp_other;
                return true;
            }
            else if (comparison_result > 0) {
                delete tmp_other;
                return false;
            }
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result <= 0) {
        delete tmp_other;
        return true;
    }

    if (this_sign) {
        const_cast<bigint_impl *>(this)->change_sign();
    }
    delete tmp_other;
    return false;
}

bool bigint_impl::greater_than_or_equal_to(const bigint & other) const {
    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other.get_count_of_digits();

    bool this_sign = this->get_sign(), other_sign = other.get_sign();
    if (this_sign && !other_sign) {
        return false;
    }
    if (!this_sign && other_sign) {
        return true;
    }
    if (this_sign) {
        reinterpret_cast<bigint_impl *>(const_cast<bigint_impl *>(this))->change_sign();
    }

    if (this_count_of_digits > other_count_of_digits) {
        return true;
    } else if (this_count_of_digits < other_count_of_digits) {
        return false;
    }

    bigint_impl * tmp_other = reinterpret_cast<bigint_impl *>(other.make_a_copy());
    if (other_sign) {
        tmp_other->change_sign();
    }

    long long i;
    int comparison_result;
    if (_count_of_digits > 1) {
        for (i = _count_of_digits - 1; i >= 0; --i) {
            comparison_result = _digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)));
            if (comparison_result < 0) {
                delete tmp_other;
                return false;
            }
            else if (comparison_result > 0) {
                delete tmp_other;
                return true;
            }
        }
    }

    comparison_result = _first_digit - (*(tmp_other->get_ptr_last_digit()));
    if (comparison_result >= 0) {
        delete tmp_other;
        return true;
    }

    delete tmp_other;

    if (this_sign) {
        reinterpret_cast<bigint_impl *>(const_cast<bigint_impl *>(this))->change_sign();
    }
    return false;
}

bool bigint_impl::equals(const bigint & other) const {
    size_t this_count_of_digits = _count_of_digits, other_count_of_digits = other.get_count_of_digits();
    if (this_count_of_digits != other_count_of_digits) {
        return false;
    }

    bigint * tmp_other = other.make_a_copy();

    if (_count_of_digits != 1) {
        long long i;
        for (i = _count_of_digits - 1; i >= 0; --i) {
            if ((_digits[i] - (*(tmp_other->get_ptr_digit_with_index(i)))) != 0) {
                delete tmp_other;
                return false;
            }
        }
    }

    if ((_first_digit - (*(tmp_other->get_ptr_last_digit()))) == 0) {
        delete tmp_other;
        return true;
    }

    delete tmp_other;
    return false;
}

bool bigint_impl::not_equals(const bigint & other) const {
    return (!(equals(other)));
}


#pragma endregion

bool bigint_impl::split(bigint_impl *bi_front, bigint_impl *bi_back, size_t split_digit) const
{
// Split the current bigint into 2 bits at the point specified by split_digit
// split correctly splits a number with EVEN count of digits.
    if (_count_of_digits & 1) {
        split_digit++;
    }

    bi_front->init();
    bi_back->init();

    if (split_digit >= _count_of_digits) {
        (*bi_back) = (*this);
        bi_front->_count_of_digits = 1;
        return false;
    }

    // Put the first half of the current LongInteger in bi_back
    bi_back->_count_of_digits = split_digit;
    bi_back->_first_digit = _first_digit;
    if (split_digit > 1) {
        bi_back->_digits = reinterpret_cast<unsigned *>(bi_back->allocate_with_guard((split_digit - 1) * sizeof(unsigned)));
        memcpy(bi_back->_digits, _digits, (split_digit - 1) * sizeof(unsigned));
    }

    // Put the second half in bi_front
    bi_front->_count_of_digits = _count_of_digits - split_digit;
    if (bi_front->_count_of_digits != 0) {
        bi_front->_first_digit = _digits[split_digit - 1];

        size_t count_of_digits_array_in_back = _count_of_digits - split_digit - 1;
        if (count_of_digits_array_in_back > 0) {
            bi_front->_digits = reinterpret_cast<unsigned *>(bi_front->allocate_with_guard(count_of_digits_array_in_back));
            memcpy(bi_front->_digits, _digits + ((split_digit + 1) * sizeof(unsigned)), (count_of_digits_array_in_back * sizeof(unsigned)));
        }
    } else {
        bi_front++;
    }

    return true;
}

bigint_impl::bigint_impl(std::string &from, const bigint_multiplication *const multiplication_impl, logger *logger, memory *allocator)
        : bigint(from, logger, allocator)
{
// reading from left to right
    from.erase(remove_if(from.begin(), from.end(), isalpha), from.end());
    // |_ log10(1 « (sizeof(int) « 3)) _|
    size_t k = (sizeof(int) << 3) * log10(2), power = 0;
    bigint_impl * multiply = new bigint_impl(size_t(pow(10, k)));
    bigint_impl * tmp_digit = new bigint_impl();
    std::string substring;
    bigint_impl * multiply_result = nullptr;
    int digit;

    while (!from.empty()) {
        substring = from.substr(0, k);
        power = substring.size();
        from.erase(0, power);
        digit = std::stoi(substring);
        (*tmp_digit) = bigint_impl(digit);
        if (power == k) {
            multiply_result = reinterpret_cast<bigint_impl *>(multiplication_impl->multiply(this, multiply));
            this->~bigint_impl();
            (*this) = (*multiply_result);
            multiply_result = nullptr;
        } else {
            // last digits
            bigint_impl * ten_in_power_last = new bigint_impl(size_t(pow(10, power)));
            multiply_result = reinterpret_cast<bigint_impl *>(multiplication_impl->multiply(this, ten_in_power_last));
            this->~bigint_impl();
            (*this) = (*multiply_result);
            multiply_result = nullptr;
            delete ten_in_power_last;
        }
        this->add(tmp_digit);
    }
    delete multiply;
    delete tmp_digit;
}

void bigint_impl::init() {
    _count_of_digits = 0;
    deallocate_with_guard(_digits);
    _first_digit = 0;
}

std::string bigint_impl::bi_to_string() {
    if (_count_of_digits == 0) {
        return "";
    }
    std::string to_return;
    to_return += std::to_string(_first_digit);
    unsigned i;
    for (i = 0; i < _count_of_digits - 1; i++) {
        to_return += _digits[i];
    }
    std::reverse(to_return.begin(), to_return.end());

    return to_return;
}


std::ostream &operator<<(std::ostream &out, const bigint_impl &value) {
    unsigned k = (sizeof(int) << 3) * log10(2), power = 0;
    bigint_impl * multiply = new bigint_impl(size_t(pow(10, k)));
    int digit = 0;
    bigint_impl tmp_res(value), *tmp_div, *bi_zero = new bigint_impl(int(0));
    std::stack<bigint_impl *> result_stack;
    std::pair<bigint_impl *, bigint_impl *> returned_by_div;
    bigint_karatsuba_multiplication * mult_impl = new bigint_karatsuba_multiplication();
    bigint_burnikel_ziegler_division * div_impl = new bigint_burnikel_ziegler_division();

    while (tmp_res != (*bi_zero)) {
        returned_by_div = div_impl->divide_with_remainder(&tmp_res, multiply, mult_impl);
        tmp_res.~bigint_impl();
        tmp_res = (*returned_by_div.first);
        result_stack.push(returned_by_div.second);
    }
    tmp_res.~bigint_impl();

    std::string result_string;
    while (!(result_stack.empty())) {
        result_string += result_stack.top()->bi_to_string();
        result_stack.pop();
    }

    out << result_string;

    return out;
}

std::istream &operator>>(std::istream &in, bigint_impl **value) {
    std::string input;
    std::getline(in, input);
    bigint_multiplication * mult_impl = new bigint_karatsuba_multiplication();
    (*value) = new bigint_impl(input, mult_impl);
    delete mult_impl;
    return in;
}
