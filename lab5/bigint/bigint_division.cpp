#include "bigint_division.h"


bigint *bigint_burnikel_ziegler_division::divide(const bigint *const dividend, const bigint *const divider,
                                                 const bigint_multiplication *const multiplication_impl) const
{
    std::pair<bigint_impl *, bigint_impl *> returned_by_divide_with_remainder = divide_with_remainder(dividend, divider, multiplication_impl);
    delete returned_by_divide_with_remainder.second;
    return returned_by_divide_with_remainder.first;
}

std::pair<bigint_impl *, bigint_impl *>
bigint_burnikel_ziegler_division::divide_with_remainder(const bigint *const dividend, const bigint *const divider,
                                                        const bigint_multiplication *const multiplication_impl) const {
// todo: check divider == 0
    if (!dividend || !divider) {
        return {nullptr, nullptr};
    }
    auto * dividend_inner = const_cast<bigint *>(dividend), * divider_inner = const_cast<bigint *>(divider);

#pragma region handle signs
    bool dividend_sign = dividend->get_sign(), divider_sign = divider->get_sign();
    if (dividend_sign) {
        dividend_inner->change_sign();
    }

    if (divider_sign) {
        divider_inner->change_sign();
    }
#pragma endregion

    if (dividend < divider) {
        // handle signs
        // todo: return 0 and dividend as a remainder
    }
    else if (dividend == divider) {
        // handle signs
        // todo: return 1 with remainder inited
    }

    bigint_impl * AHigh, * ALow;
    size_t half_size = dividend->get_count_of_digits() / 2;
    split_for_AHigh_ALow(reinterpret_cast<bigint_impl *>(const_cast<bigint *>(dividend)), &AHigh, &ALow, half_size);
    std::pair<bigint_impl *, bigint_impl *> quotient_and_remainder = div_two_digits_by_one(AHigh, ALow,
                                     dynamic_cast<bigint_impl *>(const_cast<bigint *>(divider)), multiplication_impl);

#pragma region handle signs
    if (dividend_sign) {
        dividend_inner->change_sign();
    }

    if (divider_sign) {
        divider_inner->change_sign();
    }

    if ((!(dividend_sign && divider_sign)) && (dividend_sign || divider_sign)) {
        quotient_and_remainder.first->change_sign();
    }
#pragma endregion

    return quotient_and_remainder;
}

std::pair<bigint_impl *, bigint_impl *>
bigint_burnikel_ziegler_division::div_two_digits_by_one(bigint_impl *AHigh, bigint_impl *ALow, bigint_impl * B, const bigint_multiplication *const multiplication_impl) const
{
    // DivTwoDigitsByOne(AHigh, ALow, B), return quotient Q and remainder S
    // 1) Let [a1,a2] = AH, [a3,a4] = AL, and [b1,b2] = B
    // 2) [q1,R] = DivThreeHalvesByTwo(a1,a2,a3,b1,b2)
    // 3) Let [r1,r2]=R
    // 4) [q2,S] = DivThreeHalvesByTwo(r1,r2,a4,b1,b2)
    // 5) Return Q=[q1,q2] and S

    bigint_impl * a1 = new bigint_impl(), * a2 = new bigint_impl();
    reinterpret_cast<bigint_impl *>(AHigh)->split(a1, a2, (AHigh->get_count_of_digits() / 2));
    bigint_impl * a3 = new bigint_impl(), * a4 = new bigint_impl();
    reinterpret_cast<bigint_impl *>(ALow)->split(a3, a4, (ALow->get_count_of_digits() / 2));
    bigint_impl * b1 = new bigint_impl(), * b2 = new bigint_impl();
    reinterpret_cast<bigint_impl *>(B)->split(b1, b2, (B->get_count_of_digits() / 2));
    std::pair<bigint_impl *, bigint_impl *> first_part_quotient_and_R = div_three_halves_by_two(a1, a2, a3, b1, b2, multiplication_impl);
    bigint_impl * q1 = first_part_quotient_and_R.first;

    bigint_impl * r1 = new bigint_impl(), * r2 = new bigint_impl();
    reinterpret_cast<bigint_impl *>(first_part_quotient_and_R.second)
            ->split(r1, r2, (first_part_quotient_and_R.second->get_count_of_digits() / 2));
    delete first_part_quotient_and_R.second;
    std::pair<bigint_impl *, bigint_impl *> second_part_quotient_and_remainder = div_three_halves_by_two(r1, r2, a4, b1, b2, multiplication_impl);
    bigint_impl * q2 = second_part_quotient_and_remainder.first;

    // to_return_quotient = q1 * base^{q2._count} + q2
    q1->mult_by_pow_base(second_part_quotient_and_remainder.first->get_count_of_digits());
    bigint_impl * to_return_quotient = reinterpret_cast<bigint_impl *>(second_part_quotient_and_remainder.first->sum(q1));

    delete a1;  delete a2;  delete a3;  delete a4;
    delete b1;  delete b2;
    delete r1;  delete r2;
    delete q1;  delete q2;
    return {to_return_quotient, second_part_quotient_and_remainder.second};
}

