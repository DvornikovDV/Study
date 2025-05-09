import 'package:flutter_bloc/flutter_bloc.dart';
import '../services/api_service.dart';
import '../models/photos.dart';

abstract class NasaState {}

class NasaLoadingState extends NasaState {}

class NasaLoadedState extends NasaState {
  final Photos photos;
  final int currentPage;
  NasaLoadedState(this.photos, this.currentPage);
}

class NasaErrorState extends NasaState {
  final String error;
  NasaErrorState(this.error);
}

class NasaCubit extends Cubit<NasaState> {
  final ApiService apiService;
  NasaCubit(this.apiService) : super(NasaLoadingState());

  void loadData({int page = 1}) async {
    try {
      emit(NasaLoadingState());
      final photos = await apiService.fetchPhotos(page);
      emit(NasaLoadedState(photos, page));
    } catch (e) {
      emit(NasaErrorState(e.toString()));
    }
  }
}