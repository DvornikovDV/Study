import 'package:flutter/material.dart';
import 'dart:math';

class SecondScreen extends StatelessWidget {
  final double mass;
  final double radius;

  const SecondScreen({super.key, required this.mass, required this.radius});

  double calculateFirstCosmicVelocity() {
    const double G = 6.67430e-11; // Гравитационная постоянная (м³ кг⁻¹ с⁻²)
    // Формула: v = sqrt(G * M / r)
    return sqrt(G * mass / radius);
  }

  @override
  Widget build(BuildContext context) {
    final velocity = calculateFirstCosmicVelocity();

    return Scaffold(
      appBar: AppBar(
        title: const Text('Результат расчета'),
        backgroundColor: Colors.blueAccent,
      ),
      body: Center(
        child: Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              const Text(
                'Первая космическая скорость:',
                style: TextStyle(fontSize: 20),
              ),
              const SizedBox(height: 16),
              Text(
                '${velocity.toStringAsFixed(2)} м/с',
                style: const TextStyle(
                  fontSize: 24,
                  fontWeight: FontWeight.bold,
                  color: Colors.blueAccent,
                ),
              ),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: () {
                  Navigator.pop(context);
                },
                child: const Text('Назад'),
              ),
            ],
          ),
        ),
      ),
    );
  }
}