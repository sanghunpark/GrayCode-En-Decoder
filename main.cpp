#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <Windows.h>

#include "gray_code/gray_code.h"
#include "gray_code/recorder.h"


using namespace std;
using namespace cv;

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

int main()
{	
    // set camera
    int delay = 1000;
    CamRecorder cam(delay);

    // create a projection image using a resoltuion of a main monitor
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
        cam.Record();
    while (waitKey(10) != 13);
        
	GrayCode gray_code(pattern.size(), delay, cam);
	while (gray_code.End() != true) {
        gray_code.Generate(pattern);
        gray_code.Record();
	}
         
	return 0;
}