#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <Windows.h>

#include "gray_code/gray_code.h"
#include "gray_code/recorder.h"


#include <bitset>


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

void mouse_callback(int event, int x, int y, int flag, void* param)
{
    if (event == EVENT_MOUSEMOVE) {
        CamRecorder& cam = *((CamRecorder*) param);
        Point pttn_p = cam.Decode(Point(x, y));
        cout << "(" << x << ", " << y << ") - <" << pttn_p.x << ", " << pttn_p.y << ">" << endl;
    }
}

// gray code test
//int main1()
//{
//    // set camera
//    int delay = 500;
//    CamRecorder cam(delay);
//
//    // create a projection image using a resoltuion of a main monitor
//    int width = 0;
//    int height = 0;
//    GetDesktopResolution(height, width);
//    Mat pattern; pattern.create(height, width, CV_8UC3);
//    GrayCode gray_code(pattern.size(), delay, cam);
//
//    for (int val = 1; val <= pow(2, 30); val *= 2)
//    {
//        //int val = 1;
//        val += 2;
//        cout << val << "  bin: " << bitset<16>(val) << endl;
//        cout << val << " gray: " << bitset<16>(gray_code._Binary_To_Gray(val)) << endl;
//        int _msb = pow(2, 30) / log(2);
//        cout << val << "  rec: " << bitset<16>(gray_code.Decode(gray_code._Binary_To_Gray(val), _msb)) << endl;
//        cout << " ---------------------------------------- " << endl;
//    }
//    return 0;
//}

int main()
{	
    // set camera
    int delay = 500;
    CamRecorder cam(delay);

    // create a projection image using a resoltuion of a main monitor
    int width = 0;
    int height = 0;
    GetDesktopResolution(height, width);
    Mat pattern; pattern.create(height, width, CV_8UC3);
    pattern = 0;
    String win_name = "Pattern";
    cout << "<" << width << ", " << height << ">" << endl;
    namedWindow(win_name, WINDOW_NORMAL);
    //moveWindow("Pattern", width, height);
    setWindowProperty(win_name, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    // ready to start capturing
    putText(pattern, "Press 'Enter' to start", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255,255,255), 2, LINE_AA);
    imshow(win_name, pattern);
    do
        cam.Record();
    while (waitKey(10) != 13);
        
	GrayCode gray_code(pattern.size(), delay, cam);
	while (gray_code.End() != true) {
        gray_code.Generate(pattern);
        gray_code.Record();
	}

    // Decode test
    win_name = "Decode";
    namedWindow(win_name);
    setMouseCallback(win_name, mouse_callback, (void*) &cam );
    imshow(win_name, cam.Frame());
    waitKey();
         
	return 0;
}