std::pair<bigint_impl *, bigint_impl *>
bigint_burnikel_ziegler_division::div_three_halves_by_two(bigint_impl *a1, bigint_impl *a2, bigint_impl *a3, bigint_impl *b1,
                                                          bigint_impl *b2, const bigint_multiplication *const multiplication_impl) const
{
    // DivThreeHalvesByTwo(a1,a2,a3,b1,b2)
    // 6) q=[a1,a2]/b1;
    // 7) c=[a1,a2]-q*b1
    // 8) D=q*b2
    // 9) R=[r1,r2]=[c,a3]-D
    //10) if(R<0) {
    //11)   q--;
    //12)   R+=B;
    //13)   if(R<0) {
    //14)      q--;
    //15)      R+=B;
    //16)   }
    //17) }
    //18) Return quotient q and remainder R
    bigint_impl * a1a2 = new bigint_impl();
    (*a1a2) = (*a1);
    a1a2->mult_by_pow_base(a2->get_count_of_digits());
    a1a2->add(a2);
    bigint * bi_zero = new bigint_impl(int(0));

    bigint_impl * q_quotient = new bigint_impl(),* c_remainder = new bigint_impl();
    // todo:
    if (b1 != bi_zero) {
        std::pair<bigint_impl *, bigint_impl *> quot_and_rem = divide_with_remainder(a1a2, b1, multiplication_impl);
        q_quotient = quot_and_rem.first;
        c_remainder = quot_and_rem.second;
    }

    // 9) R=[r1,r2]=[c,a3]-q*b2 = c * base(a3.size) + a3 - q*b2
    bigint * D = multiplication_impl->multiply(q_quotient, b2); // q*b2
    c_remainder->mult_by_pow_base(a3->get_count_of_digits());
    c_remainder->add(a3);
    bigint_impl * remainder = reinterpret_cast<bigint_impl *>(c_remainder->subtraction(D));

    // todo: redo, make a copy
    b1->mult_by_pow_base(b2->get_count_of_digits());
    bigint_impl * B = reinterpret_cast<bigint_impl *>(b1->sum(b2));
    while (remainder < bi_zero) {
        q_quotient--; // todo:
        reinterpret_cast<bigint *>(remainder)->add(reinterpret_cast<bigint *>(B));
    }

    delete a1a2;    delete bi_zero;   delete c_remainder;
    delete D;
    return {q_quotient, remainder};
}

