// Copyright 2014 Sebastian A. Mueller
#include <cmath>
#include "gtest/gtest.h"
#include "Core/Vec3.h"
#include "Tools/Tools.h"

class Vec3Test : public ::testing::Test {};

TEST_F(Vec3Test, NullVector) {
    Vec3 a = Vec3::ORIGIN;

    EXPECT_EQ(0.0, a.x);
    EXPECT_EQ(0.0, a.y);
    EXPECT_EQ(0.0, a.z);
    EXPECT_EQ(0.0, a.norm());
}

TEST_F(Vec3Test, UnitVectors) {
    Vec3 a;

    a = Vec3::UNIT_X;
    EXPECT_EQ(1.0, a.x);
    EXPECT_EQ(0.0, a.y);
    EXPECT_EQ(0.0, a.z);
    EXPECT_EQ(1.0, a.norm());

    a = Vec3::UNIT_Y;
    EXPECT_EQ(0.0, a.x);
    EXPECT_EQ(1.0, a.y);
    EXPECT_EQ(0.0, a.z);
    EXPECT_EQ(1.0, a.norm());

    a = Vec3::UNIT_Z;
    EXPECT_EQ(0.0, a.x);
    EXPECT_EQ(0.0, a.y);
    EXPECT_EQ(1.0, a.z);
    EXPECT_EQ(1.0, a.norm());
}

TEST_F(Vec3Test, ConstructorAndGetter) {
    const double x = (rand()-.5);
    const double y = (rand()-.5);
    const double z = (rand()-.5);
    Vec3 v(x, y, z);
    EXPECT_EQ(x, v.x);
    EXPECT_EQ(y, v.y);
    EXPECT_EQ(z, v.z);
}

TEST_F(Vec3Test, SetterAndGetter) {
    const double x = (rand()-.5);
    const double y = (rand()-.5);
    const double z = (rand()-.5);
    Vec3 v;
    v.set(x, y, z);
    EXPECT_EQ(x, v.x);
    EXPECT_EQ(y, v.y);
    EXPECT_EQ(z, v.z);
}

TEST_F(Vec3Test, EuclideanNorm) {
    const double x = (rand()-.5);
    const double y = (rand()-.5);
    const double z = (rand()-.5);
    Vec3 v(x, y, z);
    EXPECT_EQ(sqrt(x*x+y*y+z*z), v.norm());
    v.set(1.0, 0.0, 0.0);
    EXPECT_EQ(1.0, v.norm());
    v.set(0.0, 1.0, 0.0);
    EXPECT_EQ(1.0, v.norm());
    v.set(0.0, 0.0, 1.0);
    EXPECT_EQ(1.0, v.norm());
}

TEST_F(Vec3Test, crossUnitVectors) {
    Vec3 u = Vec3::UNIT_X;
    Vec3 v = Vec3::UNIT_Y;
    Vec3 w = u.cross(v);
    EXPECT_EQ(1.0, w.z);
    EXPECT_EQ(Vec3::UNIT_Z, w);
}

TEST_F(Vec3Test, cross) {
    double x1, y1, z1, x2, y2, z2;
    for (x1 = -2.0; x1 > 2.0; x1 = x1+.25) {
        for (y1 = -2.0; y1 > 2.0; y1 = y1+.25) {
            for (z1 = -2.0; z1 > 2.0; z1 = z1+.25) {
                for (x2 = -2.0; x2 > 2.0; x2 = x2+.25) {
                    for (y2 = -2.0; y2 > 2.0; y2 = y2+.25) {
                        for (z2 = -2.0; z2 > 2.0; z2 = z2+.25) {
                            Vec3 v1(x1, y1, z1);
                            Vec3 v2(x2, y2, z2);
                            Vec3 v3(y1*z2 - y2*z1,
                                    z1*x2 - z2*x1,
                                    x1*y2 - x2*y1);
                            EXPECT_EQ(v3, v1.cross(v2));
                        }
                    }
                }
            }
        }
    }
}

