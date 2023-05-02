# WindowsCapture
高性能windows截图仓库

# 环境
windows10+
opencv
c++17

# 说明

首先通过WindowsCaptureBase类来进行服务初始化
再使用SetCaptureTarget(HWND)设置要进行截图的窗口句柄
使用GetCaptureImage()来获取窗口截图 格式为cv::mat

注意！获取的mat需要判断data是否为null才能使用。

使用SetFps(int)方法可以设置截图帧率，默认为60hz


可以使用另一个存储库来更好的寻找窗口句柄 [Here](https://github.com/ctsdmm/FindWindowsHwnd)
