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
	int _msb;
	vector<Mat> _x_gray_code_image_array, _y_gray_code_image_array;
	vector<Mat> _blob_image_array;
	Mat _frame;

public:
	virtual bool Record(bool encoded = false);
	virtual void SaveCode(bool inverse, bool x_val, int idx);
	virtual void SaveBlob(int t);
	void Init_Gray_Codes(int msb, Size img_size);
	void Init_Blobs(Size img_size);
	virtual vector<pair<Point2f, Point2f>> Detect();
	virtual Point Decode(Point p);

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
#define _THRESHOLD 0.5 // 0 ~ 1
	bool Record(bool encoded=false) override;
	Mat Frame();
};


#endif // _RECORDER_H
