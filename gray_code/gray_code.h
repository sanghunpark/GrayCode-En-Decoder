#ifndef _GRAY_CODE_H
#define _GRAY_CODE_H

#include <opencv2/opencv.hpp>
#include <chrono>

#include "recorder.h"

typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define timeNow() std::chrono::high_resolution_clock::now()
#define duration(x) std::chrono::duration_cast<std::chrono::milliseconds>(x).count()

using namespace cv;
using namespace std;

class GrayCode
{
public:
    GrayCode(Size img_size, int delay, Recorder& rec);
    ~GrayCode(){};

private:    
    unsigned _msb;
    int _t;
    unsigned _idx = 0;
    unsigned _delay; // ms
    Recorder* _rec;

    bool _x_value = true;
    bool _non_inverse = true;
    bool _encoded = false; // _recored = true;
    bool _changed = false;
    TimeVar _pattern_time;

public:
    bool Generate(Mat& img);
    bool End();
    void Record();
    static unsigned Decode(unsigned gray_code, int msb);
    unsigned _Binary_To_Gray(unsigned binary);
private:
    void _Encode(Mat image, unsigned code_sequence, bool horizontal, bool inverse);
    
    void _Set_Image(Mat image, int y, int x, int value);
};



#endif // _GRAY_CODE_H
