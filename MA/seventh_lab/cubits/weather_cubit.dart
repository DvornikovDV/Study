import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:equatable/equatable.dart';
import '../models/weather_data.dart';
import '../services/weather_service.dart';
import '../services/storage_service.dart'; // Добавлен импорт
import 'dew_point_cubit.dart';

abstract class WeatherState extends Equatable {
  const WeatherState();
  @override
  List<Object> get props => [];
}

class WeatherInitial extends WeatherState {}

class WeatherLoading extends WeatherState {}

class WeatherLoaded extends WeatherState {
  final WeatherData weather;
  const WeatherLoaded(this.weather);
  @override
  List<Object> get props => [weather];
}

class WeatherError extends WeatherState {
  final String message;
  const WeatherError(this.message);
  @override
  List<Object> get props => [message];
}

class WeatherCubit extends Cubit<WeatherState> {
  final WeatherService _weatherService;
  final StorageService _storageService;
  final DewPointCubit _dewPointCubit;

  WeatherCubit(this._weatherService, this._storageService, this._dewPointCubit)
      : super(WeatherInitial());

  Future<void> fetchWeather(String city) async {
    emit(WeatherLoading());
    try {
      final weather = await _weatherService.getWeather(city);
      final dewPoint = _dewPointCubit.calculateDewPoint(
        temperature: weather.temperature,
        humidity: weather.humidity,
      );
      final weatherData = WeatherData(
        city: city,
        temperature: weather.temperature,
        humidity: weather.humidity,
        dewPoint: dewPoint,
      );
      await _storageService.saveWeather(weatherData);
      emit(WeatherLoaded(weatherData));
    } catch (e) {
      emit(WeatherError('Failed to fetch weather: $e'));
    }
  }

  Future<void> savePhotoPath(String path) async {
    final currentState = state;
    if (currentState is WeatherLoaded) {
      final updatedWeather = WeatherData(
        city: currentState.weather.city,
        temperature: currentState.weather.temperature,
        humidity: currentState.weather.humidity,
        dewPoint: currentState.weather.dewPoint,
        photoPath: path,
      );
      await _storageService.saveWeather(updatedWeather);
      emit(WeatherLoaded(updatedWeather));
    }
  }
}