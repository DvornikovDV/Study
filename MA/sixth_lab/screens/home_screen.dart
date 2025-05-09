import 'package:flutter/material.dart';
import 'package:flutter_bloc/flutter_bloc.dart';
import '../cubit/nasa_cubit.dart';

class HomeScreen extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: Text('Curiosity Rover - Sol 1000')),
      body: BlocBuilder<NasaCubit, NasaState>(
        builder: (context, state) {
          if (state is NasaLoadingState) {
            return Center(child: CircularProgressIndicator());
          } else if (state is NasaErrorState) {
            return Center(child: Text('Error: ${state.error}', style: TextStyle(color: Colors.red)));
          } else if (state is NasaLoadedState) {
            return Column(
              children: [
                Expanded(
                  child: ListView.builder(
                    itemCount: state.photos.photos.length,
                    itemBuilder: (context, index) {
                      final photo = state.photos.photos[index];
                      return Card(
                        margin: EdgeInsets.all(8.0),
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Image.network(
                              photo.imgSrc,
                              fit: BoxFit.cover,
                              height: 200,
                              errorBuilder: (context, error, stackTrace) {
                                return Container(
                                  height: 200,
                                  color: Colors.grey[200],
                                  child: Center(
                                    child: Text(
                                      'Failed to load image: $error\nURL: ${photo.imgSrc}',
                                      style: TextStyle(color: Colors.black54),
                                      textAlign: TextAlign.center,
                                    ),
                                  ),
                                );
                              },
                            ),
                            Padding(
                              padding: EdgeInsets.all(8.0),
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  Text('Photo ID: ${photo.id}', style: TextStyle(fontWeight: FontWeight.bold)),
                                  Text('Sol: ${photo.sol}'),
                                  Text('Earth Date: ${photo.earthDate}'),
                                  Text('Camera: ${photo.camera.fullName}'),
                                  Text('Rover: ${photo.rover.name} (Status: ${photo.rover.status})'),
                                ],
                              ),
                            ),
                          ],
                        ),
                      );
                    },
                  ),
                ),
                Padding(
                  padding: EdgeInsets.all(8.0),
                  child: Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      ElevatedButton(
                        onPressed: state.currentPage > 1
                            ? () => context.read<NasaCubit>().loadData(page: state.currentPage - 1)
                            : null,
                        child: Text('Previous'),
                      ),
                      Text('Page ${state.currentPage}'),
                      ElevatedButton(
                        onPressed: state.photos.photos.length == 20
                            ? () => context.read<NasaCubit>().loadData(page: state.currentPage + 1)
                            : null,
                        child: Text('Next'),
                      ),
                    ],
                  ),
                ),
              ],
            );
          }
          return Container();
        },
      ),
    );
  }
}