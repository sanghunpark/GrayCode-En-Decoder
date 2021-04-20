#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <Windows.h>

#include "gray_code/gray.code.h"


using namespace std;
using namespace cv;

int device_idx = 0;
VideoCapture vid_cap(device_idx, CAP_DSHOW);


#include <iostream>
using namespace std;



// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    // Get a handle to the desktop window
    const HWND hDesktop = GetDesktopWindow();
    // Get the size of screen to the variable desktop
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

int Get_Pixels_Sum(cv::Mat image)
{
    int sum = 0;
    for (int y = 0; y < image.rows; y++)
        for (int x = 0; x < image.cols; x++)
            for (int c = 0; c < image.channels(); c++)
                sum += image.at<cv::Vec3b>(y, x)[c];

    return sum;
}

#define _THRESHOLD 0.1 // 0~1
bool captureFunc()
{
    static Mat prev_frame;
    Mat frame;
    vid_cap >> frame;

    // verification : check if there is any change in image.
    if (prev_frame.size().width == 0)    
        prev_frame = frame.clone();    

    Mat diff = (frame.clone() - prev_frame) + (prev_frame - frame.clone());
    
    //cv::blur(diff_image, diff_image, cv::Size(3,3) );
    float sum = 0;
    sum += Get_Pixels_Sum(diff);
    sum /= (diff.rows * diff.cols * diff.channels() * 255);

    imshow(std::string("diff"), diff);
    waitKey(10);
    //cout << sum << endl;
    prev_frame = frame.clone();
    if (sum >= _THRESHOLD)
    {
        imshow("capture", frame);
        return true;
    }
    else
        return false;
}


int main()
{	
    if (!vid_cap.isOpened())
    {
        cout << "Could not open " << device_idx << endl;
        return -1;
    }    

    // create a projection image
    int width = 0;
    int height = 0;
    GetDesktopResolution(height, width);
    Mat pattern; pattern.create(height, width, CV_8UC3);
    pattern = 0;
    namedWindow("Pattern", WINDOW_NORMAL);
    //moveWindow("Pattern", width, height);
    setWindowProperty("Pattern", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    // ready to start capturing
    putText(pattern, "Press 'Enter' to start", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255,255,255), 2, LINE_AA);
    imshow("Pattern", pattern);
    do    
        captureFunc();
    while (waitKey(0) != 13);
    
	 
	GrayCode gray_code(pattern.size(), 1000, captureFunc);
	while (gray_code.End() != true) {
        if (gray_code.Generate_Gray_Code(pattern))
            gray_code.Record();
	}


     
	return 0;
}