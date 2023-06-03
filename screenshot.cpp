#include "Screenshot.h"
using cv::Mat;

Screenshot::Screenshot()
{
    double zoom = getZoom();
    m_width = GetSystemMetrics(SM_CXSCREEN) * zoom;
    m_height = GetSystemMetrics(SM_CYSCREEN) * zoom;
    m_screenshotData = new char[m_width * m_height * 4];
    memset(m_screenshotData, 0, m_width);

    m_screenDC = GetDC(NULL);
    m_compatibleDC = CreateCompatibleDC(m_screenDC);

    m_hBitmap = CreateCompatibleBitmap(m_screenDC, m_width, m_height);
    SelectObject(m_compatibleDC, m_hBitmap);
}

Mat Screenshot::getScreenshot()
{
    BitBlt(m_compatibleDC, 0, 0, m_width, m_height, m_screenDC, 0, 0, SRCCOPY);
    GetBitmapBits(m_hBitmap, m_width * m_height * 4, m_screenshotData);

    Mat screenshot(m_height, m_width, CV_8UC4, m_screenshotData);

    return screenshot;
}

Mat Screenshot::getScreenshot(int x, int y, int width, int height)
{
    Mat screenshot = getScreenshot();
    return screenshot(cv::Rect(x, y, width, height));
}

double Screenshot::getZoom()
{
    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(hMonitor, &monitorInfo);
    int cxLogical = (monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left);

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;

    return cxPhysical * 1.0 / cxLogical;
}