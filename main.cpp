#include <iostream>
#include <fstream>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <time.h>
#include <dirent.h>
#include <sstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>


using namespace cv;
using namespace std;

inline int detectMotopn(const Mat & motion, Mat & result, Mat & result_cropped,
                        int x_start, int x_stop, int y_start, int y_stop, int max_deviation,
                        Scalar & color)
{
    Scalar mean, stddev;
    meanStdDev(motion,mean,stddev);
    
    if(stddev[0] < max_deviation)
    {
        int number_of_changes = 0;
        int min_x = motion.cols, max_x = 0;
        int min_y = motion.cols, max_y = 0;
    
        //loop over images to detect
        for(int j = y_start; j<y_stop; j+= 2){      //height of images
            for(int i = x_start; i<x_stop; i+=2){   //width of images
            
                if(static_cast<int>(motion.at<uchar>(j,i)) == 255)
                {
                    number_of_changes ++;
                    if(min_x>j) min_x = i;
                    if(max_x<j) max_x = i;
                    if(min_y>j) min_y = j;
                    if(max_y<j) max_y = j;
                }
            }
    
        }
        if(number_of_changes){
            //check if not out of bounds
            if(min_x-10 > 0) min_x -= 10;
            if(min_y-10 > 0) min_y -= 10;
            if(max_x+10 < result.cols-1) max_x += 10;
            if(max_y+10 < result.rows-1) max_y += 10;
            // draw rectangle round the changed pixel
            Point x(min_x,min_y);
            Point y(max_x,max_y);
            Rect rect(x,y);
            Mat cropped = result(rect);
            cropped.copyTo(result_cropped);
            rectangle(result,rect,color,1);
        }
        return number_of_changes;
    }
    return 0;
}

int main()
{
    VideoCapture cap(0); // open the video camera no. 0
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    //double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    //double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    //cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    Mat result, result_cropped;
    Mat p_frame, c_frame, n_frame;
    cap.read(p_frame);
    result = p_frame;
    cap.read(c_frame);
    cap.read(n_frame);

    cvtColor(c_frame, c_frame, CV_RGB2GRAY);
    cvtColor(p_frame, p_frame, CV_RGB2GRAY);
    cvtColor(n_frame, n_frame, CV_RGB2GRAY);
    
    while (true)
    {
        Mat frame;
        bool bSuccess = cap.read(frame); // read a new frame from video
         if (!bSuccess) //if not success, break loop
        {
             cout << "Cannot read a frame from video stream" << endl;
             break;
        }
        //imshow("MyVideo", frame); //show the frame in "MyVideo" window
         
       
       
       
       
       
       
       
       
       
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }

    }



    return 0;

}
