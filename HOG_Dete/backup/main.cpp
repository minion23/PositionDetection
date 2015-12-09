#include "HOG.h"
#include <iostream>
using namespace cv;
using namespace std;

int main(){
    Mat img = imread("test.jpg");
    Size size(1080,720);
    resize(img,img,size);
    namedWindow("people detector",1);
    HOGDetector* detector = new HOGDetector(img);
    vector<Rect> found = detector->Detect(img);
    vector<Point2d> centers = detector->Position();
    size_t i;
    for (i=0;i<found.size();i++)
    {
        Rect r = found[i];
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.07);
        r.height = cvRound(r.height*0.8);
        rectangle(img, r.tl(),r.br(),cv::Scalar(0,255,0),3);
    }
    std::cout << "centers is" << centers <<endl;
    imshow("people detector", img);
    waitKey(0);
    return 0;
}
