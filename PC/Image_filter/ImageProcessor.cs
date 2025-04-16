// Класс для обработки изображений с применением свертки
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;
using System.Threading.Tasks;


namespace ImageFilterApp
{
    public class ImageProcessor
    {
        /// <summary>
        /// Применяет свертку к изображению в многопоточном режиме
        /// </summary>
        /// <param name="pauseEvent"></param>
        /// <param name="bitmap"></param>
        /// <param name="kernel"></param>
        /// <param name="updateProgress"></param>
        /// <param name="cancellationToken"></param>
        /// <returns></returns>
        public Bitmap ApplyConvolution(ManualResetEvent pauseEvent, Bitmap bitmap, float[,] kernel, Action<double> updateProgress = null, CancellationToken cancellationToken = default)
        {
            Bitmap inputBitmap = Ensure32bppArgb(bitmap);
            try
            {
                int width = inputBitmap.Width;
                int height = inputBitmap.Height;
                Bitmap result = new Bitmap(width, height);
                int kernelRows = kernel.GetLength(0);
                int kernelCols = kernel.GetLength(1);

                // Вычисляет смещение ядра для центрирования
                int offsetX = kernelCols / 2;
                int offsetY = kernelRows / 2;

                if (kernelCols % 2 == 0)
                    offsetX -= 1;

                if (kernelRows % 2 == 0)
                    offsetY -= 1;

                // LockBits: 
                // Предоставляет прямой доступ к памяти пикселей изображения, позволяя читать и записывать
                // данные пикселей без использования высокоуровневых методов GetPixel/SetPixel, что значительно
                // увеличило производительность.
                BitmapData srcData = inputBitmap.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                BitmapData dstData = result.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

                try
                {
                    int bytesPerPixel = 4;
                    int stride = srcData.Stride;
                    int processedRows = 0;

                    unsafe
                    {
                        byte* srcPtr = (byte*)srcData.Scan0;
                        byte* dstPtr = (byte*)dstData.Scan0;

                        // Parallel.For: 
                        // Выполняет параллельную обработку строк изображения в пуле потоков, распределяя задачи между доступными
                        // процессорными ядрами. Делит все строки изображения на блоки. Каждый блок строк обрабатывается независимо.
                        ParallelOptions options = new ParallelOptions
                        {
                            MaxDegreeOfParallelism = Environment.ProcessorCount
                        };

                        Parallel.For(0, height, options, y =>
                        {
                            try
                            {
                                cancellationToken.ThrowIfCancellationRequested();

                                while (!pauseEvent.WaitOne(TimeSpan.FromMilliseconds(100)))
                                {
                                    cancellationToken.ThrowIfCancellationRequested();
                                }

                                for (int x = 0; x < width; x++)
                                {
                                    float red = 0, green = 0, blue = 0;

                                    // Перебирает элементы ядра
                                    for (int ky = 0; ky < kernelRows; ky++)
                                    {
                                        for (int kx = 0; kx < kernelCols; kx++)
                                        {
                                            // Вычисляет координаты соседнего пикселя
                                            int pixelX = x + kx - offsetX;
                                            int pixelY = y + ky - offsetY;

                                            // Ограничивает координаты краями изображения
                                            if (pixelX < 0) pixelX = 0;
                                            if (pixelY < 0) pixelY = 0;
                                            if (pixelX >= width) pixelX = width - 1;
                                            if (pixelY >= height) pixelY = height - 1;

                                            // Получает RGB значения пикселя
                                            int pixelOffset = pixelY * stride + pixelX * bytesPerPixel;
                                            byte b = srcPtr[pixelOffset];
                                            byte g = srcPtr[pixelOffset + 1];
                                            byte r = srcPtr[pixelOffset + 2];

                                            // Применяет ядро к RGB
                                            red += r * kernel[ky, kx];
                                            green += g * kernel[ky, kx];
                                            blue += b * kernel[ky, kx];
                                        }
                                    }

                                    // Ограничивает значения RGB диапазоном [0, 255]
                                    red = Math.Clamp(red, 0, 255);
                                    green = Math.Clamp(green, 0, 255);
                                    blue = Math.Clamp(blue, 0, 255);

                                    // Записывает результат в выходное изображение
                                    int dstOffset = y * stride + x * bytesPerPixel;
                                    dstPtr[dstOffset] = (byte)blue;
                                    dstPtr[dstOffset + 1] = (byte)green;
                                    dstPtr[dstOffset + 2] = (byte)red;
                                    dstPtr[dstOffset + 3] = 255;
                                }

                                // Обновляет прогресс обработки
                                int newProcessedRows = Interlocked.Increment(ref processedRows);
                                if (updateProgress != null && newProcessedRows % 5 == 0)
                                {
                                    double progress = (double)newProcessedRows / height * 100;
                                    updateProgress(progress);
                                }
                            }
                            catch
                            {
                                throw;
                            }
                        });

                        if (updateProgress != null)
                        {
                            updateProgress(100);
                        }
                    }
                }
                finally
                {
                    inputBitmap.UnlockBits(srcData);
                    result.UnlockBits(dstData);
                }

                return result;
            }
            finally
            {
                if (inputBitmap != bitmap)
                    inputBitmap.Dispose();
            }
        }

