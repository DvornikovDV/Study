import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:hive_flutter/hive_flutter.dart';
import '../cubits/dew_point_cubit.dart';
import '../models/weather_data.dart';

class DewPointScreen extends StatelessWidget {
  const DewPointScreen({super.key});

  @override
  Widget build(BuildContext context) {
    final TextEditingController tempController = TextEditingController();
    final TextEditingController humidityController = TextEditingController();

    return Scaffold(
      appBar: AppBar(title: const Text('Dew Point Calculator')),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            TextField(
              controller: tempController,
              decoration: const InputDecoration(
                labelText: 'Temperature (°C)',
                border: OutlineInputBorder(),
              ),
              keyboardType: TextInputType.number,
            ),
            const SizedBox(height: 10),
            TextField(
              controller: humidityController,
              decoration: const InputDecoration(
                labelText: 'Humidity (%)',
                border: OutlineInputBorder(),
              ),
              keyboardType: TextInputType.number,
            ),
            const SizedBox(height: 10),
            ElevatedButton(
              onPressed: () {
                final temperature = double.tryParse(tempController.text);
                final humidity = double.tryParse(humidityController.text);
                if (temperature != null && humidity != null) {
                  BlocProvider.of<DewPointCubit>(context).calculateAndSaveDewPoint(
                    temperature: temperature,
                    humidity: humidity,
                    city: 'Manual Input',
                  );
                }
              },
              child: const Text('Calculate Dew Point'),
            ),
            const SizedBox(height: 20),
            BlocBuilder<DewPointCubit, DewPointState>(
              builder: (context, state) {
                if (state is DewPointCalculated) {
                  return Text('Dew Point: ${state.weather.dewPoint}°C');
                } else if (state is DewPointError) {
                  return Text(state.message);
                }
                return const SizedBox();
              },
            ),
            const SizedBox(height: 20),
            const Text('Calculation History', style: TextStyle(fontSize: 18)),
            Expanded(
              child: ValueListenableBuilder<Box<WeatherData>>(
                valueListenable: Hive.box<WeatherData>('dew_point_history').listenable(),
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