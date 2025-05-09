class Rover {
  int id;
  String name;
  String landingDate;
  String launchDate;
  String status;
  Rover({
    required this.id,
    required this.name,
    required this.landingDate,
    required this.launchDate,
    required this.status,
  });
  factory Rover.fromJson(Map<String, dynamic> json) {
    return Rover(
      id: json['id'],
      name: json['name'],
      landingDate: json['landing_date'],
      launchDate: json['launch_date'],
      status: json['status'],
    );
  }
}