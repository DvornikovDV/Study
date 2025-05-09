import 'package:sqflite/sqflite.dart';
import 'package:path/path.dart';

class DatabaseHelper {
  static final DatabaseHelper instance = DatabaseHelper._init();
  static Database? _database;

  DatabaseHelper._init();

  Future<Database> get database async {
    if (_database != null) return _database!;
    _database = await _initDB('calculations.db');
    return _database!;
  }

  Future<Database> _initDB(String fileName) async {
    final dbPath = await getDatabasesPath();
    final path = join(dbPath, fileName);

    print('Opening database at $path');
    return await openDatabase(
      path,
      version: 1,
      onCreate: (db, version) async {
        await db.execute('''
          CREATE TABLE calculations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            mass REAL NOT NULL,
            radius REAL NOT NULL,
            velocity REAL NOT NULL
          )
        ''');
        print('Database created');
      },
    );
  }

  Future<void> insertCalculation(double mass, double radius, double velocity) async {
    final db = await database;
    await db.insert(
      'calculations',
      {
        'mass': mass,
        'radius': radius,
        'velocity': velocity,
      },
      conflictAlgorithm: ConflictAlgorithm.replace,
    );
    print('Calculation inserted: mass=$mass, radius=$radius, velocity=$velocity');
  }

  Future<List<Map<String, dynamic>>> getCalculations() async {
    final db = await database;
    final result = await db.query('calculations');
    print('Loaded ${result.length} calculations');
    return result;
  }

  Future<void> clearDatabase() async {
    final db = await database;
    await db.delete('calculations');
    print('Database cleared');
  }

  Future<bool> checkDatabaseExists() async {
    final dbPath = await getDatabasesPath();
    final path = join(dbPath, 'calculations.db');
    return await databaseExists(path);
  }
}