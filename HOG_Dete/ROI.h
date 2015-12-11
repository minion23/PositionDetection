#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

//declaration of global variables
//variables accessed in interruption handler should be set as global
Mat img1;//img1 is the original image. img2 is after processing
Point cordinate;
vector<Point> cordinate_vector;
bool leftdown=false, leftup=false;

//mouse action interruption handler
void mouse_call(int event,int x,int y,int,void*)
{
	if(event==EVENT_LBUTTONDOWN)//when left click
	{
		//record the coordinate when left click
		leftdown=true;
		cordinate.x=x;
		cordinate.y=y;
		cout<<cordinate<<endl;
		cordinate_vector.push_back(cordinate);
	}
	if(event==EVENT_LBUTTONUP)
	{
		leftup=true;
	}
	if(leftdown==true&&leftup==false)
	{
		//display the red point where user choose as vertex
		Point pt;
		pt.x=x;
		pt.y=y;
		Mat temp_img=img1.clone();
		rectangle(temp_img, cordinate, pt, Scalar(0,0,255));
		imshow("Original", temp_img);
	}
	if(leftdown==true&&leftup==true)
	{
		leftdown=false;
		leftup=false;
	}
}

//function to draw region of interest
Mat ROI()
{
	//local variable declaration
	Mat img2;
    int lineType=8;int k=0;
    //img=imread("example.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
    img1=img1*0.98; 

	//show original image
	namedWindow("Original");
	imshow("Original",img1);

	//choose 4-edge region of interest
	//press "q" after choosing each point
	while(k!=4){
	    setMouseCallback("Original", mouse_call);
	    while(char(waitKey(1)!= 'q')){
	    }
        setMouseCallback("Original", NULL, NULL);	
	    k=k+1;
	    cout<<k<<endl;
    }
    const Point* ppt[1] ={&cordinate_vector[0]};
	int npt[] = { 4 };
	fillPoly( img1,ppt,npt,1,Scalar(255,255,255),lineType);
	threshold(img1, img2, 254, 255, 0);
    return img2;
}



//int main()
//{
     //ROI();
	//namedWindow("Region Of Interest"); 
	//imshow("Region Of Interest", img2);
	//while(char(waitKey(1)!='q'));
	//return 0;
//}
