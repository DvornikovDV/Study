import 'dart:typed_data';
import 'camera.dart';
import 'rover.dart';

class Photo {
  int id;
  int sol;
  Camera camera;
  String imgSrc;
  String earthDate;
  Rover rover;
  late Uint8List? imageBytes; // Поле для байтов изображения

  Photo({
    required this.id,
    required this.sol,
    required this.camera,
    required this.imgSrc,
    required this.earthDate,
    required this.rover,
    this.imageBytes,
  });

  factory Photo.fromJson(Map<String, dynamic> json) {
    return Photo(
      id: json['id'],
      sol: json['sol'],
      camera: Camera.fromJson(json['camera']),
      imgSrc: json['img_src'],
      earthDate: json['earth_date'],
      rover: Rover.fromJson(json['rover']),
    );
  }
}