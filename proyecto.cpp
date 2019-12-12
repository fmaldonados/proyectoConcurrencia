#include <iostream>
#include <vector>
#include <assert.h>
#include <cmath>
#include <png++/png.hpp>
#include <omp.h>
#include "timer.h"

using namespace std;

typedef vector<double> Array;
typedef vector<Array> Matrix;
typedef vector<Matrix> Image;

Matrix getGaussian()
{
    Matrix kernel(3, Array(3));
    kernel[0][0] = 0.077847;
    kernel[0][1] = 0.123317;
    kernel[0][2] = 0.077847;
    kernel[1][0] = 0.123317;
    kernel[1][1] = 0.195346;
    kernel[1][2] = 0.123317;
    kernel[2][0] = 0.077847;
    kernel[2][1] = 0.123317;
    kernel[2][2] = 0.077847;
    
    return kernel;
}

Image loadImage(const char *filename)
{
    png::image<png::rgb_pixel> image(filename);
    Image imageMatrix(3, Matrix(image.get_height(), Array(image.get_width())));

    int h, w;
    for (h = 0; h < image.get_height(); h++)
    {
        for (w = 0; w < image.get_width(); w++)
        {
            imageMatrix[0][h][w] = image[h][w].red;
            imageMatrix[1][h][w] = image[h][w].green;
            imageMatrix[2][h][w] = image[h][w].blue;
        }
    }

    return imageMatrix;
}

void saveImage(Image &image, const char *filename)
{
    assert(image.size() == 3);

    int height = image[0].size();
    int width = image[0][0].size();
    int x, y;

    png::image<png::rgb_pixel> imageFile(width, height);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            imageFile[y][x].red = image[0][y][x];
            imageFile[y][x].green = image[1][y][x];
            imageFile[y][x].blue = image[2][y][x];
        }
    }
    imageFile.write(filename);
}

Image applyFilter(Image &image, Matrix &filter)
{
    assert(image.size() == 3 && filter.size() != 0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;
    int d, i, j, h, w;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));

    for (d = 0; d < 3; d++)
    {
        for (i = 0; i < newImageHeight; i++)
        {
            for (j = 0; j < newImageWidth; j++)
            {
                for (h = i; h < i + filterHeight; h++)
                {
                    for (w = j; w < j + filterWidth; w++)
                    {
                        newImage[d][i][j] += filter[h - i][w - j] * image[d][h][w];
                    }
                }
            }
        }
    }

    return newImage;
}

void filterInLine(Image &image, Image &newImage, Matrix &filter, int d ,int i, int newImageWidth, int filterHeight, int filterWidth)
{
   
    for (int j = 0; j < newImageWidth; j++)
    {
        for (int h = i; h < i + filterHeight; h++)
        {
            for (int w = j; w < j + filterWidth; w++)
            {
                newImage[d][i][j] += filter[h - i][w - j] * image[d][h][w];
            }
        }
    }
}

Image parallelApplyFilter(Image &image, Matrix &filter)
{
    assert(image.size() == 3 && filter.size() != 0);

    int height = image[0].size();
    int width = image[0][0].size();
    int filterHeight = filter.size();
    int filterWidth = filter[0].size();
    int newImageHeight = height - filterHeight + 1;
    int newImageWidth = width - filterWidth + 1;

    Image newImage(3, Matrix(newImageHeight, Array(newImageWidth)));
    #pragma omp parallel for num_threads(100) collapse(2) 
    for (int d = 0; d < 3; d++) {
        for (int i = 0; i < newImageHeight; i++) {
            //cout<< d << " and " << i << "\n";
            filterInLine(image, newImage, filter, d, i, newImageWidth, filterHeight, filterWidth);
        }
    }

    return newImage;
}


Image applyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for (int i = 0; i < times; i++)
    {
        newImage = applyFilter(newImage, filter);
    }
    return newImage;
}

Image parallelApplyFilter(Image &image, Matrix &filter, int times)
{
    Image newImage = image;
    for (int i = 0; i < times; i++)
    {
        newImage = parallelApplyFilter(newImage, filter);
    }
    return newImage;
}

int main(int argc, char *argvs[])
{
    Matrix filter = getGaussian();
    double start, finish;
    double start2, finish2;

    cout << "Loading image..." << endl;
    Image image = loadImage("bigImage2.png");
    cout << "Image resolution: " << image[0].size() << " X " << image[0][0].size() << "\n";
    cout << "Applying Serial filter..." << endl;
    GET_TIME(start);
    Image newImage = applyFilter(image, filter, 20);
    GET_TIME(finish);
    cout << "in " << finish - start << " seconds" << endl;
    cout << "Applying Parallel filter..." << endl;
    GET_TIME(start2);
    Image newImage2 = parallelApplyFilter(image, filter, 20);
    GET_TIME(finish2);
    cout << "in " << finish2 - start2 << " seconds" << endl;
    cout << "Saving images..." << endl;
    saveImage(newImage, "newImage.png");
    saveImage(newImage2, "newImage2.png");
    cout << "Done!" << endl;
}