#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

#include <fstream>
#include <thread>
#include <windows.h>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define cmd(str) system(((string)str).c_str())