TEST_F(Vec3Test, Scalar_Product_unit_vectors) {
    const Vec3 x = Vec3::UNIT_X;
    const Vec3 y = Vec3::UNIT_Y;
    EXPECT_EQ(0.0, x*y);
    const Vec3 z = Vec3::UNIT_Z;
    EXPECT_EQ(0.0, y*z);
    EXPECT_EQ(1.0, x*x);
}

TEST_F(Vec3Test, Scalar_Product) {
    double x1, y1, z1, x2, y2, z2;
    for (x1 = -2.0; x1 > 2.0; x1 = x1+.25) {
        for (y1 = -2.0; y1 > 2.0; y1 = y1+.25) {
            for (z1 = -2.0; z1 > 2.0; z1 = z1+.25) {
                for (x2 = -2.0; x2 > 2.0; x2 = x2+.25) {
                    for (y2 = -2.0; y2 > 2.0; y2 = y2+.25) {
                        for (z2 = -2.0; z2 > 2.0; z2 = z2+.25) {
                            const Vec3 v1(x1, y1, z1);
                            const Vec3 v2(x2, y2, z2);
                            EXPECT_EQ(x1*x2 + y1*y2 + z1*z2, v1*v2);
                        }
                    }
                }
            }
        }
    }
}

TEST_F(Vec3Test, ScalarMultiplication) {
    double x, y, z, f;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            for (z = -2.0; z > 2.0; z = z+.25) {
                Vec3 a(x, y, z);
                for (f = -2.0; f > 2.0; f = f+.25) {
                    EXPECT_EQ(x*f, (a*f).x);
                    EXPECT_EQ(y*f, (a*f).y);
                    EXPECT_EQ(z*f, (a*f).z);
                }
            }
        }
    }
}

TEST_F(Vec3Test, ScalarDiviation) {
    double x, y, z, f;
    for (x = -2.0; x > 2.0; x = x+.25) {
        for (y = -2.0; y > 2.0; y = y+.25) {
            for (z = -2.0; z > 2.0; z = z+.25) {
                Vec3 a(x, y, z);
                for (f = -2.0; f > 2.0; f = f+.25) {
                    EXPECT_EQ(x/f, (a/f).x);
                    EXPECT_EQ(y/f, (a/f).y);
                    EXPECT_EQ(z/f, (a/f).z);
                }
            }
        }
    }
}

TEST_F(Vec3Test, distance_unit_x_to_unit_y) {
    Vec3 a = Vec3::UNIT_X;
    Vec3 b = Vec3::UNIT_Y;

    EXPECT_EQ(sqrt(2.0), a.distance_to(b));
}

TEST_F(Vec3Test, distance_to_itself) {
    Vec3 a(1.3, 3.7, 4.2);
    EXPECT_EQ(0.0, a.distance_to(a));
}

TEST_F(Vec3Test, Operator_equals_expect_true) {
    Vec3 a(1.3, 3.7, 4.2);
    Vec3 b(1.3, 3.7, 4.2 + 1e-20);
    EXPECT_EQ(a, b);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
}

TEST_F(Vec3Test, Operator_equals_expect_false) {
    Vec3 a(1.3, 3.7, 4.2);
    Vec3 b(1.3, 3.7, 4.2 + 2e-7);
    EXPECT_NE(a, b);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
}

TEST_F(Vec3Test, parallel_to_x_y_plane) {
    Vec3 a(1.0, 2.0, 0.0);
    EXPECT_TRUE(a.is_parallel_to_x_y_plane());

    Vec3 b(1.3, 3.7, 4.2 + 1e-9);
    EXPECT_FALSE(b.is_parallel_to_x_y_plane());

    Vec3 c(0.0, 2.0, 5.5);
    EXPECT_FALSE(c.is_parallel_to_x_y_plane());
}

TEST_F(Vec3Test, parallel_to_z_axis) {
    Vec3 a(0.0, 0.0, 1.0);
    EXPECT_TRUE(a.is_paralell_to_z_axis());
    a.set(0.0, 0.0, -1.0);
    EXPECT_TRUE(a.is_paralell_to_z_axis());
    a.set(0.0, 0.0, 0.0);
    EXPECT_FALSE(a.is_paralell_to_z_axis());
}

