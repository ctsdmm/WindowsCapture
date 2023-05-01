
#include "CaptureBase/WindowsCaptureBase.h"


#include <Windows.h>

int main()
{
	WindowsCaptureBase cap;

	HWND hwnd = FindWindow(NULL, L"坎特伯雷公主与骑士唤醒冠军之剑的奇幻冒险");

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