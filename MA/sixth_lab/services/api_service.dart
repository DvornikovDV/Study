import 'dart:convert';
import 'package:http/http.dart' as http;
import '../models/photos.dart';
import 'package:flutter/foundation.dart'; // Для debugPrint

class ApiService {
  static const String _baseUrl = 'https://api.nasa.gov/mars-photos/api/v1/rovers';
  static const String _apiKey = 'eQnprvXukgfNomTanZiHT1DqLApcABzFjI350dyZ';

  Future<Photos> fetchPhotos(int page) async {
    // От opportunity картинок получить не удалось, все ссылки ведут на страницу с инфой о машине, но не изображения(проверялось на разные sol)
    final uri = Uri.parse('$_baseUrl/curiosity/photos?sol=1000&page=$page&per_page=20&api_key=$_apiKey');
    final response = await http.get(uri);
    if (response.statusCode == 200) {
      final photos = Photos.fromJson(jsonDecode(response.body));
      // Логируем URL для диагностики
      for (var photo in photos.photos) {
        debugPrint('Image URL for ${photo.id}: ${photo.imgSrc}');
      }
      return photos;
    } else {
      throw Exception('Failed to load photos: Status ${response.statusCode}');
    }
  }
}