import 'package:hive/hive.dart';

part 'weather_data.g.dart';

@HiveType(typeId: 1)
class WeatherData {
  @HiveField(0)
  final String city;

  @HiveField(1)
  final double temperature;

  @HiveField(2)
  final double humidity;

  @HiveField(3)
  final double dewPoint;

  @HiveField(4)
  final String? photoPath;

  WeatherData({
    required this.city,
    required this.temperature,
    required this.humidity,
    required this.dewPoint,
    this.photoPath,
  });
}