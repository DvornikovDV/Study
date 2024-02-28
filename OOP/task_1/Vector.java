package org.dvornikovdv.oop_application;
//todo: наслед от obj

/** Класс Вектор */
public class Vector {
    // Компоненты вектора
    public double X;
    public double Y;

    /** Коструктор без параметров */
    Vector() { this.X = 0; this.Y = 0; }

    /** Коструктор с параметрами x и y */
    Vector(double x, double y){ this.X = x; this.Y = y; }

    Vector(Vector another) {
       this(another.X, another.Y);
    }

    /** Возвращает длину вектора */
    public double length_v(){
        return Math.sqrt(Math.pow(this.X, 2) + Math.pow(this.Y, 2));
    }

    /** Складывает компоненты векторов
     * принимает объект класса Vector */
    public void add_v(Vector vector){
        this.X += vector.X;
        this.Y += vector.Y;
    }

    /** Вычисляет разность компонент векторов
     * принимает объект класса Vector */
    public void dif_v(Vector vector){
        this.X -= vector.X;
        this.Y -= vector.Y;
    }

    /** Вычисляет произведение компонент векторов
     * принимает объект класса Vector */
    public void scalar_multiply(Vector vector){
        this.X *= vector.X;
        this.Y *= vector.Y;
    }

    /** Возвращает угол между вектором и осью 0X */
    public double angle_OX() throws IllegalArgumentException {
        if( this.X == 0 & this.Y == 0){
            throw new IllegalArgumentException("Задан нулевой вектор");
        }

        return ( Math.acos(this.X / length_v()) * 180 / Math.PI );
    }

    /** Возвращает угол между вектором и осью 0Y */
    public double angle_OY() throws IllegalArgumentException {
        if( this.X == 0 & this.Y == 0){
            throw new IllegalArgumentException("Задан нулевой вектор");
        }

        return ( Math.acos(this.Y / length_v()) * 180 / Math.PI );
    }

    /** Преобразование полей класса в строку */
    @Override
    public String toString(){
        return "( %s; %s)".formatted(Double.toString(this.X), Double.toString(this.Y));
    }
}
