#include "recorder.h"

bool Recorder::Record(bool inverse, bool encoded, bool x_val, int idx)
{
    return encoded; // true whenever pattern is encoded
}

void Recorder::SaveCode(bool _non_inverse, bool x_val, int idx)
{
    static Mat code_frame;
    if (_non_inverse)
        code_frame = _frame.clone();
    else
    {
        x_val ?
            cvtColor(code_frame - _frame.clone(), _x_gray_code_image_array[idx], COLOR_BGR2GRAY) :
            cvtColor(code_frame - _frame.clone(), _y_gray_code_image_array[idx], COLOR_BGR2GRAY);

        imshow("cap!", code_frame - _frame.clone());
        waitKey(10);
    }
}

int Recorder::_Sum_Pixels(Mat image)
{
    int sum = 0;
    for (int y = 0; y < image.rows; y++)
        for (int x = 0; x < image.cols; x++)
            for (int c = 0; c < image.channels(); c++)
                sum += image.at<cv::Vec3b>(y, x)[c];
    return sum;
}

void Recorder::Init_Code_Images(int msb, Size img_size)
{
    Mat x_gray_code_image, y_gray_code_image;
    x_gray_code_image.create(img_size, CV_MAKETYPE(CV_8U, msb + 1)); // n-bit
    y_gray_code_image.create(img_size, CV_MAKETYPE(CV_8U, msb + 1)); // n-bit

    cv::split(x_gray_code_image, _x_gray_code_image_array);
    cv::split(y_gray_code_image, _y_gray_code_image_array);
}

//// CamRecorder
CamRecorder::CamRecorder(int delay, int device_idx) : Recorder(delay)
{
    _vid_cap.open(device_idx, CAP_DSHOW);
    if (!_vid_cap.isOpened())
        cout << "Could not open " << device_idx << endl;
}

bool CamRecorder::Record(bool inverse, bool encoded, bool x_val, int idx)
{
    static Mat prev_frame;
    _vid_cap >> _frame;
    if (!encoded || prev_frame.size().width == 0)
    {
        prev_frame = _frame.clone();
        return false;
    }

    Mat diff = (_frame.clone() - prev_frame) + (prev_frame - _frame.clone());

    // cv::blur(diff_image, diff_image, cv::Size(3,3));
    float sum = 0;
    sum += _Sum_Pixels(diff);
    sum /= (diff.rows * diff.cols * diff.channels() * 255);

    imshow(std::string("diff"), diff);
    waitKey(10);
    cout << sum << endl;
    prev_frame = _frame.clone();
    if (sum >= _THRESHOLD)
        return true;    
    else
        return false;
}

