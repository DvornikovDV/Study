/// Главное окно приложения для взаимодействия с пользователем
using System.Windows;
using System.Windows.Controls;


namespace ImageFilterApp
{
    public partial class MainWindow : Window
    {
        // Контроллер для управления обработкой изображений
        private readonly ImageFilterController _controller;


        /// <summary>
        /// Конструктор, инициализирующий окно и контроллер
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
            _controller = new ImageFilterController(this);
            _controller.OnProgressUpdated += UpdateProgressBar;
            SaveImageButton.IsEnabled = false;
            PauseButton.IsEnabled = false;
            ResumeButton.IsEnabled = false;
            CancelButton.IsEnabled = false;
        }

        /// <summary>
        /// Обновляет значение прогресс-бара в интерфейсе
        /// </summary>
        /// <param name="progress"></param>
        private void UpdateProgressBar(double progress)
        {
            Progress.Value = progress;
        }

        /// <summary>
        /// Загружает изображение по нажатию кнопки
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void LoadImage_Click(object sender, RoutedEventArgs e)
        {
            SaveImageButton.IsEnabled = false;
            _controller.LoadImage();
        }

        /// <summary>
        /// Применяет фильтр к изображению по нажатию кнопки
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ApplyFilter_Click(object sender, RoutedEventArgs e)
        {
            string selectedFilter = (FilterComboBox.SelectedItem as ComboBoxItem)?.Content.ToString();
            string executionMode = (ExecutionModeComboBox.SelectedItem as ComboBoxItem)?.Content.ToString();

            _controller.ApplyFilter(selectedFilter, executionMode);

            SaveImageButton.IsEnabled = true;
        }

        /// <summary>
        /// Сохраняет отфильтрованное изображение по нажатию кнопки
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SaveImage_Click(object sender, RoutedEventArgs e)
        {
            _controller.SaveImage();
        }

        /// <summary>
        /// Приостанавливает обработку изображения по нажатию кнопки
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Pause_Click(object sender, RoutedEventArgs e)
        {
            _controller.PauseProcessing();
            PauseButton.IsEnabled = false;
            ResumeButton.IsEnabled = true;
        }

        /// <summary>
        /// Возобновляет обработку изображения по нажатию кнопки
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Resume_Click(object sender, RoutedEventArgs e)
        {
            _controller.ResumeProcessing();
            PauseButton.IsEnabled = true;
            ResumeButton.IsEnabled = false;
        }

        /// <summary>
        /// Отменяет обработку изображения по нажатию кнопки
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            _controller.CancelProcessing();
        }

        /// <summary>
        /// Отключает элементы управления во время обработки
        /// </summary>
        public void DisableControls()
        {
            LoadImageButton.IsEnabled = false;
            FilterComboBox.IsEnabled = false;
            ExecutionModeComboBox.IsEnabled = false;
            ApplyFilterButton.IsEnabled = false;
            SaveImageButton.IsEnabled = false;
            PauseButton.IsEnabled = true;
            ResumeButton.IsEnabled = false;
            CancelButton.IsEnabled = true;
        }

        /// <summary>
        /// Включает элементы управления после завершения обработки
        /// </summary>
        public void EnableControls()
        {
            LoadImageButton.IsEnabled = true;
            FilterComboBox.IsEnabled = true;
            ExecutionModeComboBox.IsEnabled = true;
            ApplyFilterButton.IsEnabled = true;
            SaveImageButton.IsEnabled = true;
            PauseButton.IsEnabled = false;
            ResumeButton.IsEnabled = false;
            CancelButton.IsEnabled = false;
        }
    }
}