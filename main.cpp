#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <Windows.h>
#include <filesystem>

#include "gray_code/pattern.h"
#include "gray_code/recorder.h"

#include <bitset>


using namespace std;
using namespace cv;

void GetDesktopResolution(int& horizontal, int& vertical)
{
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    horizontal = desktop.right;
    vertical = desktop.bottom;
}

enum EnDecodeMode
{
    ENCODE,
    ENCODE_DECODE,    
    DECODE
};

int main(int argc, char** argv)
{
    cout << std::filesystem::current_path() << endl;
    //EnDecodeMode mode = ENCODE_DECODE;
    //EnDecodeMode mode = ENCODE;
    EnDecodeMode mode = DECODE;
    string file_name = "./en_decoder_test2.mp4";
    int delay = 1000;


    // set camera       
    Recorder* cam;
    switch (mode)
    {
        case ENCODE_DECODE:
            cam = new CamRecorder(delay);
            break;
        case ENCODE:
            cam = new Recorder(delay);
            break;
        case DECODE:
            cam = new FileRecorder(delay, file_name);
            break;
        default:
            cam = new Recorder(delay);
    }

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
    
    Mat pattern;

    pattern.create(height, width, CV_8UC3);
    pattern = 0;
    // ready to start capturing
    putText(pattern, "Generating blob pattern...", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 255), 2, LINE_AA);
    imshow(win_name, pattern);
    waitKey(10);
    
    Blob blob(Size(width, height), cam, Size(appect_w, aspect_h), win_name, 1, 10, -1);
    GrayCode gray_code(pattern.size(), cam, win_name);

    pattern = 0;
   
    if (mode != DECODE)
    {
        putText(pattern, "Press 'Enter' to start", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 255), 2, LINE_AA);
        imshow(win_name, pattern);
        do
            cam->Frame();
        while (waitKey(10) != 13);

        // gray code acquisition
        while (gray_code.End() != true) {
            if (gray_code.Generate(pattern))
                gray_code.Record();
        }
        // gray blob acquisition
        while (blob.End() != true) {
            if (blob.Generate(pattern))
                blob.Record();
        }
    }
    else
    {
        putText(pattern, "Press 'Enter' to capture patterns...", Point(50, height / 2), FONT_HERSHEY_SIMPLEX, 3, Scalar(255, 255, 255), 2, LINE_AA);
        imshow(win_name, pattern);
        ((FileRecorder*)cam)->CaptureByUser();
    }

    // print decoding results
    if (mode >= ENCODE_DECODE)
    {
        win_name = "Decode";
        namedWindow(win_name);
        Mat img = cam->Frame();
        vector<pair<Point2f, Point2f>> ret = cam->Detect();
        for (pair<Point2f, Point2f> match : ret)
        {
            circle(img, Point(match.first), 5, Scalar(0, 0, 255), FILLED, LINE_AA);
            cout << "cam (" << match.first.x << ", " << match.first.y << ") - " <<
                "prj (" << match.second.x << ", " << match.second.y << ")." << endl;
        }
        imshow(win_name, img);
    }
    waitKey();
    return 0;
}

