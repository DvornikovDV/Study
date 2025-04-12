import 'package:flutter/material.dart';
import 'comp_result_screen.dart';

class FirstScreen extends StatefulWidget {
  const FirstScreen({super.key});

  @override
  _FirstScreenState createState() => _FirstScreenState();
}

class _FirstScreenState extends State<FirstScreen> {
  final _formKey = GlobalKey<FormState>();
  final _massController = TextEditingController();
  final _radiusController = TextEditingController();
  bool _isAgreed = false;

  void _navigateToResult() {
    if (_formKey.currentState!.validate() && _isAgreed) {
      final mass = double.parse(_massController.text);
      final radius = double.parse(_radiusController.text);
      Navigator.push( 
      context,
      MaterialPageRoute(
        builder: (context) => SecondScreen(mass: mass, radius: radius),
      ),
    );
    } else if (!_isAgreed) {
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(content: Text('Пожалуйста, согласитесь на обработку данных')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Калькулятор космической скорости'),
        backgroundColor: Colors.blueAccent,
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Form(
          key: _formKey,
          child: Column(
            children: [
              TextFormField(
                controller: _massController,
                decoration: const InputDecoration(
                  labelText: 'Масса небесного тела (кг)',
                  border: OutlineInputBorder(),
                ),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Введите массу';
                  }
                  final numValue = double.tryParse(value);
                  if (numValue == null || numValue <= 0) {
                    return 'Введите положительное число';
                  }
                  return null;
                },
              ),
              const SizedBox(height: 16),
              TextFormField(
                controller: _radiusController,
                decoration: const InputDecoration(
                  labelText: 'Радиус небесного тела (м)',
                  border: OutlineInputBorder(),
                ),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value == null || value.isEmpty) {
                    return 'Введите радиус';
                  }
                  final numValue = double.tryParse(value);
                  if (numValue == null || numValue <= 0) {
                    return 'Введите положительное число';
                  }
                  return null;
                },
              ),
              const SizedBox(height: 16),
              CheckboxListTile(
                title: const Text('Согласие на обработку данных'),
                value: _isAgreed,
                onChanged: (value) {
                  setState(() {
                    _isAgreed = value ?? false;
                  });
                },
              ),
              const SizedBox(height: 16),
              ElevatedButton(
                onPressed: _navigateToResult,
                child: const Text('Рассчитать'),
              ),
            ],
          ),
        ),
      ),
    );
  }

  @override
  void dispose() {
    _massController.dispose();
    _radiusController.dispose();
    super.dispose();
  }
}