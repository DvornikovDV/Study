import 'package:flutter/material.dart';
import 'package:camera/camera.dart';
import 'package:path_provider/path_provider.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import 'package:permission_handler/permission_handler.dart';
import 'dart:io';
import '../cubits/weather_cubit.dart';
import 'photo_gallery_screen.dart';

class CameraScreen extends StatefulWidget {
  const CameraScreen({super.key});

  @override
  CameraScreenState createState() => CameraScreenState();
}

class CameraScreenState extends State<CameraScreen> {
  CameraController? _controller;
  Future<void>? _initializeControllerFuture;

  @override
  void initState() {
    super.initState();
    _initializeCamera();
  }

  Future<void> _initializeCamera() async {
    // Запрос разрешения на камеру
    if (await Permission.camera.request().isGranted) {
      final cameras = await availableCameras();
      _controller = CameraController(cameras[0], ResolutionPreset.medium);
      _initializeControllerFuture = _controller!.initialize();
      setState(() {});
    } else {
      if (mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text('Camera permission denied')),
        );
      }
    }
  }

  @override
  void dispose() {
    _controller?.dispose();
    super.dispose();
  }

  Future<String> _savePhoto() async {
    // Запрос разрешения на фото
    final photoPermission = await Permission.photos.request();
    if (photoPermission.isGranted) {
      final directory = await getExternalStorageDirectory();
      final appDir = Directory('${directory!.path}/Pictures/WeatherPhotos');
      if (!await appDir.exists()) {
        await appDir.create(recursive: true);
      }
      final path = '${appDir.path}/thermometer_${DateTime.now().millisecondsSinceEpoch}.jpg';
      await _controller!.takePicture().then((XFile file) {
        final savedFile = File(path);
        file.saveTo(savedFile.path);
        scanPath(path);
      });
      return path;
    } else {
      throw Exception('Photo permission denied: ${photoPermission.toString()}');
    }
  }

  Future<void> scanPath(String path) async {
    // Для Android 13+ уведомление медиа-сканера не требуется, так как используется общедоступная директория
    debugPrint('Photo saved at: $path');
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Take Thermometer Photo'),
        actions: [
          IconButton(
            icon: const Icon(Icons.photo_album),
            onPressed: () => Navigator.push(
              context,
              MaterialPageRoute(builder: (_) => const PhotoGalleryScreen()),
            ),
          ),
        ],
      ),
      body: FutureBuilder<void>(
        future: _initializeControllerFuture,
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.done) {
            if (_controller != null) {
              return CameraPreview(_controller!);
            } else {
              return const Center(child: Text('Camera initialization failed'));
            }
          } else {
            return const Center(child: CircularProgressIndicator());
          }
        },
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () async {
          try {
            await _initializeControllerFuture;
            final path = await _savePhoto();
            if (!mounted) return;
            ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text('Photo saved at $path')),
            );
            BlocProvider.of<WeatherCubit>(context).savePhotoPath(path);
          } catch (e) {
            if (!mounted) return;
            ScaffoldMessenger.of(context).showSnackBar(
              SnackBar(content: Text('Error: $e')),
            );
          }
        },
        child: const Icon(Icons.camera),
      ),
    );
  }
}