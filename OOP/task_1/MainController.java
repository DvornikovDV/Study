package org.dvornikovdv.oop_application;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;

public class MainController { // Класс контроллера
    @FXML
    private ComboBox<String> ComboBox_operations; // Выпадающий список

    @FXML
    private Label Label_result; // Текст для вывода результата

    @FXML
    private TextField TextField_vector_X0; // Поле для ввода поля X первого вектора

    @FXML
    private TextField TextField_vector_X1; // Поле для ввода поля Y первого вектора

    @FXML
    private TextField TextField_vector_Y0; // Поле для ввода поля X второго вектора

    @FXML
    private TextField TextField_vector_Y1; // Поле для ввода поля Y второго вектора

    public Vector vector1 = new Vector( 1,1);
    public Vector vector2 = new Vector( 1,1);


    // Список элементов выпадающего списка ComboBox_operations
    ObservableList<String> operations = FXCollections.observableArrayList("Length (1 vector)", "Addition (2 vectors)", "Subtraction (2 vectors)", "Scalar multiply (2 vectors)", "The angle between 0X (1 vector)", "The angle between 0Y (1 vector)");

    // Инициализация выпадающего списка ComboBox_operations списком operations
    @FXML
    private void initialize(){
        ComboBox_operations.setItems(operations);

        // Значение по умолчанию
        ComboBox_operations.setValue("Length (1 vector)");
    }

    // Обработчик событий для нажатия на кнопку Run
    @FXML
    void on_calculate(ActionEvent event) {
        {
            // Ввод значений
            vector1.X = Float.parseFloat(TextField_vector_X0.getText());
            vector1.Y = Float.parseFloat(TextField_vector_Y0.getText());

            vector2.X = Float.parseFloat(TextField_vector_X1.getText());
            vector2.Y = Float.parseFloat(TextField_vector_Y1.getText());
        }

        // Строка для выбора операции над вектором
        String temp_string = ComboBox_operations.getValue();
        switch (temp_string){
            case "Length (1 vector)":
                Label_result.setText(Double.toString(vector1.length_v()));
                break;

            case "Addition (2 vectors)":
                vector1.add_v(vector2);
                Label_result.setText(vector1.toString());
                break;

            case "Subtraction (2 vectors)":
                vector1.dif_v(vector2);
                Label_result.setText(vector1.toString());
                break;

            case "Scalar multiply (2 vectors)":
                vector1.scalar_multiply(vector2);
                Label_result.setText(vector1.toString());
                break;

            case "The angle between 0X (1 vector)":
                Label_result.setText(Double.toString(vector1.angle_OX()));;
                break;

            case "The angle between 0Y (1 vector)":
                Label_result.setText(Double.toString(vector1.angle_OY()));;
                break;
        }
    }
}