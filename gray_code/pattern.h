#ifndef _PATTERN_H
#define _PATTERN_H

#include <opencv2/opencv.hpp>
#include <chrono>

#include "recorder.h"

typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define timeNow() std::chrono::high_resolution_clock::now()
#define duration(x) std::chrono::duration_cast<std::chrono::milliseconds>(x).count()

using namespace cv;
using namespace std;

class Pattern
{
public:
    Pattern() {};
    ~Pattern() {};

protected:
    String _win_name;
};


class GrayCode : public Pattern
{
public:
    GrayCode(Size img_size, int delay, Recorder& rec, String win_name="Pattern");
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
    void _Encode(Mat& image, unsigned code_sequence, bool horizontal, bool inverse);    
    void _Set_Image(Mat& image, int y, int x, int value);
};


class Blob : public Pattern
{
public:
    Blob(Size img_size, int delay, Recorder& rec, Size aspect, String win_name = "Pattern", int scale = 1, float sigma=10, int shift= 1);
    ~Blob() {};

private:
    Size2f _interval;
    vector<Point2f> _grid;

    int _t;
    Recorder* _rec;
    unsigned _delay; // ms
    
    Mat _blob_pattern;
    float _sigma = 10;
    int _shift;

    bool _encoded = false; // _recored = true;
    //bool _changed = false;  // is not used
    TimeVar _pattern_time;

public:
    bool Generate(Mat& img);
    bool End();
    void Record();

private:
    void _Encode(Mat& image, int t);
    void _Create_Blobs(Size img_size);
};


#endif // _PATTERN_H