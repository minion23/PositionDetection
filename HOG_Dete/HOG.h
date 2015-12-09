#pragma once
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

using namespace cv;
using namespace std;
class HOGDetector
{
private: 
    //void DetectContour(void);
    vector<Point2d> centers;
    Mat fg;
public:
    HOGDetector(Mat gray);
    vector<Rect> Detect(Mat gray);
    ~HOGDetector(void);
    vector<Point2d> Position(void);
};
