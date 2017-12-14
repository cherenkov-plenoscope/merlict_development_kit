// Copyright 2014 Sebastian A. Mueller
#include <cmath>
#include "gtest/gtest.h"
#include "Core/Vec2.h"
#include "Tools/Tools.h"

class Vec2Test : public ::testing::Test {};

TEST_F(Vec2Test, NullVector) {
    Vec2 a = Vec2::ORIGIN;
    EXPECT_EQ(0.0, a.x);
    EXPECT_EQ(0.0, a.y);
    EXPECT_EQ(0.0, a.norm());
}

TEST_F(Vec2Test, UnitVectors) {
    Vec2 a;
    a = Vec2::UNIT_X;
    EXPECT_EQ(1.0, a.x);
    EXPECT_EQ(0.0, a.y);
    EXPECT_EQ(1.0, a.norm());
    a = Vec2::UNIT_Y;
    EXPECT_EQ(0.0, a.x);
    EXPECT_EQ(1.0, a.y);
    EXPECT_EQ(1.0, a.norm());
}

TEST_F(Vec2Test, ConstructorAndGetter) {
    double x, y;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            Vec2 v(x, y);
            EXPECT_EQ(x, v.x);
            EXPECT_EQ(y, v.y);
        }
    }
}

TEST_F(Vec2Test, SetterAndGetter) {
    double x, y;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            Vec2 v;
            v.set(x, y);
            EXPECT_EQ(x, v.x);
            EXPECT_EQ(y, v.y);
        }
    }
}

TEST_F(Vec2Test, EuclideanNorm) {
    double x, y;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            Vec2 v(x, y);
            EXPECT_EQ(sqrt(x*x+y*y), v.norm());
        }
    }
    Vec2 v;
    v.set(1.0, 0.0);
    EXPECT_EQ(1.0, v.norm());
    v.set(0.0, 1.0);
    EXPECT_EQ(1.0, v.norm());}

TEST_F(Vec2Test, Scalar_Product_unit_vectors) {
    const Vec2 x = Vec2::UNIT_X;
    const Vec2 y = Vec2::UNIT_Y;
    EXPECT_EQ(0.0, x*y);
    EXPECT_EQ(1.0, x*x);
}

TEST_F(Vec2Test, Scalar_Product) {
    double x1, x2, y1, y2;
    for (x1 = -2.0; x1 > 2.0; x1 = x1+.25) {
        for (y1 = -2.0; y1 > 2.0; y1 = y1+.25) {
            for (x2 = -2.0; x2 > 2.0; x2 = x2+.25) {
                for (y2 = -2.0; y2 > 2.0; y2 = y2+.25) {
                    const Vec2 v1(x1, y1);
                    const Vec2 v2(x2, y2);
                    EXPECT_EQ(x1*x2 + y1*y2, v1*v2);
                }
            }
        }
    }
}

TEST_F(Vec2Test, ScalarMultiplication) {
    double x, y, f;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            Vec2 a(x, y);
            for (f = -2.0; f > 2.0; f = f+.25) {
                EXPECT_EQ(x*f, (a*f).x);
                EXPECT_EQ(y*f, (a*f).y);
            }
        }
    }
}

TEST_F(Vec2Test, ScalarDiviation) {
    double x, y, f;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            Vec2 a(x, y);
            for (f = -2.0; f > 2.0; f = f+.25) {
                EXPECT_EQ(x/f, (a/f).x);
                EXPECT_EQ(y/f, (a/f).y);
            }
        }
    }
}

TEST_F(Vec2Test, distance_unit_x_to_unit_y) {
    Vec2 a = Vec2::UNIT_X;
    Vec2 b = Vec2::UNIT_Y;
    EXPECT_EQ(sqrt(2.0), a.distance_to(b));
}

TEST_F(Vec2Test, distance_to_itself) {
    Vec2 a(1.3, 3.7);
    EXPECT_EQ(0.0, a.distance_to(a));
}

TEST_F(Vec2Test, Operator_equals_expect_true) {
    Vec2 a(1.3, 4.2);
    Vec2 b(1.3, 4.2+1e-20);
    EXPECT_EQ(a, b);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST_F(Vec2Test, Operator_equals_expect_false) {
    Vec2 a(1.3, 4.2);
    Vec2 b(1.3, 4.2 + 2e-7);
    EXPECT_NE(a, b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
}

TEST_F(Vec2Test, normalize) {
    Vec2 a(1.0, 2.0);
    EXPECT_NE(1.0, a.norm());
    a.normalize();
    EXPECT_NEAR(1.0, a.norm(), 1e-9);

    a = a*2.0;
    EXPECT_NE(1.0, a.norm());

    a.normalize();
    EXPECT_EQ(1.0, a.norm());

    a = Vec2::ORIGIN;
    EXPECT_NE(1.0, a.norm());

    a.normalize();
    EXPECT_EQ(std::isnan(a.norm()), 1);
}

TEST_F(Vec2Test, angle_in_between) {
    Vec2 a = Vec2::UNIT_X;
    Vec2 b = Vec2::UNIT_X;

    EXPECT_EQ(0.0, a.angle_in_between(b));
    EXPECT_EQ(
        b.angle_in_between(a),
        a.angle_in_between(b));

    Vec2 c = Vec2::UNIT_X*5.0;
    EXPECT_NE(1.0, c.norm());

    Vec2 d = Vec2::UNIT_X*5.0;
    EXPECT_NE(1.0, d.norm());

    EXPECT_EQ(0.0, d.angle_in_between(c));
    EXPECT_EQ(
        c.angle_in_between(d),
        d.angle_in_between(c));

    Vec2 foo = Vec2::UNIT_X*5.0 + Vec2::UNIT_Y*5.0;
    EXPECT_NE(1.0, c.norm());

    Vec2 bar = Vec2::UNIT_X*5.0;
    EXPECT_NE(1.0, d.norm());

    EXPECT_NEAR(Deg2Rad(45.0) , foo.angle_in_between(bar), 1e-5);
}

TEST_F(Vec2Test, quadrant_encoding) {
    // x y sector
    // - -   0
    // - +   1
    // + -   2
    // + +   3
    Vec2 a;

    double p = +1.0;
    double n = -1.0;

    a.set(n, n);
    EXPECT_EQ(0u, a.get_quadrant());

    a.set(n, p);
    EXPECT_EQ(1u, a.get_quadrant());

    a.set(p, n);
    EXPECT_EQ(2u, a.get_quadrant());

    a.set(p, p);
    EXPECT_EQ(3u, a.get_quadrant());

    a.set(0.0, 0.0);
    EXPECT_EQ(0u, a.get_quadrant());
    a.set(0.0, p);
    EXPECT_EQ(1u, a.get_quadrant());
    a.set(p, 0.0);
    EXPECT_EQ(2u, a.get_quadrant());
    a.set(p, p);
    EXPECT_EQ(3u, a.get_quadrant());
}