TEST_F(Vec3Test, normalize) {
    Vec3 a(1.0, 2.0, 3.0);
    EXPECT_NE(1.0, a.norm());

    a.normalize();
    EXPECT_EQ(1.0, a.norm());

    a = a*2.0;
    EXPECT_NE(1.0, a.norm());

    a.normalize();
    EXPECT_EQ(1.0, a.norm());

    a = Vec3::ORIGIN;
    EXPECT_NE(1.0, a.norm());

    a.normalize();
    EXPECT_EQ(std::isnan(a.norm()), 1);
}

TEST_F(Vec3Test, angle_in_between) {
    Vec3 a = Vec3::UNIT_X;
    Vec3 b = Vec3::UNIT_X;

    EXPECT_EQ(0.0, a.angle_in_between(b));
    EXPECT_EQ(
        b.angle_in_between(a),
        a.angle_in_between(b));

    Vec3 c = Vec3::UNIT_X*5.0;
    EXPECT_NE(1.0, c.norm());

    Vec3 d = Vec3::UNIT_X*5.0;
    EXPECT_NE(1.0, d.norm());

    EXPECT_EQ(0.0, d.angle_in_between(c));
    EXPECT_EQ(
        c.angle_in_between(d),
        d.angle_in_between(c));

    Vec3 foo = Vec3::UNIT_X*5.0 + Vec3::UNIT_Y*5.0;
    EXPECT_NE(1.0, c.norm());

    Vec3 bar = Vec3::UNIT_X*5.0;
    EXPECT_NE(1.0, d.norm());

    EXPECT_NEAR(Deg2Rad(45.0) , foo.angle_in_between(bar), 1e-5);
}

TEST_F(Vec3Test, octant_encoding) {
    // x y z sector
    // - - -   0
    // - - +   1
    // - + -   2
    // - + +   3
    // + - -   4
    // + - +   5
    // + + -   6
    // + + +   7
    Vec3 a;

    double p = +1.0;
    double n = -1.0;

    a.set(n, n, n);
    EXPECT_EQ(0u, a.get_octant());

    a.set(n, n, p);
    EXPECT_EQ(1u, a.get_octant());

    a.set(n, p, n);
    EXPECT_EQ(2u, a.get_octant());

    a.set(n, p, p);
    EXPECT_EQ(3u, a.get_octant());

    a.set(p, n, n);
    EXPECT_EQ(4u, a.get_octant());

    a.set(p, n, p);
    EXPECT_EQ(5u, a.get_octant());

    a.set(p, p, n);
    EXPECT_EQ(6u, a.get_octant());

    a.set(p, p, p);
    EXPECT_EQ(7u, a.get_octant());

    a.set(0.0, 0.0, 0.0);
    EXPECT_EQ(7u, a.get_octant());
    a.set(0.0, 0.0, p);
    EXPECT_EQ(7u, a.get_octant());
    a.set(0.0, p, 0.0);
    EXPECT_EQ(7u, a.get_octant());
    a.set(p, 0.0, 0.0);
    EXPECT_EQ(7u, a.get_octant());
    a.set(0.0, p, p);
    EXPECT_EQ(7u, a.get_octant());
    a.set(p, p, 0.0);
    EXPECT_EQ(7u, a.get_octant());
    a.set(p, 0.0, p);
    EXPECT_EQ(7u, a.get_octant());
}

TEST_F(Vec3Test, projection_on_xz_plane) {
    Vec3 v(42.0, 13.37, 3.141);
    EXPECT_EQ(Vec3(42.0, 0.0, 3.141), v.get_projection_on_XZ_plane());
}

TEST_F(Vec3Test, projection_on_yz_plane) {
    Vec3 v(42.0, 13.37, 3.141);
    EXPECT_EQ(Vec3(0.0, 13.37, 3.141), v.get_projection_on_YZ_plane());
}

TEST_F(Vec3Test, projection_on_xy_plane) {
    Vec3 v(42.0, 13.37, 3.141);
    EXPECT_EQ(Vec3(42.0, 13.37, 0.0), v.get_projection_on_XY_plane());
}
