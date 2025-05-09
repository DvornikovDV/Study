import 'package:flutter_bloc/flutter_bloc.dart';
import 'dart:math';
import '../data/database_helper.dart';

class CalculatorState {
  final String mass;
  final String radius;
  final bool isAgreed;
  final String? massError;
  final String? radiusError;
  final bool showAgreementError;
  final double? velocity;
  final bool isCalculating;

  CalculatorState({
    this.mass = '',
    this.radius = '',
    this.isAgreed = false,
    this.massError,
    this.radiusError,
    this.showAgreementError = false,
    this.velocity,
    this.isCalculating = false,
  });

  CalculatorState copyWith({
    String? mass,
    String? radius,
    bool? isAgreed,
    String? massError,
    String? radiusError,
    bool? showAgreementError,
    double? velocity,
    bool? isCalculating,
  }) {
    return CalculatorState(
      mass: mass ?? this.mass,
      radius: radius ?? this.radius,
      isAgreed: isAgreed ?? this.isAgreed,
      massError: massError ?? this.massError,
      radiusError: radiusError ?? this.radiusError,
      showAgreementError: showAgreementError ?? this.showAgreementError,
      velocity: velocity ?? this.velocity,
      isCalculating: isCalculating ?? this.isCalculating,
    );
  }
}

class CalculatorCubit extends Cubit<CalculatorState> {
  final DatabaseHelper _db;

  CalculatorCubit({DatabaseHelper? db}) : _db = db ?? DatabaseHelper.instance, super(CalculatorState());

  void updateMass(String mass) {
    emit(state.copyWith(mass: mass, massError: null, velocity: null));
  }

  void updateRadius(String radius) {
    emit(state.copyWith(radius: radius, radiusError: null, velocity: null));
  }

  void toggleAgreement(bool isAgreed) {
    emit(state.copyWith(isAgreed: isAgreed, showAgreementError: false, velocity: null));
  }

  void calculate() async {
    String? massError;
    String? radiusError;
    bool showAgreementError = !state.isAgreed;

    final massValue = double.tryParse(state.mass);
    if (state.mass.isEmpty) {
      massError = 'Введите массу';
    } else if (massValue == null || massValue <= 0) {
      massError = 'Введите положительное число';
    }

    final radiusValue = double.tryParse(state.radius);
    if (state.radius.isEmpty) {
      radiusError = 'Введите радиус';
    } else if (radiusValue == null || radiusValue <= 0) {
      radiusError = 'Введите положительное число';
    }

    emit(state.copyWith(
      massError: massError,
      radiusError: radiusError,
      showAgreementError: showAgreementError,
      isCalculating: massError == null && radiusError == null && !showAgreementError,
      velocity: null,
    ));

    if (massError == null && radiusError == null && !showAgreementError) {
      final velocity = _calculateFirstCosmicVelocity(massValue!, radiusValue!);
      emit(state.copyWith(
        velocity: velocity,
        isCalculating: false,
      ));
      await _saveCalculation(massValue, radiusValue, velocity);
    } else {
      emit(state.copyWith(isCalculating: false));
    }
  }

  double _calculateFirstCosmicVelocity(double mass, double radius) {
    const double G = 6.67430e-11;
    return sqrt(G * mass / radius);
  }

  Future<void> _saveCalculation(double mass, double radius, double velocity) async {
    try {
      await _db.insertCalculation(mass, radius, velocity);
    } catch (e) {
      print('Error saving calculation: $e');
    }
  }
}