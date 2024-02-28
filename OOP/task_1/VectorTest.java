package org.dvornikovdv.oop_application;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

class VectorTest {

    //** Погрешность вычислений */
    private static final double eps = 0.00001;

    @Test
    void length_v() {
        Vector v = new Vector(3, 4);
        double actual = v.length_v();
        double expected = 5;
        assertEquals(expected, actual);
    }

    @Test
    void add_v() {
        Vector actual = new Vector(3.1, 4.2);
        Vector v1 = new Vector(3.1, 4.3);
        Vector expected = new Vector(6.2, 8.5);

        actual.add_v(v1);

        assertEquals(expected.X, actual.X);
        assertEquals(expected.Y, actual.Y);
    }

    @Test
    void dif_v() {
        Vector actual = new Vector(6.2, 8.5);
        Vector v1 = new Vector(3.1, 4.3);
        Vector expected = new Vector(3.1, 4.2);

        actual.dif_v(v1);

        assert(expected.X - actual.X < eps);
        assert(expected.Y - actual.Y < eps);
    }

    @Test
    void scalar_multiply() {
        Vector actual = new Vector(6.2, 8.5);
        Vector v1 = new Vector(3.1, 4.3);
        Vector expected = new Vector(19.22, 35.7);

        actual.scalar_multiply(v1);

        assert(expected.X - actual.X < eps );
        assert(expected.Y - actual.Y < eps );
    }

    @Test
    void angle_OX() {
        Vector v = new Vector(3.1, 4.3);
        double actual = v.angle_OX();
        double expected = 54.21102654081667;
        assert(expected - actual < eps & expected - actual > -eps );
    }

    @Test
    void angle_OY() {
        Vector v = new Vector(3.1, 4.3);
        double actual = v.angle_OY();
        double expected = 35.78897345918333;
        assert(expected - actual < eps & expected - actual > -eps);
    }
}