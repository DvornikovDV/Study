import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import '../bloc/calc_cubit.dart';
import 'history_screen.dart';

class CalculatorScreen extends StatelessWidget {
  const CalculatorScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Калькулятор космической скорости'),
        backgroundColor: Colors.blueAccent,
        leading: IconButton(
          icon: const Icon(Icons.history),
          onPressed: () {
            Navigator.push(
              context,
              MaterialPageRoute(builder: (context) => const HistoryScreen()),
            );
          },
          tooltip: 'История расчетов',
        ),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: BlocBuilder<CalculatorCubit, CalculatorState>(
          builder: (context, state) {
            final cubit = context.read<CalculatorCubit>();

            return Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Масса небесного тела (кг)',
                    border: const OutlineInputBorder(),
                    errorText: state.massError,
                  ),
                  keyboardType: TextInputType.number,
                  onChanged: cubit.updateMass,
                ),
                const SizedBox(height: 16),
                TextFormField(
                  decoration: InputDecoration(
                    labelText: 'Радиус небесного тела (м)',
                    border: const OutlineInputBorder(),
                    errorText: state.radiusError,
                  ),
                  keyboardType: TextInputType.number,
                  onChanged: cubit.updateRadius,
                ),
                const SizedBox(height: 16),
                CheckboxListTile(
                  title: const Text('Согласие на обработку данных'),
                  value: state.isAgreed,
                  onChanged: (value) => cubit.toggleAgreement(value ?? false),
                ),
                if (state.showAgreementError)
                  const Padding(
                    padding: EdgeInsets.only(top: 8.0),
                    child: Text(
                      'Пожалуйста, согласитесь на обработку данных',
                      style: TextStyle(color: Colors.red),
                    ),
                  ),
                const SizedBox(height: 16),
                ElevatedButton(
                  onPressed: state.isCalculating ? null : cubit.calculate,
                  child: const Text('Рассчитать'),
                ),
                const SizedBox(height: 16),
                if (state.isCalculating)
                  const Center(child: CircularProgressIndicator())
                else if (state.velocity != null)
                  Center(
                    child: Text(
                      'Первая космическая скорость: ${state.velocity!.toStringAsFixed(2)} м/с',
                      style: const TextStyle(
                        fontSize: 18,
                        fontWeight: FontWeight.bold,
                        color: Colors.blueAccent,
                      ),
                      textAlign: TextAlign.center,
                    ),
                  ),
              ],
            );
          },
        ),
      ),
    );
  }
}