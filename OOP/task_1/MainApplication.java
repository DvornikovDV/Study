package org.dvornikovdv.oop_application;

import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.stage.Stage;

import java.io.IOException;

// Класс с обработчиками событий главного окна
public class MainApplication extends Application {
    // Метод создания окна приложения
    @Override
    public void start(Stage stage) throws IOException {
        FXMLLoader fxmlLoader = new FXMLLoader(MainApplication.class.getResource("main-view.fxml"));

        Scene scene = new Scene(fxmlLoader.load(), 500, 300);

        stage.setTitle("Vector");
        stage.setScene(scene);
        stage.show();
    }

    // Входная точка в программу, обязательный метод
    public static void main(String[] args) {
        launch();
    }
}