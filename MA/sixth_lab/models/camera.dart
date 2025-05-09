class Camera {
  int id;
  String name;
  int roverId;
  String fullName;
  Camera({
    required this.id,
    required this.name,
    required this.roverId,
    required this.fullName,
  });
  factory Camera.fromJson(Map<String, dynamic> json) {
    return Camera(
      id: json['id'],
      name: json['name'],
      roverId: json['rover_id'],
      fullName: json['full_name'],
    );
  }
}