#include "recorder.h"
#include "gray_code.h"
#include <bitset>

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
        if (idx == 0)
            cout << "here";
        Mat bin;
        cvtColor(code_frame - _frame.clone(), bin, COLOR_BGR2GRAY);
        threshold(bin, bin, 0, 255, THRESH_BINARY);
        x_val ?
            _x_gray_code_image_array[idx] = bin.clone() :
            _y_gray_code_image_array[idx] = bin.clone();

        imshow("cap!", bin);
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
    _msb = msb;

    cv::split(x_gray_code_image, _x_gray_code_image_array);
    cv::split(y_gray_code_image, _y_gray_code_image_array);
}


Point Recorder::Decode(Point pos)
{
    Point point;
    unsigned gray_code = 0;
    for (int t = 0; t <= _msb; t++)
    {
        gray_code = gray_code << 1;        
        //msb - lsb
        if (_x_gray_code_image_array[_msb - t].at<uchar>(pos.y, pos.x) > 0)
            gray_code = gray_code | 1;
    }
    point.x = GrayCode::Decode(gray_code, _msb);
    /// vertical
    gray_code = 0;
    for (int t = 0; t <= _msb; t++)
    {
        gray_code = gray_code << 1;
        //msb - lsb
        if (_y_gray_code_image_array[_msb - t].at<uchar>(pos.y, pos.x) > 0)
            gray_code = gray_code | 1;
    }
    point.y = GrayCode::Decode(gray_code, _msb);


    return point;
}


//// CamRecorder
CamRecorder::CamRecorder(int delay, int device_idx) : Recorder(delay)
{
    _vid_cap.open(device_idx, CAP_DSHOW);
    if (!_vid_cap.isOpened())
        cout << "Could not open " << device_idx << endl;
    _vid_cap.set(CAP_PROP_AUTO_EXPOSURE, 0);
}

bool CamRecorder::Record(bool inverse, bool encoded, bool x_val, int idx)
{
    static Mat prev_frame;
    _vid_cap >> _frame;
    imshow("Frame", _frame);
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
    //cout << sum << endl;
    prev_frame = _frame.clone();
    if (sum >= _THRESHOLD)
        return true;    
    else
        return false;
}

Mat CamRecorder::Frame()
{
    _vid_cap >> _frame;
    return _frame;
}