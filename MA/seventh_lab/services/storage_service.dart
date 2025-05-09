import 'package:hive/hive.dart';
import '../models/weather_data.dart';

class StorageService {
  Future<void> saveWeather(WeatherData weather) async {
    final box = Hive.box<WeatherData>('weather_history');
    await box.add(weather);
  }

  Future<void> saveDewPoint(WeatherData weather) async {
    final box = Hive.box<WeatherData>('dew_point_history');
    await box.add(weather);
  }
}