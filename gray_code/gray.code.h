#ifndef _GRAY_CODE_H
#define _GRAY_CODE_H

#include <opencv2/opencv.hpp>

using namespace cv;

class GrayCode
{
public:
    GrayCode(Size img_size, int delay_ms);
    ~GrayCode(){};

private:
    std::vector<Mat> _x_gray_code_image_array, _y_gray_code_image_array;
    int _msb;
    int _t;
    int _idx = 0;
    const int _delay_ms;

    bool _x_value = true;
    bool _inverse = true;
    bool _encoded = false; // _recored = true;
    //bool _changed = false;


public:
    bool Generate_Gray_Code(Mat& img);
    bool End();
    void Record();
        
private:
    void _Encode(Mat image, unsigned code_sequence, bool horizontal, bool inverse);
    unsigned _Decode(unsigned gray_code, int msb);
    unsigned _Binary_To_Gray(unsigned binary);
    void _Set_Image(Mat image, int y, int x, int value);
};



#endif // _GRAY_CODE_H
