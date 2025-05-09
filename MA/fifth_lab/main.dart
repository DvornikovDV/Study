import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'bloc/calc_cubit.dart';
import 'screens/calc_screen.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    print('Building MyApp');
    return MaterialApp(
      title: 'Калькулятор космической скорости',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: BlocProvider(
        create: (context) {
          print('Creating CalculatorCubit');
          return CalculatorCubit();
        },
        child: const CalculatorScreen(),
      ),
    );
  }
}