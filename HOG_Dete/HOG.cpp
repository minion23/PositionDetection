#include "HOG.h"
#include <iostream>
using namespace cv;
using namespace std;


HOGDetector::HOGDetector(Mat gray)
{
    fg = gray.clone();
    vector<Rect> rects;
    vector<Point2d> centers;
}

vector<Point2d> HOGDetector::Position()
{
    //double area=0;
    //centers.clear();
    return centers;
}

vector<Rect> HOGDetector::Detect(Mat gray)
{
    centers.clear();
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    vector<Rect> found, found_filtered;
    //double t = (double)getTickCount();
    // run the detector with default parameters. to get a higher hit-rate
    // (and more false alarms, respectively), decrease the hitThreshold and
    // groupThreshold (set groupThreshold to 0 to turn off the grouping completely).

    hog.detectMultiScale(gray, found, 0.1, Size(8,8), Size(32,32), 1.05,2);
    //t = (double)getTickCount();
    //printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());
    size_t i,j;
    for( i = 0; i < found.size(); i++ )
    {
        Rect r = found[i];
        for( j = 0; j < found.size(); j++ )
            if( j != i && (r & found[j]) == r)
                break;
        if( j == found.size() )
            found_filtered.push_back(r);
    }
    for( i = 0; i < found_filtered.size(); i++ )
    {
        Rect r = found_filtered[i];
        // the HOG detector returns slightly larger rectangles than the real objects.
        // so we slightly shrink the rectangles to get a nicer output.
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.07);
        r.height = cvRound(r.height*0.8);
        Point2d center;
        center.x = r.x+r.width/2;
        center.y = r.y+r.height;
        centers.push_back(center);
    }
    return found_filtered;
}

HOGDetector::~HOGDetector(void){
}
