using System.Windows;
using System.Windows.Media.Imaging;
using System.IO;
using System.Drawing;
using System.Threading;
using System;

namespace ImageFilterApp
{
    public class ImageFilterController
    {
        private readonly MainWindow _view;
        private Bitmap _originalBitmap;
        private ManualResetEvent _pauseEvent = new ManualResetEvent(true);
        private CancellationTokenSource _cancellationTokenSource;
        private bool _isProcessing;
        private readonly ImageProcessor _imageProcessor = new ImageProcessor();

        public delegate void ProgressUpdateHandler(double progress);
        public event ProgressUpdateHandler OnProgressUpdated;

        private void UpdateProgress(double progress)
        {
            OnProgressUpdated?.Invoke(progress);
        }

        public Dictionary<string, float[,]> Filters { get; } = new Dictionary<string, float[,]>
        {
            { "Blur", new float[,] {
                { 1 / 16f, 2 / 16f, 1 / 16f },
                { 2 / 16f, 4 / 16f, 2 / 16f },
                { 1 / 16f, 2 / 16f, 1 / 16f }
            }},
            { "Edge Detection", new float[,] {
                { -1, -2, -1 },
                {  0,  0,  0 },
                {  1,  2,  1 }
            }},
            { "Sharpen", new float[,] {
                {  0, -1,  0 },
                { -1,  5, -1 },
                {  0, -1,  0 }
            }},
            { "Emboss", new float[,] {
                { -2, -1,  0 },
                { -1,  1,  1 },
                {  0,  1,  2 }
            }},
            { "Even Blur", new float[,] {
                { 1 / 36f, 2 / 36f, 2 / 36f, 1 / 36f },
                { 2 / 36f, 4 / 36f, 4 / 36f, 2 / 36f },
                { 2 / 36f, 4 / 36f, 4 / 36f, 2 / 36f },
                { 1 / 36f, 2 / 36f, 2 / 36f, 1 / 36f }
            }}
        };

        public Bitmap OriginalBitmap { get => _originalBitmap; set => _originalBitmap = value; }

        public ImageFilterController(MainWindow view)
        {
            _view = view;
        }

        public void LoadImage()
        {
            var openFileDialog = new Microsoft.Win32.OpenFileDialog
            {
                Filter = "Image Files (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp"
            };

            if (openFileDialog.ShowDialog() == true)
            {
                string filePath = openFileDialog.FileName;
                OriginalBitmap = new Bitmap(filePath);
                _view.OriginalImage.Source = ConvertToBitmapImage(OriginalBitmap);
                _view.FilteredImage.Source = null;
                _view.Progress.Value = 0;
            }
        }

        public async void ApplyFilter(string filterName, string executionMode)
        {
            if (_originalBitmap == null)
            {
                MessageBox.Show("Пожалуйста, выберите изображение.");
                return;
            }

            _view.DisableControls();
            _view.Progress.Value = 0;

            if (!Filters.ContainsKey(filterName))
            {
                MessageBox.Show("Выбранный фильтр не найден.");
                _view.EnableControls();
                return;
            }

            float[,] kernel = Filters[filterName];
            Bitmap filteredBitmap = null;
            _isProcessing = true;

            try
            {
                _cancellationTokenSource = new CancellationTokenSource();

                if (executionMode == "Основной поток")
                {
                    filteredBitmap = _imageProcessor.ApplyConvolution(_pauseEvent, _originalBitmap, kernel,
                        progress => UpdateProgress(progress),
                        _cancellationTokenSource.Token);
                }
                else if (executionMode == "Дополнительный поток")
                {
                    filteredBitmap = await ApplyConvolutionInThreadAsync(_pauseEvent, _originalBitmap, kernel, _cancellationTokenSource.Token);
                }

                if (filteredBitmap != null)
                {
                    _view.FilteredImage.Source = ConvertToBitmapImage(filteredBitmap);
                }
            }
            catch (OperationCanceledException)
            {
                MessageBox.Show("Выполнение фильтрации было отменено.");
            }
            finally
            {
                _isProcessing = false;
                _view.EnableControls();
                _pauseEvent.Set();
            }
        }

        private Task<Bitmap> ApplyConvolutionInThreadAsync(ManualResetEvent pauseEvent, Bitmap bitmap, float[,] kernel, CancellationToken cancellationToken)
        {
            return Task.Run(() =>
            {
                var processor = new ImageProcessor();
                return processor.ApplyConvolution(pauseEvent, bitmap, kernel,
                    progress =>
                    {
                        Application.Current.Dispatcher.Invoke(() =>
                        {
                            _view.Progress.Value = progress;
                        });
                    },
                    cancellationToken);
            }, cancellationToken);
        }

        public void PauseProcessing()
        {
            if (_isProcessing)
            {
                _pauseEvent.Reset();
            }
        }

        public void ResumeProcessing()
        {
            if (_isProcessing)
            {
                _pauseEvent.Set();
            }
        }

        public void CancelProcessing()
        {
            if (_isProcessing && _cancellationTokenSource != null)
            {
                _view.Progress.Value = 0;
                _cancellationTokenSource.Cancel();
            }
        }

        public void SaveImage()
        {
            if (_view.FilteredImage.Source == null)
            {
                MessageBox.Show("Нет отфильтрованного изображения для сохранения.");
                return;
            }

            var saveFileDialog = new Microsoft.Win32.SaveFileDialog
            {
                Filter = "PNG Image (*.png)|*.png|JPEG Image (*.jpg)|*.jpg|Bitmap Image (*.bmp)|*.bmp"
            };

            if (saveFileDialog.ShowDialog() == true)
            {
                string filePath = saveFileDialog.FileName;
                BitmapEncoder encoder = Path.GetExtension(filePath).ToLower() switch
                {
                    ".png" => new PngBitmapEncoder(),
                    ".jpg" => new JpegBitmapEncoder(),
                    ".bmp" => new BmpBitmapEncoder(),
                    _ => throw new NotSupportedException("Формат файла не поддерживается.")
                };

                encoder.Frames.Add(BitmapFrame.Create((BitmapSource)_view.FilteredImage.Source));
                using var fileStream = new System.IO.FileStream(filePath, System.IO.FileMode.Create);
                encoder.Save(fileStream);
            }
        }

        private BitmapImage ConvertToBitmapImage(Bitmap bitmap)
        {
            using var memoryStream = new System.IO.MemoryStream();
            bitmap.Save(memoryStream, System.Drawing.Imaging.ImageFormat.Png);
            memoryStream.Position = 0;

            var bitmapImage = new BitmapImage();
            bitmapImage.BeginInit();
            bitmapImage.StreamSource = memoryStream;
            bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
            bitmapImage.EndInit();
            bitmapImage.Freeze();
            return bitmapImage;
        }
    }
}