import 'package:flutter/material.dart';
import 'package:path_provider/path_provider.dart';
import 'dart:io';

class PhotoGalleryScreen extends StatelessWidget {
  const PhotoGalleryScreen({super.key});

  Future<List<File>> _getPhotos() async {
    final directory = await getExternalStorageDirectory();
    final photoDir = Directory('${directory!.path}/Pictures/WeatherPhotos');
    if (await photoDir.exists()) {
      return photoDir.listSync().whereType<File>().toList();
    }
    return [];
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Photo Gallery')),
      body: FutureBuilder<List<File>>(
        future: _getPhotos(),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return const Center(child: CircularProgressIndicator());
          }
          if (snapshot.hasError || snapshot.data!.isEmpty) {
            return const Center(child: Text('No photos found'));
          }
          final photos = snapshot.data!;
          return GridView.builder(
            gridDelegate: const SliverGridDelegateWithFixedCrossAxisCount(
              crossAxisCount: 2,
              crossAxisSpacing: 4,
              mainAxisSpacing: 4,
            ),
            itemCount: photos.length,
            itemBuilder: (context, index) {
              return Image.file(photos[index], fit: BoxFit.cover);
            },
          );
        },
      ),
    );
  }
}