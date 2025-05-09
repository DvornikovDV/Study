import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:hive_flutter/hive_flutter.dart';
import 'cubits/weather_cubit.dart';
import 'cubits/dew_point_cubit.dart';
import 'screens/home_screen.dart';
import 'services/weather_service.dart';
import 'services/storage_service.dart'; // Добавлен импорт

import 'models/weather_data.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Hive.initFlutter();
  Hive.registerAdapter(WeatherDataAdapter());
  await Hive.openBox<WeatherData>('weather_history');
  await Hive.openBox<WeatherData>('dew_point_history');
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MultiBlocProvider(
      providers: [
        BlocProvider(
          create: (context) => WeatherCubit(
            WeatherService(),
            StorageService(),
            DewPointCubit(StorageService()),
          )..fetchWeather('Moscow'),
        ),
        BlocProvider(
          create: (context) => DewPointCubit(StorageService()),
        ),
      ],
      child: MaterialApp(
        title: 'Weather App',
        theme: ThemeData(primarySwatch: Colors.blue),
        home: const HomeScreen(),
      ),
    );
  }
}