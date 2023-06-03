#pragma once

#include "v2c.hpp"

// get a screenshot from OpenCV
class Screenshot // program from internet
{
public:
    Screenshot();
    double static getZoom();
    Mat getScreenshot();
    Mat getScreenshot(int x, int y, int width, int height);

private:
    int m_width;
    int m_height;
    HDC m_screenDC;
    HDC m_compatibleDC;
    HBITMAP m_hBitmap;
    LPVOID m_screenshotData = nullptr;
};
