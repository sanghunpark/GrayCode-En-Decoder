#ifndef _RECORDER_H
#define _RECORDER_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace cv;
using namespace std;


typedef std::chrono::high_resolution_clock::time_point TimeVar;
#define timeNow() std::chrono::high_resolution_clock::now()
#define duration(x) std::chrono::duration_cast<std::chrono::milliseconds>(x).count()


class Recorder
{
public:
	Recorder(int delay);
	~Recorder(){};

protected:
	Ptr<SimpleBlobDetector> _det;
	const int _delay;
	int _msb;
	vector<Mat> _x_gray_code_image_array, _y_gray_code_image_array;
	vector<Mat> _blob_image_array;
	Mat _frame;
	TimeVar _pattern_time;
	

public:
	virtual bool Changed(bool encoded = false);
	virtual void SaveGray(bool inverse, bool x_val, int idx);
	virtual void SaveBlob(int t);
	virtual bool Delayed();
	virtual void SetEncodingTime();
	virtual Mat Frame();
	void Init_Gray_Codes(int msb);
	void Init_Blobs();

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

protected:
	VideoCapture _vid_cap;
	
public:
#define _THRESHOLD 0.5 // 0 ~ 1
	bool Changed(bool encoded=false) override;
	void SaveGray(bool inverse, bool x_val, int idx) override;
	void SaveBlob(int t) override;
	Mat Frame() override;
};

class FileRecorder : public CamRecorder
{
public:
	FileRecorder(int delay, string file);
	~FileRecorder(){};

	Mat Frame() override;
	void CaptureByUser();

	static void onTrackBarSlide(int pos, void*);
	static void onButton(int state, void* ptr);

private:
	
	int _slider_pos = 0;
};

#endif // _RECORDER_H
