import 'photo.dart';

class Photos {
  List<Photo> photos;
  Photos({required this.photos});
  factory Photos.fromJson(Map<String, dynamic> json) {
    return Photos(
      photos: List<Photo>.from(json['photos'].map((x) => Photo.fromJson(x))),
    );
  }
}