#include "HOG.h"
#include "opencv2/opencv.hpp"

#include <opencv2/highgui/highgui_c.h>
#include "CTracker.h"
#include <iostream>
#include <vector>
#include <iostream>
using namespace cv;
using namespace std;

int main(){
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};
    VideoCapture capture("atrium.avi");
    if(!capture.isOpened())
    {
    return 0;
    }
    CTracker tracker(0.2,0.5,60.0,15,10);
    namedWindow("Video");
    Mat img;
    capture >> img;
    HOGDetector* detector = new HOGDetector(img);
    int k=0;
    while(k!=27){
        capture >> img;
        //Mat img = imread("test.jpg");
        Size size(1080,720);
        resize(img,img,size);
        namedWindow("people detector",1);
        //double t = (double)getTickCount();
        //HOGDetector* detector = new HOGDetector(img);
        vector<Rect> found = detector->Detect(img);
        vector<Point2d> centers = detector->Position();
        //t = (double) getTickCount();
        //printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());
        //size_t i;
        //for (i=0;i<found.size();i++)
        //{
            //Rect r = found[i];
            //r.x += cvRound(r.width*0.1);
            //r.width = cvRound(r.width*0.8);
            //r.y += cvRound(r.height*0.07);
            //r.height = cvRound(r.height*0.8);
            //rectangle(img, r.tl(),r.br(),cv::Scalar(0,255,0),3);
        //}
        //for(i=0; i<centers.size();i++){
            //Point2d c = centers[i];
            //circle(img, c,3, CV_RGB(255,0,0),-1, 8, 0);
        //}
        //std::cout << "centers is" << centers <<endl;
        //imshow("people detector", img);
        //k=waitKey(1);
        size_t i;
        for( i=0; i<centers.size(); i++)
        {
        circle(img,centers[i],3,Scalar(0,255,0),1,CV_AA);
        }


        if(centers.size()>0)
        {
            tracker.Update(centers);
            
            cout << tracker.tracks.size()  << endl;

            for(size_t i=0;i<tracker.tracks.size();i++)
            {
                if(tracker.tracks[i]->trace.size()>1)
                {
                    for(size_t j=0;j<tracker.tracks[i]->trace.size()-1;j++)
                    {
                        line(img,tracker.tracks[i]->trace[j],tracker.tracks[i]->trace[j+1],Colors[tracker.tracks[i]->track_id%9],2,CV_AA);
                    }
                }
            }
        }

        imshow("Video",img);

        k=waitKey(20);
        }
        delete detector;
        destroyAllWindows(); 
        return 0;
}
