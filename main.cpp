#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <Windows.h>

#include "gray_code/pattern.h"
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

int main()
{
    // set camera
    int delay = 1000;
    CamRecorder cam(delay);

    // create a projection image using a resoltuion of a main monitor
    int width = 0;
    int height = 0;
    int appect_w = 16;
    int aspect_h = 9;
    GetDesktopResolution(width, height);

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
    Blob blob(Size(width, height), cam, Size(appect_w, aspect_h), win_name, 1, 10, -1);

    pattern = 0;
    putText(pattern, "Press 'Enter' to start", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 255), 2, LINE_AA);
    imshow(win_name, pattern);
    do
        cam.Record();
    while (waitKey(10) != 13);

    // gray code acquisition
    GrayCode gray_code(pattern.size(), cam, win_name);
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