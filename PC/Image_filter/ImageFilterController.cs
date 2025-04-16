// Контроллер для управления обработкой изображений и взаимодействия с пользовательским интерфейсом
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
        // Ссылка на главное окно приложения
        private readonly MainWindow _view;
        // Исходное изображение для обработки
        private Bitmap _originalBitmap;
        // Событие для управления паузой обработки
        private ManualResetEvent _pauseEvent = new ManualResetEvent(true);
        // Токен для отмены асинхронных операций
        private CancellationTokenSource _cancellationTokenSource;
        // Флаг, указывающий, выполняется ли обработка
        private bool _isProcessing;
        // Объект для выполнения операций с изображениями
        private readonly ImageProcessor _imageProcessor = new ImageProcessor();

        // Делегат для уведомления об обновлении прогресса
        public delegate void ProgressUpdateHandler(double progress);
        // Событие для обновления прогресса обработки
        public event ProgressUpdateHandler OnProgressUpdated;

        /// <summary>
        /// Обновляет значение прогресс-бара в пользовательском интерфейсе
        /// </summary>
        /// <param name="progress"></param>
        private void UpdateProgress(double progress)
        {
            try
            {
                OnProgressUpdated?.Invoke(progress);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Ошибка при обновлении прогресса: {ex.Message}");
            }
        }

        // Словарь фильтров, содержащий ядра свертки для различных эффектов
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
            }},
            { "Strong Blur", new float[,] {
                { 1 / 256f, 4 / 256f, 6 / 256f, 4 / 256f, 1 / 256f, 4 / 256f, 6 / 256f },
                { 4 / 256f, 16 / 256f, 24 / 256f, 16 / 256f, 4 / 256f, 16 / 256f, 24 / 256f },
                { 6 / 256f, 24 / 256f, 36 / 256f, 24 / 256f, 6 / 256f, 24 / 256f, 36 / 256f },
                { 4 / 256f, 16 / 256f, 24 / 256f, 16 / 256f, 4 / 256f, 16 / 256f, 36 / 256f },
                { 1 / 256f, 4 / 256f, 6 / 256f, 4 / 256f, 1 / 256f, 4 / 256f, 6 / 256f },
                { 4 / 256f, 16 / 256f, 24 / 256f, 16 / 256f, 4 / 256f, 16 / 256f, 24 / 256f },
                { 6 / 256f, 24 / 256f, 36 / 256f, 24 / 256f, 6 / 256f, 24 / 256f, 36 / 256f }
            }}
        };

        // Свойство для доступа к исходному изображению
        public Bitmap OriginalBitmap { get => _originalBitmap; set => _originalBitmap = value; }

        /// <summary>
        /// Конструктор, инициализирующий контроллер с ссылкой на главное окно
        /// </summary>
        /// <param name="view"></param>
        public ImageFilterController(MainWindow view)
        {
            _view = view;
        }

        /// <summary>
        /// Загружает изображение из файла и отображает его в интерфейсе
        /// </summary>
        public void LoadImage()
        {
            try
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
            catch (Exception ex)
            {
                MessageBox.Show($"Ошибка при загрузке изображения: {ex.Message}");
            }
        }

        /// <summary>
        /// Применяет выбранный фильтр к изображению в указанном режиме выполнения
        /// </summary>
        /// <param name="filterName"></param>
        /// <param name="executionMode"></param>
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
                    filteredBitmap = _imageProcessor.ApplyConvolutionMainThread(_originalBitmap, kernel,
                        progress => UpdateProgress(progress));
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
            catch (AggregateException ae)
            {
                // Проверяем, являются ли все внутренние исключения OperationCanceledException
                bool allCanceled = ae.InnerExceptions.All(ex => ex is OperationCanceledException);
                if (allCanceled)
                {
                    MessageBox.Show("Выполнение фильтрации было отменено.");
                }
                else
                {
                    // Если есть другие исключения, показываем общее сообщение
                    MessageBox.Show($"Общая ошибка: {ae.Message}");
                }
            }
            catch (OperationCanceledException)
            {
                MessageBox.Show("Выполнение фильтрации было отменено.");
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Общая ошибка: {ex.Message}");
            }
            finally
            {
                _isProcessing = false;
                _view.EnableControls();
                _pauseEvent.Set();
            }
        }

        /// <summary>
        /// Асинхронно применяет свертку в отдельном потоке
        /// </summary>
        /// <param name="pauseEvent"></param>
        /// <param name="bitmap"></param>
        /// <param name="kernel"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        private Task<Bitmap> ApplyConvolutionInThreadAsync(ManualResetEvent pauseEvent, Bitmap bitmap, float[,] kernel, CancellationToken cancellationToken)
        {
            return Task.Run(() =>
            {
                var processor = new ImageProcessor();
                return processor.ApplyConvolution(pauseEvent, bitmap, kernel,
                    progress => Application.Current.Dispatcher.Invoke(() => UpdateProgress(progress)),
                    cancellationToken);
            }, cancellationToken);
        }

        /// <summary>
        /// Приостанавливает обработку изображения
        /// </summary>
        public void PauseProcessing()
        {
            if (_isProcessing)
            {
                _pauseEvent.Reset();
            }
        }

        /// <summary>
        /// Возобновляет обработку изображения
        /// </summary>
        public void ResumeProcessing()
        {
            if (_isProcessing)
            {
                _pauseEvent.Set();
            }
        }

        /// <summary>
        /// Отменяет обработку изображения
        /// </summary>
        public void CancelProcessing()
        {
            if (_isProcessing && _cancellationTokenSource != null)
            {
                _view.Progress.Value = 0;
                _cancellationTokenSource.Cancel();
            }
        }

        /// <summary>
        /// Сохраняет отфильтрованное изображение в файл
        /// </summary>
        public void SaveImage()
        {
            try
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
            catch (Exception ex)
            {
                MessageBox.Show($"Ошибка при сохранении: {ex.Message}");
            }
        }

        /// <summary>
        /// Конвертирует Bitmap в BitmapImage для отображения в WPF
        /// </summary>
        /// <param name="bitmap"></param>
        /// <returns></returns>
        private BitmapImage ConvertToBitmapImage(Bitmap bitmap)
        {
            try
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
            catch (Exception ex)
            {
                MessageBox.Show($"Ошибка при конвертации изображения: {ex.Message}");
                throw;
            }
        }
    }
}