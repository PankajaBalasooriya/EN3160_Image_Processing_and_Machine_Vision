#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "matplotlibcpp.h"  

namespace plt = matplotlibcpp;
using namespace cv;
using namespace std;

int main() {
    // Define control points: (input_intensity, output_intensity)
    vector<Point> c = { {50, 50}, {50, 100}, {150, 255}, {150, 150} };

    // Lookup table (256 values)
    Mat transform(1, 256, CV_8U);
    uchar* lut = transform.ptr();

    // Helper lambda to fill linear ranges
    auto fillRange = [&](int startX, int startY, int endX, int endY) {
        int length = endX - startX;
        for (int i = 0; i <= length; i++) {
            float t = (length == 0) ? 0 : (float)i / (float)length;
            lut[startX + i] = saturate_cast<uchar>(startY + t * (endY - startY));
        }
    };

    // Fill each segment
    fillRange(0, 0, c[0].x, c[0].y);
    fillRange(c[0].x + 1, c[0].y + 1, c[1].x, c[1].y);
    fillRange(c[1].x + 1, c[1].y + 1, c[2].x, c[2].y);
    fillRange(c[2].x + 1, c[2].y + 1, c[3].x, c[3].y);
    fillRange(c[3].x + 1, c[3].y + 1, 255, 255);

    cout << "Length of transform: " << transform.cols << endl;

    // Plot transform curve
    vector<int> x_vals(256), y_vals(256);
    for (int i = 0; i < 256; i++) {
        x_vals[i] = i;
        y_vals[i] = lut[i];
    }

    plt::figure();
    plt::plot(x_vals, y_vals);
    plt::xlabel("Input intensity");
    plt::ylabel("Output intensity");
    plt::xlim(0, 255);
    plt::ylim(0, 255);
    plt::grid(true);
    plt::savefig("../Report/resources/emma_intensity_transform_curve.png");

    // Load grayscale image
    Mat img_emma = imread("../a1images/emma.jpg", IMREAD_GRAYSCALE);
    if (img_emma.empty()) {
        cerr << "Error: Could not load image." << endl;
        return -1;
    }

    // Apply LUT
    Mat img_emma_transformed;
    LUT(img_emma, transform, img_emma_transformed);

    // Save results
    imwrite("../Report/resources/emma_original.jpg", img_emma);
    imwrite("../Report/resources/emma_transformed.jpg", img_emma_transformed);

    // Display results
    imshow("Original Image", img_emma);
    imshow("Intensity Transformed Image", img_emma_transformed);
    waitKey(0);

    return 0;
}

//g++ main.cpp -o output -I/usr/local/include/opencv4 -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs