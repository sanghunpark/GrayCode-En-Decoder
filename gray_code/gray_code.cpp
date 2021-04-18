#include "gray.code.h"
//#include <iostream>
using namespace cv;
using namespace std;

GrayCode::GrayCode(Size img_size, int delay_ms) : _delay_ms(delay_ms)
{
    Mat x_gray_code_image, y_gray_code_image;
    int resolution;
    
    img_size.width >= img_size.height ? resolution = img_size.width : resolution = img_size.height;

    _msb = log(resolution) / log(2); // 1024(2^10) < Resolution <= 2048(2^11)
    x_gray_code_image.create(img_size, CV_MAKETYPE(CV_8U, _msb + 1)); // n-bit
    y_gray_code_image.create(img_size, CV_MAKETYPE(CV_8U, _msb + 1)); // n-bit

    cv::split(x_gray_code_image, _x_gray_code_image_array);
    cv::split(y_gray_code_image, _y_gray_code_image_array);
    _t = _msb * 2 + 1;
}

bool GrayCode::Generate_Gray_Code(Mat& img)
{
    //cout << " T : " << _t << endl;
    if (_t < 0) // done with recording graycode.
    {
        //Record_Gray_Code();
        //cout << "RECORD_GRAY_CODE" << endl;
        _t = _msb * 2 + 1;
        _x_value = false;
        return false;
    }
    if (_encoded == false)
    {
        _idx = _t / 2;
        _inverse = _t % 2;
        _Encode(img, _idx, _x_value, _inverse);
        //cout << " encoding : " << _idx << " inverse :" << _inverse << endl;
        _encoded = true;
        _t--;
    }
    return true;
}
void GrayCode::Record()
{
    waitKey(_delay_ms);
    _encoded = false; 
}


bool GrayCode::End()
{
    if (_t < 0 && _x_value == false && _encoded == false)
        return true;
    else
        return false;
}

void GrayCode::_Encode(cv::Mat image, unsigned code_sequence, bool x_encdoing, bool inverse)
{
    unsigned gray;
    for (int y = 0; y < image.rows; y++)
    {
        for (int x = 0; x < image.cols; x++)
        {
            // horizontal
            gray = x_encdoing ? _Binary_To_Gray(x) : _Binary_To_Gray(y);
            gray = (gray >> code_sequence) & 1;
            // gray_number is n-th gray bit.
            gray^ inverse ? _Set_Image(image, y, x, 255) : _Set_Image(image, y, x, 0);
        }
    }
}

unsigned GrayCode::_Decode(unsigned gray_code, int msb)
{
    unsigned mask_bit = 1 << msb;

    // gray to binary
    unsigned short result = gray_code & mask_bit;
    for (int i = 1; i < msb; i++)
    {
        result |= (gray_code ^ (result >> 1)) & (mask_bit >> i);
    }
    return result;
}

unsigned GrayCode::_Binary_To_Gray(unsigned binary)
{
    return (binary >> 1) ^ binary;
}

void GrayCode::_Set_Image(cv::Mat image, int y, int x, int value)
{
    int channels = image.channels();
    if (channels == 1)
        image.at<uchar>(y, x) = value;
    else
    {
        for (int c = 0; c < channels; c++)
            image.at<cv::Vec3b>(y, x)[c] = value;
    }
}