import 'package:http/http.dart' as http;
import 'dart:convert';
import '../models/weather_data.dart';

class WeatherService {
  // Ключ не прячу, достался бесплатно
  static const String _apiKey = 'feef1d65a9cd4fe7ac00b2809b604eaa';
  static const String _baseUrl = 'https://api.weatherbit.io/v2.0/current';

  Future<WeatherData> getWeather(String city) async {
    final response = await http.get(
      Uri.parse('$_baseUrl?city=$city&key=$_apiKey'),
    );

    if (response.statusCode == 200) {
      final data = jsonDecode(response.body);
      final weather = data['data'][0];
      return WeatherData(
        city: city,
        temperature: weather['temp'].toDouble(),
        humidity: weather['rh'].toDouble(),
        dewPoint: 0.0, // Рассчитывается в Cubit
      );
    } else {
      throw Exception('Failed to load weather');
    }
  }
}