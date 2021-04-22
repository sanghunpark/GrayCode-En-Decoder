#include "gray_code.h"

GrayCode::GrayCode(Size img_size, int delay, Recorder& rec) : _rec(&rec), _delay(delay)
{
    int resolution;    
    img_size.width >= img_size.height ? resolution = img_size.width : resolution = img_size.height;

    _msb = log(resolution) / log(2); // 1024(2^10) < Resolution <= 2048(2^11)
    rec.Init_Code_Images(_msb, img_size);
    _t = _msb * 2 + 1;
}

bool GrayCode::Generate(Mat& img)
{
    //cout << " T : " << _t << endl;
    if (_t < 0) // done with recording a sequence of graycode.
    {
        if (End())
        {
            img = 0;
            return false;
        }
        
        _t = _msb * 2 + 1;
        _x_value = false;
        return false;
    }
    if (_encoded == false)
    {
        _idx = _t / 2;
        _non_inverse = _t % 2;
        _Encode(img, _idx, _x_value, _non_inverse);
        //cout << " encoding : " << _idx << " inverse :" << _inverse << endl;
        _encoded = true;
        _changed = false;
        _t--;

        imshow("Pattern", img);
        waitKey(10);
        _pattern_time = timeNow();
    }
    return true;
}

void GrayCode::Record()
{   
    if (_changed == false)
        if (_rec->Record(_non_inverse, _encoded))
            _changed = true;
    
    auto dur = duration(timeNow() - _pattern_time);
    if (_changed || dur >= _delay)
    {
        _rec->SaveCode(_non_inverse, _x_value, _idx);
        _encoded = false;
        _changed = false;
    }
}

bool GrayCode::End()
{
    if (_t < 0 && _x_value == false)
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