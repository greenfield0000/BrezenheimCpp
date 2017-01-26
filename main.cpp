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

vector<unsigned char> BrezenhemPainLines(Mat &edges, int x0, int y0, int x1, int y1, int n, int curs);
void show_vector(vector<unsigned char>&colorVector);

int main(int, char**) {
    int x0, y0, x1, y1, x, y, curs = 0, n = 5;
    bool isRun = TRUE;
    //кол-во ячеек
    int numBins = 256;
    unsigned char range[] = {0, 255};
    unsigned char *ranges[] = {range};
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

        vector<unsigned char> colorUnderLine = BrezenhemPainLines(frameGistogram, 0, 0, 160, 479, n, curs);
        namedWindow("Gray", 1);
        imshow("Gray", frameGistogram);

        Mat img(300, colorUnderLine.size(), CV_8SC4);
        line(img, Point(0, 299), Point(colorUnderLine.size() - 1, 299), Scalar(255, 0, 0, 255), 3);
        for (int i = 0; i < (colorUnderLine.size() - 1); i++) {
            line(img, Point(i, 255 - colorUnderLine[i]), Point(i + 1, 255 - colorUnderLine[i + 1]), Scalar(255), 1);
        }

        imshow("MYWINDOW", img);

        unsigned char c = cvWaitKey(100);
        switch (c) {
                // escape    
            case 27:
                isRun = !isRun;
                break;
                // left    
            case 81:
                if (curs == 0) {
                    curs = (n - 1);
                }
                curs--;
                break;
        }

    }
    return 0;
}

vector<unsigned char> BrezenhemPainLines(Mat &frameGistogram, int x0, int y0, int x1, int y1, int n, int curs) {
    int x, y, color = 1, i = 0;
    vector<unsigned char> colorVector(256);

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

        if (color == 255) {
            colorVector.push_back((unsigned char) frameGistogram.at<unsigned char>(y0, x1));
        }
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
                if (color == 255) {
                    colorVector.push_back((unsigned char) frameGistogram.at<unsigned char>(y, x));
                }
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
                if (color == 255) {
                    colorVector.push_back((unsigned char) frameGistogram.at<unsigned char>(y, x));
                }
                frameGistogram.at<unsigned char>(y, x) = color;
            } while (x != x1 || y != y1);
        }
        x0 += 160;
        x1 += 160;
        color = 0;
    }

    //    show_vector(colorVector);


    return colorVector;
}

void show_vector(vector<unsigned char>&a) {
    for (vector<unsigned char>::iterator it = a.begin(); it != a.end(); ++it)
        cout << (int) *it << endl;
}