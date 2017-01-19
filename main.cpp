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

void BrezenhemPainLines(Mat &edges, int x0, int y0, int x1, int y1, int n, int curs);

int main(int, char**) {
    int x0, y0, x1, y1, x, y, curs = 0, n = 5;
    bool isRun = TRUE;
    // количество бинов гистограммы 
    int countBins = 256;
    // интервал изменения значений бинов 
    float range[] = {0.0f, 256.0f};
    const float* histRange = {range};
    // равномерное распределение интервала по бинам 
    bool uniform = true;
    // запрет очищения перед вычислением гистограммы 
    bool accumulate = false;


    VideoCapture cap(0); // open the default camera
    if (!cap.isOpened()) // check if we succeeded
        return -1;

    Mat edges;
    namedWindow("edges", CV_WINDOW_AUTOSIZE);
    namedWindow("greyWindow", CV_WINDOW_AUTOSIZE);

    while (isRun) {
        Mat frame, frameGistogram;
        cap >> frame; // get a new frame from camera
        cvtColor(frame, edges, CV_8UC1); //COLOR_BGR2GRAY

        frameGistogram = frame.clone();
        cvtColor(frameGistogram, frameGistogram, COLOR_BGR2GRAY);

        //Нужно трешхолдить с хорошим порогом
        //        cvThreshold(&frameGistogram, &frameGistogram, 50.0, 250.0, CV_THRESH_BINARY);

        // Рисуем линии Брезенхейма и показываем кадр
        BrezenhemPainLines(frameGistogram, 0, 0, 160, 479, n, curs);
        imshow("edges", edges);
        imshow("greyWindow", frameGistogram);

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
    }

    //Очищаем ресурсы
    cvDestroyAllWindows();

    return 0;
}

void BrezenhemPainLines(Mat &frameGistogram, int x0, int y0, int x1, int y1, int n, int curs) {
    int x, y, color = 1;
    // x = 640 y = 480

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
                frameGistogram.at<unsigned char>(y, x) = color;
            } while (x != x1 || y != y1);
        }
        x0 += 160;
        x1 += 160;
        color = 0;
    }
}
