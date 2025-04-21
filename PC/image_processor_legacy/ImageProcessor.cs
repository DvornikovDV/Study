using System.Drawing;
using System.Drawing.Imaging;

namespace ImageFilterApp
{
    public class ImageProcessor
    {
        public Bitmap ApplyConvolution(ManualResetEvent pauseEvent, Bitmap bitmap, float[,] kernel, Action<double> updateProgress = null, CancellationToken cancellationToken = default)
        {
            int width = bitmap.Width;
            int height = bitmap.Height;
            Bitmap result = new Bitmap(width, height);
            int kernelRows = kernel.GetLength(0);
            int kernelCols = kernel.GetLength(1);

            int offsetX = kernelCols / 2;
            int offsetY = kernelRows / 2;

            if (kernelCols % 2 == 0)
                offsetX -= 1;

            if (kernelRows % 2 == 0)
                offsetY -= 1;

            // Блокируем память для исходного и результирующего изображений
            BitmapData srcData = bitmap.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            BitmapData dstData = result.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

            try
            {
                int bytesPerPixel = 4; // ARGB (32 бита на пиксель)
                int stride = srcData.Stride; // Длина строки в байтах
                int totalPixels = width * height;
                int processedPixels = 0;

                unsafe
                {
                    byte* srcPtr = (byte*)srcData.Scan0;
                    byte* dstPtr = (byte*)dstData.Scan0;

                    for (int y = 0; y < height; y++)
                    {
                        cancellationToken.ThrowIfCancellationRequested();

                        while (!pauseEvent.WaitOne(TimeSpan.FromMilliseconds(100)))
                        {
                            cancellationToken.ThrowIfCancellationRequested();
                        }

                        for (int x = 0; x < width; x++)
                        {
                            float red = 0, green = 0, blue = 0;

                            for (int ky = 0; ky < kernelRows; ky++)
                            {
                                for (int kx = 0; kx < kernelCols; kx++)
                                {
                                    int pixelX = x + kx - offsetX;
                                    int pixelY = y + ky - offsetY;

                                    // Replication padding
                                    if (pixelX < 0) pixelX = 0;
                                    if (pixelY < 0) pixelY = 0;
                                    if (pixelX >= width) pixelX = width - 1;
                                    if (pixelY >= height) pixelY = height - 1;

                                    // Вычисляем адрес пикселя в памяти
                                    int pixelOffset = pixelY * stride + pixelX * bytesPerPixel;
                                    byte b = srcPtr[pixelOffset];
                                    byte g = srcPtr[pixelOffset + 1];
                                    byte r = srcPtr[pixelOffset + 2];

                                    red += r * kernel[ky, kx];
                                    green += g * kernel[ky, kx];
                                    blue += b * kernel[ky, kx];
                                }
                            }

                            red = Math.Clamp(red, 0, 255);
                            green = Math.Clamp(green, 0, 255);
                            blue = Math.Clamp(blue, 0, 255);

                            // Записываем результат в результирующий Bitmap
                            int dstOffset = y * stride + x * bytesPerPixel;
                            dstPtr[dstOffset] = (byte)blue;
                            dstPtr[dstOffset + 1] = (byte)green;
                            dstPtr[dstOffset + 2] = (byte)red;
                            dstPtr[dstOffset + 3] = 255; // Alpha

                            processedPixels++;
                            if (updateProgress != null && processedPixels % (totalPixels / 100) == 0)
                            {
                                double progress = (double)processedPixels / totalPixels * 100;
                                updateProgress(progress);
                            }
                        }
                    }
                }
            }
            finally
            {
                // Разблокируем память
                bitmap.UnlockBits(srcData);
                result.UnlockBits(dstData);
            }

            return result;
        }
    }
}