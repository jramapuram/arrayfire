/*******************************************************
 * Copyright (c) 2015, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <gtest/gtest.h>
#include <arrayfire.h>
#include <af/dim4.hpp>
#include <af/traits.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <testHelpers.hpp>

using std::vector;
using namespace af;

template<typename T>
class Select : public ::testing::Test
{
};

typedef ::testing::Types<float, double, af::cfloat, af::cdouble, uint, int, intl, uintl, uchar, char> TestTypes;
TYPED_TEST_CASE(Select, TestTypes);

template<typename T>
void selectTest(const dim4 &dims)
{
    if (noDoubleTests<T>()) return;
    af::dtype ty = (af::dtype)af::dtype_traits<T>::af_type;

    array a = randu(dims, ty);
    array cond = randu(dims, ty) > constant(0.3, dims, ty);
    array b = randu(dims, ty);

    array c = select(cond, a, b);

    int num = (int)a.elements();

    std::vector<T> ha(num);
    std::vector<T> hb(num);
    std::vector<T> hc(num);
    std::vector<char> hcond(num);

    a.host(&ha[0]);
    b.host(&hb[0]);
    c.host(&hc[0]);
    cond.host(&hcond[0]);

    for (int i = 0; i < num; i++) {
        ASSERT_EQ(hc[i], hcond[i] ? ha[i] : hb[i]);
    }
}

template<typename T, bool is_right>
void selectScalarTest(const dim4 &dims)
{
    if (noDoubleTests<T>()) return;
    af::dtype ty = (af::dtype)af::dtype_traits<T>::af_type;

    array a = randu(dims, ty);
    array cond = randu(dims, ty) > constant(0.3, dims, ty);
    double b = 3;

    array c = is_right ? select(cond, a, b) : select(cond, b, a);

    int num = (int)a.elements();

    std::vector<T> ha(num);
    std::vector<T> hc(num);
    std::vector<char> hcond(num);

    a.host(&ha[0]);
    c.host(&hc[0]);
    cond.host(&hcond[0]);

    if (is_right) {
        for (int i = 0; i < num; i++) {
            ASSERT_EQ(hc[i], hcond[i] ? ha[i] : b);
        }
    } else {
        for (int i = 0; i < num; i++) {
            ASSERT_EQ(hc[i], hcond[i] ? b : ha[i]);
        }
    }
}

TYPED_TEST(Select, Simple)
{
    selectTest<TypeParam>(dim4(1024, 1024));
}

TYPED_TEST(Select, RightScalar)
{
    selectScalarTest<TypeParam, true>(dim4(1000, 1000));
}

TYPED_TEST(Select, LeftScalar)
{
    selectScalarTest<TypeParam, true>(dim4(1000, 1000));
}

TEST(Select, NaN)
{
    dim4 dims(1000, 1250);
    af::dtype ty = f32;

    array a = randu(dims, ty);
    a(seq(a.dims(0) / 2), span, span, span) = af::NaN;
    double b = 0;
    array c = select(isNaN(a), b, a);

    int num = (int)a.elements();

    std::vector<float> ha(num);
    std::vector<float> hc(num);

    a.host(&ha[0]);
    c.host(&hc[0]);

    for (int i = 0; i < num; i++) {
        ASSERT_EQ(hc[i], std::isnan(ha[i]) ? b : ha[i]);
    }
}
