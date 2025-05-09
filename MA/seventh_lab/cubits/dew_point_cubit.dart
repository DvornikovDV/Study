import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:equatable/equatable.dart';
import '../models/weather_data.dart';
import '../services/storage_service.dart';
import 'dart:math';

abstract class DewPointState extends Equatable {
  const DewPointState();
  @override
  List<Object> get props => [];
}

class DewPointInitial extends DewPointState {}

class DewPointCalculated extends DewPointState {
  final WeatherData weather;
  const DewPointCalculated(this.weather);
  @override
  List<Object> get props => [weather];
}

class DewPointError extends DewPointState {
  final String message;
  const DewPointError(this.message);
  @override
  List<Object> get props => [message];
}

class DewPointCubit extends Cubit<DewPointState> {
  final StorageService _storageService;

  DewPointCubit(this._storageService) : super(DewPointInitial());

  double calculateDewPoint({required double temperature, required double humidity}) {
    final a = (17.27 * temperature) / (237.7 + temperature);
    final b = log(humidity / 100) + a;
    return (237.7 * b) / (17.27 - b);
  }

  Future<void> calculateAndSaveDewPoint({
    required double temperature,
    required double humidity,
    required String city,
  }) async {
    try {
      final dewPoint = calculateDewPoint(temperature: temperature, humidity: humidity);
      final weatherData = WeatherData(
        city: city,
        temperature: temperature,
        humidity: humidity,
        dewPoint: dewPoint,
      );
      await _storageService.saveDewPoint(weatherData);
      emit(DewPointCalculated(weatherData));
    } catch (e) {
      emit(DewPointError('Failed to calculate dew point: $e'));
    }
  }
}