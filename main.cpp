#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

#include "gray_code/gray.code.h"

using namespace std;
using namespace cv;

void main()
{
	
	Mat prj_img; prj_img.create(500, 300, CV_8UC3);

	GrayCode gray_code = GrayCode(prj_img.size(), 200);

	while (gray_code.End() != true) {
		if (gray_code.Generate_Gray_Code(prj_img))
			imshow("Pattern", prj_img);
		gray_code.Record();
	}


	cout << "test";
}