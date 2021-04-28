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
//
//// gaussian blob test   
//void createGaussian(Size& size, Mat& output, float uX, float uY, float sigmaX, float sigmaY, float amplitude = 1.0f)
//{
//    Mat temp = Mat(size, CV_32F);
//
//    for(int r = 0; r < size.height; r++)
//    {
//        for (int c = 0; c < size.width; c++)
//        {
//            float x = ((c - uX) * (c - uX)) / (2.0f * sigmaX * sigmaX);
//            float y = ((r - uY) * (r - uY)) / (2.0f * sigmaY * sigmaY);
//            float value = amplitude * exp(-(x + y));
//            temp.at<float>(r, c) = value;
//        }
//    }
//    normalize(temp, temp, 0.0f, 1.0f, NORM_MINMAX);
//    output = temp;
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

    String win_name = "Pattern";
    cout << "Resoultion: <" << width << ", " << height << ">" << endl;
    namedWindow(win_name, WINDOW_NORMAL);
    //moveWindow("Pattern", width, height);
    setWindowProperty(win_name, WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

    // blob generation
    Mat pattern; pattern.create(height, width, CV_8UC3);
    pattern = 0;
    // ready to start capturing
    putText(pattern, "Generating blob pattern...", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 255), 2, LINE_AA);
    imshow(win_name, pattern);
    waitKey(10);
    Blob blob(Size(width, height), delay, cam, Size(9, 16), 1, 10);

    pattern = 0;
    putText(pattern, "Press 'Enter' to start", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 255), 2, LINE_AA);
    imshow(win_name, pattern);
    do
        cam.Record();
    while (waitKey(10) != 13);

    // gray code acquisition
    GrayCode gray_code(pattern.size(), delay, cam);
    while (gray_code.End() != true) {
        if (gray_code.Generate(pattern))
            gray_code.Record();
    }
    // gray blob acquisition
    while (blob.End() != true) {
        if (blob.Generate(pattern))
            blob.Record();
    }           

    // Decode test
    win_name = "Decode";
    namedWindow(win_name);
    Mat img = cam.Frame();

    vector<pair<Point2f, Point2f>> ret = cam.Detect();
    for (pair<Point2f, Point2f> match : ret)
    {
        circle(img, Point(match.first), 5, Scalar(0, 0, 255), FILLED, LINE_AA);
        cout << "cam (" << match.first.x << ", " << match.first.y << ") - " <<
            "prj (" << match.second.x << ", " << match.second.y << ")." << endl;
    }
    imshow(win_name, img);
    waitKey();

}


int main1()
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
        if(gray_code.Generate(pattern))
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