#include <iostream>
#include <ctime>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2/core/internal.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/legacy/compat.hpp>

#define CV_CAP_ANY 0 // autodetect

using namespace cv;
using namespace std;

Mat BrezenhemPainLines(Mat &edges, int x0, int y0, int x1, int y1, int n, int curs);
void show_vector(vector<unsigned char>&colorVector);

int main(int, char**) {
    int x0, y0, x1, y1, x, y, curs = 0, n = 5;
    bool isRun = TRUE;
    //кол-во ячеек
    int numBins = 256;
    float range[] = {0, 255};
    float *ranges[] = {range};
    bool uniform = true;
    // запрет очищения перед вычислением гистограммы 
    bool accumulate = false;
    // штука, которая будет строить гистограмму 
    CvHistogram* histogram;

    VideoCapture cap(0); // open the default camera
    if (!cap.isOpened()) // check if we succeeded
        return -1;

    Mat edges;

    while (isRun) {
        Mat frame, frameGistogram;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, CV_8UC1); //COLOR_BGR2GRAY

        frameGistogram = frame.clone();
        cvtColor(frameGistogram, frameGistogram, COLOR_BGR2GRAY);

        Mat colorUnderLine = BrezenhemPainLines(frameGistogram, 0, 0, 160, 479, n, curs);
        namedWindow("Gray", 1);
        imshow("Gray", frameGistogram);

        // Initialize parameters
        int histSize = 256; // bin size
        float range[] = {0, 255};
        const float *ranges[] = {range};

        // Calculate histogram
        MatND hist;
        calcHist(&frameGistogram, 1, 0, Mat(), hist, 1, &histSize, ranges, true, false);

        //        // Show the calculated histogram in command window
        //        double total;
        //        total = frameGistogram.rows * frameGistogram.cols;
        //        for (int h = 0; h < histSize; h++) {
        //            float binVal = hist.at<float>(h);
        //            cout << " " << binVal;
        //        }

        // Plot the histogram
        int hist_w = 512;
        int hist_h = 400;
        int bin_w = cvRound((double) hist_w / histSize);

        Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0, 0, 0));
        normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

        for (int i = 1; i < histSize; i++) {
            line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                    Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
                    Scalar(255, 0, 0), 2, 8, 0);
        }

        char c = cvWaitKey(33);
        switch (c) {
                // escape    
            case 27:
                isRun = !isRun;
                break;
                // left    
            case 81:
                if (curs == 0) {
                    curs = n;
                }
                curs--;
                break;
                // right
            case 83:
                if (curs == (n - 1)) {
                    curs = -1; // жесткий костыль :) 
                }
                curs++;
                break;
        }

        namedWindow("Result", 1);
        imshow("Result", histImage);

    }
    return 0;
}

Mat BrezenhemPainLines(Mat &frameGistogram, int x0, int y0, int x1, int y1, int n, int curs) {
    int x, y, color = 1, i = 0;
    vector<unsigned char> colorVector(sizeof frameGistogram);

    for (int i = 0; i < n - 1; i++) {
        if (curs == i) color = 255;
        int A, B, sign;
        A = y1 - y0;
        B = x0 - x1;
        if (abs(A) > abs(B)) sign = 1;
        else sign = -1;

        int signa, signb;

        if (A < 0) signa = -1;
        else signa = 1;

        if (B < 0) signb = -1;
        else signb = 1;

        int f = 0;

        colorVector.push_back(frameGistogram.at<unsigned char>(y0, x0));
        frameGistogram.at<unsigned char>(y0, x0) = color;
        x = x0, y = y0;
        if (sign == -1) {
            do {
                f += A*signa;
                if (f > 0) {
                    f -= B*signb;
                    y += signa;
                }
                x -= signb;
                colorVector.push_back(frameGistogram.at<unsigned char>(y, x));
                frameGistogram.at<unsigned char>(y, x) = color;
            } while (x != x1 || y != y1);
        } else {
            do {
                f += B*signb;
                if (f > 0) {
                    f -= A*signa;
                    x -= signb;
                }
                y += signa;
                colorVector.push_back(frameGistogram.at<unsigned char>(y, x));
                frameGistogram.at<unsigned char>(y, x) = color;
            } while (x != x1 || y != y1);
        }
        x0 += 160;
        x1 += 160;
        color = 0;
    }

    //    show_vector(colorVector);

    Mat A = Mat(1, colorVector.size(), CV_32FC1, &colorVector);

    return A;
}

void show_vector(vector<unsigned char>&a) {
    for (vector<unsigned char>::iterator it = a.begin(); it != a.end(); ++it)
        cout << (int) *it << endl;
}