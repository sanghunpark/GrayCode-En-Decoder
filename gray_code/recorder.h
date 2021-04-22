#ifndef _RECORDER_H
#define _RECORDER_H

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Recorder
{
public:
	Recorder(int delay) : _delay(delay) {};
	~Recorder(){};

protected:
	const int _delay;
	std::vector<Mat> _x_gray_code_image_array, _y_gray_code_image_array;
	Mat _frame;

public:
	virtual bool Record(bool inverse, bool encoded = false, bool x_val = true, int idx = 0);
	virtual void SaveCode(bool inverse, bool x_val, int idx);
	void Init_Code_Images(int msb, Size img_size);
	

protected:	
	int _Sum_Pixels(Mat image);
};

class CamRecorder : public Recorder
{
public:
	CamRecorder(int delay, int device_idx=0);
	~CamRecorder(){};

private:
	VideoCapture _vid_cap;
	
public:
#define _THRESHOLD 0.15 // 0 ~ 1
	bool Record(bool inverse=true, bool encoded=false, bool x_val=true, int idx=0) override;
};


#endif // _RECORDER_H
