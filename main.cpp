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
#include <curl/curl.h>

using namespace cv;
using namespace std;

int baseline_x_start, baseline_x_end;
int baseline_y_start;                   //use these three line to get the position of people in the figure

inline void directoryExistsOrCreate(const char* pzPath)
{
    DIR *pDir;
    if ( pzPath == NULL || (pDir = opendir (pzPath)) == NULL)
        mkdir(pzPath, 0777);
    else if(pDir != NULL)
        (void) closedir (pDir);
}

int incr = 0;
inline bool saveImg(Mat image, const string DIRECTORY, const string EXTENSION, const char * DIR_FORMAT, const char * FILE_FORMAT)
{
    stringstream ss;
    time_t seconds;
    struct tm * timeinfo;
    char TIME[80];
    time (&seconds);
    // Get the current time
    timeinfo = localtime (&seconds);
    
    // Create name for the date directory
    strftime (TIME,80,DIR_FORMAT,timeinfo);
    ss.str("");
    ss << DIRECTORY << TIME;
    directoryExistsOrCreate(ss.str().c_str());
    ss << "/cropped";
    directoryExistsOrCreate(ss.str().c_str());

    // Create name for the image
    strftime (TIME,80,FILE_FORMAT,timeinfo);
    ss.str("");
    if(incr < 100) incr++; // quick fix for when delay < 1s && > 10ms, (when delay <= 10ms, images are overwritten)
    else incr = 0;
    ss << DIRECTORY << TIME << static_cast<int>(incr) << EXTENSION;
    return imwrite(ss.str().c_str(), image);
}

inline void PostData(int area_number){
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, "127.0.0.1:8010");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, area_number); 

        res = curl_easy_perform(curl);  
  
        if(res != CURLE_OK)  
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));  
        
        curl_easy_cleanup(curl); 
    }
    curl_global_cleanup(); 
}

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
        baseline_x_end = min_x;
        baseline_x_start = max_x;
        baseline_y_start = min_y;
        return number_of_changes;
    }
    return 0;
}

int main()
{   
    const string DIR = "/Users/yangyang/209_test/pic"; // directory where the images will be stored
    const string EXT = ".jpg"; // extension of the images
    const int DELAY = 500; // in mseconds, take a picture every 1/2 second
    const string LOGFILE = "/Users/yangyang/209_test/log";

    // Format of directory
    string DIR_FORMAT = "%d%h%Y"; // 1Jan1970
    string FILE_FORMAT = DIR_FORMAT + "/" + "%d%h%Y_%H%M%S"; // 1Jan1970/1Jan1970_12153
    string CROPPED_FILE_FORMAT = DIR_FORMAT + "/cropped/" + "%d%h%Y_%H%M%S"; // 1Jan1970/cropped/1Jan1970_121539
    
    Mat reference_image1, reference_image2;
    
    //reference_image = imread("reference.jpg",1); //set reference 

    VideoCapture cap(0); // open the video camera no. 0
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    //double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
    //double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
    //cout << "Frame size : " << dWidth << " x " << dHeight << endl;
    //namedWindow("MyVideo",CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
    
    Mat result, result_cropped;
    Mat p_frame, c_frame, n_frame;
    cap.read(reference_image1);  //read the first image as reference image
    cap.read(reference_image2);
    //cap.read(p_frame);
    //result = p_frame;
    //cap.read(c_frame);
    //cap.read(n_frame);

    //cvtColor(c_frame, c_frame, CV_RGB2GRAY);
    //cvtColor(p_frame, p_frame, CV_RGB2GRAY);
    //cvtColor(n_frame, n_frame, CV_RGB2GRAY);
    
    result = reference_image1;
    cvtColor(reference_image1, reference_image1, CV_RGB2GRAY);
    cvtColor(reference_image2, reference_image2, CV_RGB2GRAY);
    //imshow("MyVideo",n_frame);

    Mat difference1 , difference2;
    Mat motion;
    int nChanges;
    int nSequence=0;
    Scalar mean_, color(0,255,255);

    // set threthold. If changed more than motion_threthold, it will be labeled as motion
    // maxDec decide the toleration of motion
    // this two value can be changed
    int motion_threthold = 5;
    int maxDev = 20;
    
    //int x_stop = c_frame.cols, x_start = 0;
    //int y_stop = c_frame.cols, y_start = 0;

    int x_start = 10, x_stop = c_frame.cols-11;
    int y_start = 350, y_stop = c_frame.rows-11;
    //int y_start = 350, y_stop = 530;
    
    cout << c_frame.cols << endl;
    Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2,2));
    
    while (true)
    {
        //Mat frame;
        //bool bSuccess = cap.read(frame); // read a new frame from video
         //if (!bSuccess) //if not success, break loop
        //{
             //cout << "Cannot read a frame from video stream" << endl;
             //break;
        //}
        //imshow("MyVideo", frame); //show the frame in "MyVideo" window
        //p_frame = c_frame;
        //c_frame = n_frame;
        //cap.read(n_frame);
        cap.read(c_frame);
        result = c_frame;
        //result = n_frame;
        //cvtColor(n_frame, n_frame, CV_RGB2GRAY);
        
        cvtColor(c_frame, c_frame, CV_RGB2GRAY); 


        //absdiff(p_frame, n_frame, difference1);
        //absdiff(n_frame, c_frame, difference2);
        
        absdiff(reference_image1, c_frame, difference1);
        absdiff(c_frame, reference_image2, difference2);
        
        bitwise_and(difference1, difference2, motion);
        threshold(motion, motion, 35, 255, CV_THRESH_BINARY); //set threshold to get object from backgroud
        erode(motion, motion, kernel_ero);
        //imshow("MyVideo",motion);
       
        nChanges = detectMotopn(motion, result, result_cropped, x_start, x_stop, y_start, y_stop, maxDev, color);
        cout << "number of changes are"<< nChanges << endl;
       
        if(nChanges>=motion_threthold)
        {
            if(nSequence>0){ 
                saveImg(result,DIR,EXT,DIR_FORMAT.c_str(),FILE_FORMAT.c_str());
                saveImg(result_cropped,DIR,EXT,DIR_FORMAT.c_str(),CROPPED_FILE_FORMAT.c_str());
                cout << "motion detected" << endl;
                
                //imshow("MyVideo", result_cropped); //show the frame in "MyVideo" window
                
            }
            nSequence++;
        }
        else
        {
            nSequence = 0;
            // Delay, wait a 1/2 second.
            cvWaitKey (DELAY);
        }
       
       
        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }

    }

    return 0;
}
