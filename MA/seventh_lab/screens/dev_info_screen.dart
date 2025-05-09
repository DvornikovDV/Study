import 'package:flutter/material.dart';

class DevInfoScreen extends StatelessWidget {
  const DevInfoScreen({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('About Developer')),
      body: const Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            /// 
            Text('Name: Дворников Даниил', style: TextStyle(fontSize: 18)),
            Text('Group: ИВТ-22', style: TextStyle(fontSize: 18)),
            Text('Contact: dvornikovdaniil1@gmail.com', style: TextStyle(fontSize: 18)),
          ],
        ),
      ),
    );
  }
}