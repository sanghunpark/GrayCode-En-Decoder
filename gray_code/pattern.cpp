#include "pattern.h"

GrayCode::GrayCode(Size img_size, Recorder& rec, String win_name) : _rec(&rec)
{
    _win_name = win_name;
    int resolution;    
    img_size.width >= img_size.height ? resolution = img_size.width : resolution = img_size.height;

    _msb = log(resolution) / log(2); // 1024(2^10) < Resolution <= 2048(2^11)
    cout << "MSB: " << _msb << endl;
    rec.Init_Gray_Codes(_msb);
    _t = _msb * 2 + 1;
}

bool GrayCode::Generate(Mat& img)
{
    //cout << " T : " << _t << endl;   
    if (_encoded == false) // code is saved
    {
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

        _idx = _t / 2;
        _non_inverse = _t % 2;
        _Encode(img, _idx, _x_value, _non_inverse);
        //cout << " encoding : " << _idx << " inverse :" << _inverse << endl;
        _encoded = true;
        _changed = false;
        _t--;

        imshow(_win_name, img);
        waitKey(10);
        _rec->SetEncodingTime();
    }
    return true;
}

void GrayCode::Record()
{   
    if (!_encoded)
        return;

    if (_changed == false)
        if (_rec->Record(_encoded))
            _changed = true;

    if (_changed || _rec->Delayed())
    {
        _rec->SaveCode(_non_inverse, _x_value, _idx);
        _encoded = false;
        _changed = false;
    }
}

bool GrayCode::End()
{
    if (_t < 0 && _x_value == false && _changed == false)
        return true;
    else
        return false;
}

void GrayCode::_Encode(Mat& image, unsigned code_sequence, bool x_encdoing, bool non_inverse)
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
            gray^ unsigned(non_inverse) ? _Set_Image(image, y, x, 0) : _Set_Image(image, y, x, 255);
        }
    }
}

unsigned GrayCode::Decode(unsigned gray_code, int msb)
{
    unsigned mask_bit = 1 << msb;

    // gray to binary
    unsigned result = gray_code & mask_bit;
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

void GrayCode::_Set_Image(Mat& image, int y, int x, int value)
{
    int channels = image.channels();
    if (channels == 1)
        image.at<uchar>(y, x) = value;
    else
    {
        for (int c = 0; c < channels; c++)
            image.at<Vec3b>(y, x)[c] = value;
    }
}

//// Blob
Blob::Blob(Size img_size, Recorder& rec, Size aspect, String win_name, int scale, float sigma, int shift) : _rec(&rec), _sigma(sigma), _shift(shift)
{
    _win_name = win_name;
    int w = aspect.width * scale;
    int h = aspect.height * scale;
    _interval.width = img_size.width / w;
    _interval.height = img_size.height / h;

    // make a grid for blob patterns
    _grid.clear();
    for (int wi = 1; wi < w; wi++)
        for (int hi = 1; hi < h; hi++)
            _grid.push_back(Point2f(wi * _interval.width, hi * _interval.height));
    rec.Init_Blobs();
    _Create_Blobs(img_size);

    if (_shift <= 0)
        _shift = min(_interval.width, _interval.height) / 4;
    _t = 4;
}

bool Blob::End()
{
    if (_t < 0 && _encoded == false)
        return true;
    else
        return false;
}

bool Blob::Generate(Mat& img)
{
    //cout << " T : " << _t << endl;   
    if (_encoded == false) // code is saved
    {
        
        if (End()) // done with recording a sequence of graycode.
        {
            img = 0;
            return false;
        }

        _t--;        
        _Encode(img, _t);
        //cout << " encoding : " << _idx << " inverse :" << _inverse << endl;
        _encoded = true;
        

        imshow(_win_name, img);
        waitKey(10);
        _rec->SetEncodingTime();
    }
    return true;
}

void Blob::Record()
{
    if (!_encoded)
        return;
    _rec->Record(false); // get frame


    if (_rec->Delayed())
    {
        _rec->SaveBlob(_t);
        _encoded = false;
    }
}

void Blob::_Encode(Mat& image, int t)
{
    cout << "t:" << t << endl;
    // _t: [0 1 2 3], _t%2: [0, 1, 0, 1], int(_t/2): [0, 0, 1, 1]
    // shift: [(-1,-1), (+1,-1),   [00, 10,
    //         (-1,+1), (+1,+1)]    01, 11]
    int t_x, t_y;
    t_x = _t % 2 ? -(_shift) : _shift;
    t_y = int(_t / 2) ? -(_shift) : _shift;

    // shift a blob pattern by index
    Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, t_x, 0, 1, t_y);
    warpAffine(_blob_pattern, image, trans_mat, image.size(), INTER_LINEAR, BORDER_REFLECT_101);
}

void Blob::_Create_Blobs(Size img_size)
{
    Mat temp = Mat(img_size, CV_32F);
    temp = 0;

    // row and col mat
    Mat r = Mat(_interval, CV_32F);
    Mat c = Mat(_interval, CV_32F);
    for (int i = 0; i < r.rows; i++)
        r.row(i) = i;
    for (int i = 0; i < c.cols; i++)
        c.col(i) = i;

    // create 2D gaussian at a poistion 'p0'
    float amplitude = 1.0f;
    Point2f p0(_interval.width / 2, _interval.height / 2);
    Mat x = c - p0.x;
    x = x.mul(x) / (2.0f * _sigma * _sigma);
    Mat y = r - p0.y;
    y = y.mul(y) / (2.0f * _sigma * _sigma);
    Mat v; cv::exp(-(x + y), v);
    v = amplitude * v;

    // add 2D gaussian at a poistion 'p' in the grid.
    for (Point2f p : _grid)
        v.copyTo(temp(Rect(p.x - _interval.width / 2, p.y - _interval.height / 2, _interval.width, _interval.height)));

    normalize(temp, temp, 0.0f, 1.0f, NORM_MINMAX);
    temp.convertTo(_blob_pattern, CV_8UC1, 255);
}