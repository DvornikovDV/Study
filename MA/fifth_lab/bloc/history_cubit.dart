import 'package:flutter_bloc/flutter_bloc.dart';
import '../data/database_helper.dart';

class Calculation {
  final int id;
  final double mass;
  final double radius;
  final double velocity;

  Calculation({
    required this.id,
    required this.mass,
    required this.radius,
    required this.velocity,
  });

  factory Calculation.fromMap(Map<String, dynamic> map) {
    return Calculation(
      id: map['id'],
      mass: map['mass'],
      radius: map['radius'],
      velocity: map['velocity'],
    );
  }
}

class HistoryState {
  final List<Calculation> calculations;
  final bool isLoading;
  final String? error;

  HistoryState({
    this.calculations = const [],
    this.isLoading = false,
    this.error,
  });

  HistoryState copyWith({
    List<Calculation>? calculations,
    bool? isLoading,
    String? error,
  }) {
    return HistoryState(
      calculations: calculations ?? this.calculations,
      isLoading: isLoading ?? this.isLoading,
      error: error ?? this.error,
    );
  }
}

class HistoryCubit extends Cubit<HistoryState> {
  final DatabaseHelper _db;

  HistoryCubit({DatabaseHelper? db}) : _db = db ?? DatabaseHelper.instance, super(HistoryState());

  Future<void> loadCalculations() async {
    emit(state.copyWith(isLoading: true, error: null));
    try {
      final maps = await _db.getCalculations();
      final calculations = maps.map((map) => Calculation.fromMap(map)).toList();
      print('Loaded ${calculations.length} calculations');
      emit(state.copyWith(
        calculations: calculations,
        isLoading: false,
      ));
    } catch (e) {
      print('Error loading calculations: $e');
      emit(state.copyWith(
        isLoading: false,
        error: 'Ошибка загрузки данных: $e',
      ));
    }
  }
}