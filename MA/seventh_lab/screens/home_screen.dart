import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:hive_flutter/hive_flutter.dart';
import '../cubits/weather_cubit.dart';
import '../models/weather_data.dart';
import 'dev_info_screen.dart';
import 'dew_point_screen.dart';
import 'camera_screen.dart';

class HomeScreen extends StatelessWidget {
  const HomeScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final TextEditingController cityController = TextEditingController();

    return Scaffold(
      appBar: AppBar(
        title: const Text('Weather App'),
        actions: [
          IconButton(
            icon: const Icon(Icons.info),
            onPressed: () => Navigator.push(
              context,
              MaterialPageRoute(builder: (_) => const DevInfoScreen()),
            ),
          ),
          IconButton(
            icon: const Icon(Icons.calculate),
            onPressed: () => Navigator.push(
              context,
              MaterialPageRoute(builder: (_) => const DewPointScreen()),
            ),
          ),
          IconButton(
            icon: const Icon(Icons.camera),
            onPressed: () => Navigator.push(
              context,
              MaterialPageRoute(builder: (_) => const CameraScreen()),
            ),
          ),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            TextField(
              controller: cityController,
              decoration: const InputDecoration(
                labelText: 'Enter city',
                border: OutlineInputBorder(),
              ),
            ),
            const SizedBox(height: 10),
            ElevatedButton(
              onPressed: () {
                if (cityController.text.isNotEmpty) {
                  BlocProvider.of<WeatherCubit>(context)
                      .fetchWeather(cityController.text);
                }
              },
              child: const Text('Get Weather'),
            ),
            const SizedBox(height: 20),
            BlocBuilder<WeatherCubit, WeatherState>(
              builder: (context, state) {
                if (state is WeatherLoading) {
                  return const CircularProgressIndicator();
                } else if (state is WeatherLoaded) {
                  return Text(
                    'City: ${state.weather.city}\n'
                    'Temperature: ${state.weather.temperature}°C\n'
                    'Humidity: ${state.weather.humidity}%\n'
                    'Dew Point: ${state.weather.dewPoint}°C',
                  );
                } else if (state is WeatherError) {
                  return Text(state.message);
                }
                return const SizedBox();
              },
            ),
            const SizedBox(height: 20),
            const Text('Weather History', style: TextStyle(fontSize: 18)),
            Expanded(
              child: ValueListenableBuilder<Box<WeatherData>>(
                valueListenable:
                    Hive.box<WeatherData>('weather_history').listenable(),
                builder: (context, box, _) {
                  final history = box.values.toList();
                  return ListView.builder(
                    itemCount: history.length,
                    itemBuilder: (context, index) {
                      final weather = history[index];
                      return ListTile(
                        title: Text(weather.city),
                        subtitle: Text(
                          'Temp: ${weather.temperature}°C, '
                          'Humidity: ${weather.humidity}%, '
                          'Dew Point: ${weather.dewPoint}°C',
                        ),
                      );
                    },
                  );
                },
              ),
            ),
          ],
        ),
      ),
    );
  }
}