        /// <summary>
        /// Применяет свертку к изображению в однопоточном режиме
        /// </summary>
        /// <param name="bitmap"></param>
        /// <param name="kernel"></param>
        /// <param name="updateProgress"></param>
        /// <returns></returns>
        public Bitmap ApplyConvolutionMainThread(Bitmap bitmap, float[,] kernel, Action<double> updateProgress = null)
        {
            Bitmap inputBitmap = Ensure32bppArgb(bitmap);
            try
            {
                int width = inputBitmap.Width;
                int height = inputBitmap.Height;
                Bitmap result = new Bitmap(width, height);
                int kernelRows = kernel.GetLength(0);
                int kernelCols = kernel.GetLength(1);

                // Вычисляет смещение ядра для центрирования
                int offsetX = kernelCols / 2;
                int offsetY = kernelRows / 2;

                if (kernelCols % 2 == 0)
                    offsetX -= 1;

                if (kernelRows % 2 == 0)
                    offsetY -= 1;

                // LockBits: 
                // Используется для прямого доступа к памяти пикселей, аналогично многопоточному методу.
                BitmapData srcData = inputBitmap.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                BitmapData dstData = result.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

                try
                {
                    int bytesPerPixel = 4;
                    int stride = srcData.Stride;

                    unsafe
                    {
                        byte* srcPtr = (byte*)srcData.Scan0;
                        byte* dstPtr = (byte*)dstData.Scan0;

                        for (int y = 0; y < height; y++)
                        {
                            for (int x = 0; x < width; x++)
                            {
                                float red = 0, green = 0, blue = 0;

                                // Перебирает элементы ядра
                                for (int ky = 0; ky < kernelRows; ky++)
                                {
                                    for (int kx = 0; kx < kernelCols; kx++)
                                    {
                                        // Вычисляет координаты соседнего пикселя
                                        int pixelX = x + kx - offsetX;
                                        int pixelY = y + ky - offsetY;

                                        // Ограничивает координаты краями изображения
                                        if (pixelX < 0) pixelX = 0;
                                        if (pixelY < 0) pixelY = 0;
                                        if (pixelX >= width) pixelX = width - 1;
                                        if (pixelY >= height) pixelY = height - 1;

                                        // Получает RGB значения пикселя
                                        int pixelOffset = pixelY * stride + pixelX * bytesPerPixel;
                                        byte b = srcPtr[pixelOffset];
                                        byte g = srcPtr[pixelOffset + 1];
                                        byte r = srcPtr[pixelOffset + 2];

                                        // Применяет ядро к RGB, в соотсетствии позиции пикселя относительно ядра
                                        red += r * kernel[ky, kx];
                                        green += g * kernel[ky, kx];
                                        blue += b * kernel[ky, kx];
                                    }
                                }

                                // Ограничивает значения RGB диапазоном [0, 255]
                                red = Math.Clamp(red, 0, 255);
                                green = Math.Clamp(green, 0, 255);
                                blue = Math.Clamp(blue, 0, 255);

                                // Записывает результат в выходное изображение
                                int dstOffset = y * stride + x * bytesPerPixel;
                                dstPtr[dstOffset] = (byte)blue;
                                dstPtr[dstOffset + 1] = (byte)green;
                                dstPtr[dstOffset + 2] = (byte)red;
                                dstPtr[dstOffset + 3] = 255;
                            }

                            // Обновляет прогресс обработки
                            if (updateProgress != null && (y + 1) % 5 == 0)
                            {
                                double progress = (double)(y + 1) / height * 100;
                                updateProgress(progress);
                            }
                        }
                    }
                }
                finally
                {
                    inputBitmap.UnlockBits(srcData);
                    result.UnlockBits(dstData);
                }

                return result;
            }
            finally
            {
                if (inputBitmap != bitmap)
                    inputBitmap.Dispose();
            }
        }

        /// <summary>
        /// Конвертирует изображение в формат 32bpp ARGB, если необходимо
        /// </summary>
        /// <param name="bitmap"></param>
        /// <returns></returns>
        private Bitmap Ensure32bppArgb(Bitmap bitmap)
        {
            try
            {
                if (bitmap.PixelFormat != PixelFormat.Format32bppArgb)
                {
                    Bitmap newBitmap = new Bitmap(bitmap.Width, bitmap.Height, PixelFormat.Format32bppArgb);
                    using (Graphics g = Graphics.FromImage(newBitmap))
                    {
                        g.DrawImage(bitmap, 0, 0);
                    }
                    return newBitmap;
                }
                return bitmap;
            }
            catch
            {
                throw;
            }
        }
    }
}