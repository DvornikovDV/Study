using System.Windows;
using System.Windows.Controls;

namespace ImageFilterApp
{
    public partial class MainWindow : Window
    {
        private readonly ImageFilterController _controller;

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

        private void UpdateProgressBar(double progress)
        {
            Progress.Value = progress; // Обновляем значение ProgressBar
        }

        private void LoadImage_Click(object sender, RoutedEventArgs e)
        {
            SaveImageButton.IsEnabled = false;
            _controller.LoadImage();
        }

        private void ApplyFilter_Click(object sender, RoutedEventArgs e)
        {
            string selectedFilter = (FilterComboBox.SelectedItem as ComboBoxItem)?.Content.ToString();
            string executionMode = (ExecutionModeComboBox.SelectedItem as ComboBoxItem)?.Content.ToString();

            _controller.ApplyFilter(selectedFilter, executionMode);

            SaveImageButton.IsEnabled = true;
        }

        private void SaveImage_Click(object sender, RoutedEventArgs e)
        {
            _controller.SaveImage();
        }

        private void Pause_Click(object sender, RoutedEventArgs e)
        {
            _controller.PauseProcessing();
            PauseButton.IsEnabled = false;
            ResumeButton.IsEnabled = true;
        }

        private void Resume_Click(object sender, RoutedEventArgs e)
        {
            _controller.ResumeProcessing();
            PauseButton.IsEnabled = true;
            ResumeButton.IsEnabled = false;
        }

        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            _controller.CancelProcessing();
        }

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