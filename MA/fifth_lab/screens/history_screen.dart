import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import '../bloc/history_cubit.dart';

class HistoryScreen extends StatelessWidget {
  const HistoryScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return BlocProvider(
      create: (context) => HistoryCubit()..loadCalculations(),
      child: Scaffold(
        appBar: AppBar(
          title: const Text('История расчетов'),
          backgroundColor: Colors.blueAccent,
        ),
        body: BlocBuilder<HistoryCubit, HistoryState>(
          builder: (context, state) {
            if (state.isLoading) {
              return const Center(child: CircularProgressIndicator());
            }
            if (state.error != null) {
              return Center(child: Text(state.error!, style: const TextStyle(color: Colors.red)));
            }
            if (state.calculations.isEmpty) {
              return const Center(child: Text('Нет сохраненных расчетов'));
            }
            return ListView.builder(
              padding: const EdgeInsets.all(16.0),
              itemCount: state.calculations.length,
              itemBuilder: (context, index) {
                final calc = state.calculations[index];
                return Card(
                  margin: const EdgeInsets.symmetric(vertical: 8.0),
                  child: ListTile(
                    title: Text(
                      'Скорость: ${calc.velocity.toStringAsFixed(2)} м/с',
                      style: const TextStyle(fontWeight: FontWeight.bold),
                    ),
                    subtitle: Text(
                      'Масса: ${calc.mass.toStringAsExponential(2)} кг\n'
                      'Радиус: ${calc.radius.toStringAsExponential(2)} м',
                    ),
                  ),
                );
              },
            );
          },
        ),
      ),
    );
  }
}