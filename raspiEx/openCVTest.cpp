#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<iostream>
#include<string>

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
	Mat image;
	Mat convert_image;
	String file_name = "./test.jpg";
	image = imread(file_name, CV_WINDOW_AUTOSIZE);
	if(image.empty())
	{
		cout<<"Image is empty."<<std::endl;
		return -1;
	}

	cvtColor(image, convert_image, COLOR_BGR2GRAY);

	namedWindow("ING", WINDOW_AUTOSIZE);
	namedWindow("ING2", WINDOW_AUTOSIZE);

	imshow("ING", image);
	imshow("ING2", convert_image);
	
	imwrite("./conver.jpg", convert_image);
	waitKey(0);

	return 0;
}
