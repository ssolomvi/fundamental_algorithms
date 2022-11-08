#include "header.h"


qes_statuses quadratic_equation_solve(long double a, long double b, long double c, double* _1, double* _2)
{
	if (fabs(a) < EPS) {
        return qes_not_qe;
    }

    else {
        long double d;
        if ((LDBL_MAX / b) >= b && (LDBL_MAX / a) >= 4 * c) {
            d = b * b - 4 * a * c;
            if (!d) {
                *_1 = -b / (2 * a);
                return qes_1_ans;
            }
            else if (d > 0) {
                *_1 = (- b + sqrt(d)) / (2 * a);
                *_2 = (-b - sqrt(d)) / (2 * a);
                return qes_2_ans;
            }
            else {
                return qes_not_r;
            }
        }
        else {
            return qes_overflow;
        }
    }
}


mult_statuses is_multiple(long long _1, long long  _2)
{
    if (_1 == 0 || _2 == 0) {
        return mult_is_0;
    }
    if (_1 % _2 == 0) {
        return mult_is_mult;
    }
    else {
        return mult_is_not_mult;
    }
}

triangle_statuses is_right_triangle(long double side_1, long double side_2, long double side_3) 
{
    long double tmp, tmp2, tmp3;
    tmp = fmax(fmax(side_1, side_2), side_3);
    tmp2 = fmin(fmax(side_1, side_2), side_3);
    tmp3 = fmin(side_1, fmin(side_2, side_3));
    if (LDBL_MAX / tmp <= tmp || LDBL_MAX / tmp3 <= tmp3 || LDBL_MAX / tmp2 <= tmp2) {
        return trian_overflow;
    }

    if (fabs(tmp * tmp - tmp3 * tmp3 - tmp2 * tmp2) < EPS) {
        return trian_is_r_trian;
    }

    else {
        return trian_is_not_r_trian;
    }
}
