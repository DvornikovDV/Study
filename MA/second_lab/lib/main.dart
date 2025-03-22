import 'package:flutter/material.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: Text('Дворников Даниил Васильевич'),
        ),
        body: MyHomePage(),
      ),
    );
  }
}

class MyHomePage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: <Widget>[

          Wrap(
            spacing: 8.0,
            children: [
              Container(
                padding: EdgeInsets.all(8.0),
                color: Colors.deepOrange,
                child: Text('Дата рождения: 10.12.2004г.'),
              ),

              Container(
                padding: EdgeInsets.all(8.0),
                color: Colors.deepPurple,
                child: Text('Группа: ИВТ-22'),
              ),

              Container(
                padding: EdgeInsets.all(8.0),
                color: Colors.amber,
                child: Text('Курс: Третий'),
              ),

              Container(
                padding: EdgeInsets.all(8.0),
                color: Colors.teal,
                child: Text('Контейнер для количества, но не для качества'),
              ),
              
            ],
          ),


          // Здесь будут ваши виджеты
        ],
      ),
    );
  }
}
