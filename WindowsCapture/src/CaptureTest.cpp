
#include "CaptureBase/WindowsCaptureBase.h"


#include <Windows.h>

int main()
{
	WindowsCaptureBase cap;

	// 获取窗口句柄  这种方式只能获取  win32 窗口
	HWND hwnd = FindWindow(NULL, NULL);
	// 还有另一种方式可以枚举所有桌面上可以用本方法进行截图的窗口


	cap.SetCaptureTarget(hwnd);

	while (true)
	{
		cv::Mat ma = cap.GetCaptureImage();
		if (ma.data != NULL)
		{
			cv::imshow("1", ma);
			cv::waitKey(1);
		}
	}


	return 0;
}