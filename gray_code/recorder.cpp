#include "recorder.h"
#include "gray_code.h"
#include <bitset>

bool Recorder::Record(bool encoded)
{
    return encoded; // true whenever pattern is encoded
}

void Recorder::SaveCode(bool _non_inverse, bool x_val, int idx)
{
    static Mat code_frame;
    if (_non_inverse)
        _frame.clone().convertTo(code_frame, CV_16SC3);
        //code_frame = _frame.clone();
    else
    {
        Mat bin;
        Mat inv_frame;
        _frame.clone().convertTo(inv_frame, CV_16SC3);
        Mat(code_frame - inv_frame).convertTo(bin, CV_8UC3);        
        cvtColor(bin, bin, COLOR_BGR2GRAY);
        threshold(bin, bin, 0, 255, THRESH_BINARY);
        
        x_val ?
            bin.clone().convertTo(_x_gray_code_image_array[idx], CV_8UC1) :
            bin.clone().convertTo(_y_gray_code_image_array[idx], CV_8UC1);

        imshow("cap!", bin);
        waitKey(10);
    }
}


void Recorder::SaveBlob(int t)
{
    if (t < 0)
        return;
    cvtColor(_frame, _blob_image_array[t], COLOR_BGR2GRAY);
    imshow("cap!", _blob_image_array[t]);
    waitKey(10);
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

void Recorder::Init_Gray_Codes(int msb, Size img_size)
{    
    Mat x_gray_code_image, y_gray_code_image;
    x_gray_code_image.create(img_size, CV_MAKETYPE(CV_8U, msb + 1)); // n-bit
    y_gray_code_image.create(img_size, CV_MAKETYPE(CV_8U, msb + 1)); // n-bit
    _msb = msb;

    _x_gray_code_image_array.clear();
    _y_gray_code_image_array.clear();
    cv::split(x_gray_code_image, _x_gray_code_image_array);
    cv::split(y_gray_code_image, _y_gray_code_image_array);
}

void Recorder::Init_Blobs(Size img_size)
{
    _blob_image_array.clear();
    _blob_image_array = vector<Mat>(4, Mat(img_size, CV_8UC1));
}

vector<pair<Point2f, Point2f>> Recorder::Detect()
{
    vector<pair<Point2f, Point2f>> cam_prj;
    vector<KeyPoint> cam_pnts;
    SimpleBlobDetector::Params params;
    params.blobColor = 255; // find white blob
    Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
    detector->detect(_blob_image_array[0], cam_pnts);

    for (KeyPoint cam_kp : cam_pnts)
    {
        Point prj_p = Decode(cam_kp.pt);
        cam_prj.push_back(make_pair(cam_kp.pt, prj_p));
    }
    return cam_prj;    
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

bool CamRecorder::Record(bool encoded)
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