void bigint_burnikel_ziegler_division::split_for_AHigh_ALow(bigint_impl *to_split, bigint_impl **AH, bigint_impl **AL,
                                                            size_t half_size) const
{
    (*AH) = new bigint_impl();
    (*AL) = new bigint_impl();
    if (to_split->get_count_of_digits() > 2) {
        reinterpret_cast<bigint_impl *>(to_split)->split((*AH), (*AL), half_size);
        return;
    }

    (*((*AH)->get_ptr_count_of_digits()))++;
    (**AL) = (*to_split);
}
/*
vector<LongIntegerUP> LongInteger::DivTwoDigitsByOne(LongIntegerUP & AHigh, LongIntegerUP & ALow, LongIntegerUP & B, UINT uNumDigits) {
    // DivTwoDigitsByOne(AHigh, ALow, B), return quotient Q and remainder S
    //
    // 1) Let [a1,a2] = AH, [a3,a4] = AL, and [b1,b2] = B
    // 2) [q1,R] = DivThreeHalvesByTwo(a1,a2,a3,b1,b2)
    // 3) Let [r1,r2]=R
    // 4) [q2,S] = DivThreeHalvesByTwo(r1,r2,a4,b1,b2)
    // 5) Return Q=[q1,q2] and S


//    Input Two nonnegative integers A and B, such that A<βnB and βn/2 ≤ B<βn.
//    n must be even.
//    Output The quotient A/B and the remainder A mod B.
//    1. Let A = A3β3n/2 +A2βn +A1βn/2 +A0 and B = B1βn/2 +B0, with 0 ≤ Ai <
//    βn/2 and 0 ≤ Bi < βn/2.
//    2. Compute the high half Q1 of the quotient as Q1 = A3βn+A2βn/2+A1
//    B with remainder
//    R1 using algorithm 3.4.
//    3. Compute the low half Q0 of the quotient as Q0 = R1βn/2+A4
//    B with remainder
//    R0 using algorithm 3.4.
//    4. Return the quotient Q = Q1βn/2 + Q0 and the remainder R = R0.


    vector<LongIntegerUP> vReturn(2); // Return Q & S

    // Decide how many digits we are chopping up the numbers by (it needs to be consistent)
    if (uNumDigits == 0) uNumDigits = 1;

    vector<LongIntegerUP> vWorking;
    vWorking = move(LongInteger::split(AHigh, 2, uNumDigits));
    LongIntegerUP a1 = move(vWorking[1]);
    LongIntegerUP a2 = move(vWorking[0]);
    vWorking = move(LongInteger::split(ALow, 2, uNumDigits));
    LongIntegerUP a3 = move(vWorking[1]);
    LongIntegerUP a4 = move(vWorking[0]);
    vWorking = move(LongInteger::split(B, 2, uNumDigits));
    LongIntegerUP b1 = move(vWorking[1]);
    LongIntegerUP b2 = move(vWorking[0]);

    vector<LongIntegerUP> vResult1, vResult2;
    // 2) [q1,R] = DivThreeHalvesByTwo(a1,a2,a3,b1,b2)
    LongInteger a1a2a3 = (*a1 << (uNumDigits * BASEVALBITS * 2)) + (*a2 << (uNumDigits * BASEVALBITS)) + *a3;

    if (a1a2a3 < *B) {
        vResult1.push_back(make_unique<LongInteger>(LongInteger()));
        vResult1.push_back(make_unique<LongInteger>(a1a2a3));
    }
    else
        vResult1 = DivThreeHalvesByTwo(a1, a2, a3, b1, b2, uNumDigits);

    // 3) Let [r1,r2]=R
    vWorking = move(LongInteger::split(vResult1[1], 2, uNumDigits));
    LongIntegerUP r1 = move(vWorking[1]);
    LongIntegerUP r2 = move(vWorking[0]);

    // 4) [q2,S] = DivThreeHalvesByTwo(r1,r2,a4,b1,b2)
    a1a2a3 = (*vResult1[1] << (uNumDigits * BASEVALBITS)) + *a4;
    if (a1a2a3 < *B) {
        vResult2.push_back(make_unique<LongInteger>(LongInteger()));
        vResult2.push_back(make_unique<LongInteger>(a1a2a3));
    }
    else {
        vResult2 = DivThreeHalvesByTwo(r1, r2, a4, b1, b2, uNumDigits);
    }
    // 5) Return Q=[q1,q2] and S
    vReturn[1] = move(vResult2[1]);
    vector<LongIntegerUP> Q(2);
    Q[1] = move(vResult1[0]);
    Q[0] = move(vResult2[0]);
    vReturn[0] = move(merge(Q, 2, uNumDigits));

    return move(vReturn);
}

vector<LongIntegerUP> LongInteger::DivThreeHalvesByTwo(LongIntegerUP & a2, LongIntegerUP & a1, LongIntegerUP & a0, LongIntegerUP & b1, LongIntegerUP & b0, UINT uNumDigits) {
    vector<LongIntegerUP> vResult(2); // To hold q & R
    vector<LongIntegerUP> vMerge(2);

    LongIntegerUP B(nullptr);
    LongIntegerUP Q, R;

    Q = make_unique<LongInteger>(0);
    R = make_unique<LongInteger>(0);

    if (*b1 != 0) {
        *Q = ((*a2) << (uNumDigits * LongInteger::BASEVALBITS)) + (*a1);
        // The next 2 lines can be done recursively as they are division and modulus

        if (Q->size > BURKINELZIEGLERCUTOFF) {
            LongIntegerUP temp;
            BurnikelZiegler(*Q, *b1, temp, R);
            Q = move(temp);
        }
        else {
            LongIntegerUP Q2 = make_unique<LongInteger>(0);
            DivAndMod(*Q, *b1, Q2, R);
            Q = move(Q2);
        }
    }
    else {
        // I am baffled. The line below doesn't work as expected and instead gives the result 255
        // no matter what value uNumDigits is. However, the calculation works.
        // I've worked out why the number goes wrong - it is calculating 1 rightshifted as a UINT,
        // which can't be shifted more than 3 bytes left.
        // However, I cannot explain why the calculation still works.
        *Q = (1 << (uNumDigits * BASEVALBITS)) - 1;
        *R = ((*a2) << (uNumDigits * LongInteger::BASEVALBITS)) + *a1;
    }

    *R = (*R << (uNumDigits * LongInteger::BASEVALBITS)) + *a0 - (*Q * *b0);

    vMerge[0] = make_unique<LongInteger>(*b0);
    vMerge[1] = make_unique<LongInteger>(*b1);
    B = move(merge(vMerge, 2, uNumDigits));

    // Added to handle a scenario not mentioned in the algorithm
    // Need to speed this up a bit as it can loop for up to 256^uNumDigits-1 times - which can be a lot!
    // Speed up code added by adding left-shifts. uDiff is the number of bits to left-shift
    // There are still awkward scenarios. The worst case is that this loop runs for log2(B) times, which isn't great
    // but a lot less than before

    while (*R >= *B || *R < 0) {
        if (*R >= *B) {
            if (R->size < B->size) {
                *R -= *B;
                (*Q)++;
            }
            else {
                UINT uDiff = R->size - B->size;
                uDiff *= BASEVALBITS;
                // Try to get the exact number of bits different
                if (R->digits[(R->size - 1)] > B->digits[(B->size - 1)]) {
                    int uDiffBits = R->digits[(R->size - 1)] / B->digits[(B->size - 1)];
                    while (uDiffBits > 1) {
                        uDiffBits /= 2;
                        uDiff++;
                    }
                }
                else {
                    int uDiffBits = B->digits[(B->size - 1)] / R->digits[(R->size - 1)];
                    while (uDiffBits > 1) {
                        uDiffBits /= 2;
                        uDiff--;
                    }
                }
                // *temp created to check we aren't deleting too large a number
                // It is very hard to get the balance between too large and too small
                // This is an overhead, but it should reduce the number of loops and so
                // save more than it costs
                // Changed the test as I encountered a scenario where the overall while loop went into an infinite cycle
                LongInteger* temp = new LongInteger(*B << uDiff);
                while (temp->size == R->size && temp->digits[temp->size - 1] > R->digits[R->size - 1] && uDiff > 0) {
                    *temp >>= 1;
                    uDiff--;
                }
                if (temp->size > 1 && temp->size == R->size && temp->digits[temp->size - 1] == R->digits[R->size - 1] &&
                    temp->digits[temp->size - 2] > R->digits[R->size - 2] && uDiff > 0) {
                    *temp >>= 1;
                    uDiff--;
                }

                *R -= *temp;
                delete temp;
                *Q += (LongInteger(1) << uDiff);
            }
        }
        else {
            // In some scenarios R can end up massively negative, so make B a lot bigger
            if (R->size < B->size) {
                // If B is already bigger than R then just add it to R to make R positive
                *R += *B;
                (*Q)--;
            }
            else {
                UINT uDiff = R->size - B->size;
                uDiff *= BASEVALBITS;
                // Try to get the exact number of bits different
                int uDiffBits = R->digits[(R->size - 1)] / B->digits[(B->size - 1)];
                while (uDiffBits > 1) {
                    uDiffBits /= 2;
                    uDiff++;
                }
                if (uDiff > 0)
                    uDiff--;

                *R += (*B << uDiff);
                *Q -= (LongInteger(1) << uDiff);
            }
        }
    }


    vResult[0] = move(Q);
    vResult[1] = move(R);

    return vResult;

}
*/