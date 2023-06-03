#include "v2c.hpp"
#include "screenshot.h"

map<int, bool> keyCodeTrackerLIB;
const int keyCheckFreq = 60;
bool end_ = false;
double getScale() { // get the scale of current monitor
    // program from internet

    HWND hWnd = GetDesktopWindow();
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX miex;
    miex.cbSize = sizeof(miex);
    GetMonitorInfo(hMonitor, &miex);
    int cxLogical = (miex.rcMonitor.right - miex.rcMonitor.left);
    int cyLogical = (miex.rcMonitor.bottom - miex.rcMonitor.top);

    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    dm.dmDriverExtra = 0;
    EnumDisplaySettings(miex.szDevice, ENUM_CURRENT_SETTINGS, &dm);
    int cxPhysical = dm.dmPelsWidth;
    int cyPhysical = dm.dmPelsHeight;

    double horzScale = ((double)cxPhysical / (double)cxLogical);
    double vertScale = ((double)cyPhysical / (double)cyLogical);

    return (horzScale + vertScale) / 2.0;
}
void KeyCodeTracker()
{
    register int keycodeTrackerSleep = 1000 / keyCheckFreq;
    while (1)
    {
        Sleep(keycodeTrackerSleep);
        if (end_)
        {
            return;
        }
        map<int, bool>::iterator iter;
        iter = keyCodeTrackerLIB.begin();
        while (iter != keyCodeTrackerLIB.end())
        {
            if (!KEY_DOWN(iter->first))
            {
                keyCodeTrackerLIB.erase(iter->first);
                break;
            }
            iter++;
        }
    }
    return;
}
inline void gotoxy(int x, int y)
{
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { (short)x, (short)y });
}
inline bool keyDown(int KeyCode)
{
    return KEY_DOWN(KeyCode);
}
inline bool keyPress(int KeyCode)
{
    if (keyDown(KeyCode))
    {
        bool used = false;
        if (keyCodeTrackerLIB.count(KeyCode) == 0)
        {
            keyCodeTrackerLIB.insert({ KeyCode,true });
            return true;
        }
    }
    return false;
}
inline void fatal(const char* err, bool code = false)
{
    cerr << "\n.\n.\n.\n.\n.\n.\n.\nFATAL ERROR\n\n";
    cerr << "This program ran into a error that it couldn't handle,\n";
    cerr << "and has to exit right now.\n\n";
    if (code)
    {
        cerr << "error-code: " << int(err[0] - '0') << "\n";
    }
    else
    {
        cerr << "error-info: ";
        puts(err);
    }
    cerr << "\n\n\n";
    system("pause");
    exit(err[0]);
}
void usage()
{
    cout << "Usage:" << endl;
    cout << "Directly start the executable file.";
}
void fullScreen(HWND hwnd = nullptr)
{
    if (hwnd == nullptr)
    {
        hwnd = FindWindowA("ConsoleWindowClass", NULL);
    }
    ShowWindow(hwnd, SW_MAXIMIZE);
}
void setFontSize(int size = 6)
{
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;                   // Width of each character in the font
    cfi.dwFontSize.Y = size;                  // Height
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

inline bool inear(float a, float b, float dif = 0.1)
{
    return (a >= b && a - dif <= b);
}

vector<string> img2console(Mat img, int zoom, float threshold)
{
    const float factor = 1.0 * img.cols / img.rows * 2.0;
    float lengthX = zoom;
    float lengthY = zoom / factor;
    float pixelPerCharX = img.cols / lengthX;
    float pixelPerCharY = img.rows / lengthY;
    lengthX = ceil(lengthX);
    lengthY = ceil(lengthY);
    pixelPerCharX = ceil(pixelPerCharX);
    pixelPerCharY = ceil(pixelPerCharY);
    vector<string> frame;
    register string buffer;
    register double val;
    register double average;

    for (int i = 0; i < lengthY; ++i)
    {
        buffer.clear();
        for (int j = 0; j < lengthX; ++j)
        {
            val = 0;
            for (int k = i * pixelPerCharY; k < (i + 1) * pixelPerCharY; ++k)
            {
                for (int l = j * pixelPerCharX; l < (j + 1) * pixelPerCharX; ++l)
                {
                    if (l >= img.cols || k >= img.rows) { continue; };
                    val += (img.at<uchar>({ l,k })) / 127.5;
                }
            }
            average = 1.0 * val / (pixelPerCharX * pixelPerCharY) * threshold;

            if (average > 1.1)
            {
                buffer += '@';
            }
            if (inear(average, 1))
            {
                buffer += '#';
            }
            else if (inear(average, 0.9))
            {
                buffer += '%';
            }
            else if (inear(average, 0.8))
            {
                buffer += '9';
            }
            else if (inear(average, 0.7))
            {
                buffer += 'M';
            }
            else if (inear(average, 0.6))
            {
                buffer += 'H';
            }
            else if (inear(average, 0.5))
            {
                buffer += '?';
            }
            else if (inear(average, 0.4))
            {
                buffer += '=';
            }
            else if (inear(average, 0.3))
            {
                buffer += '/';
            }
            else if (inear(average, 0.2))
            {
                buffer += ':';
            }
            else if (inear(average, 0.1))
            {
                buffer += '.';
            }
            else if (inear(average, 0))
            {
                buffer += ' ';
            }
        }
        frame.push_back(buffer);
    }
    return frame;
}

void ui()
{
    thread keyCodeTrackerProcess(KeyCodeTracker);

    int size = 100;
    float thres = 1;
    vector<string> frame;

    setFontSize(6);
    fullScreen();
    Mat tmp, now;
    Screenshot ss;
    while (true)
    {
        cvtColor(ss.getScreenshot(), now, COLOR_RGB2GRAY);
        gotoxy(0, 0);
        for (int i = 0; i < frame.size(); ++i)
        {
            cout << frame[i] << endl;
        }
        if (keyDown(VK_UP))
        {
            size += 10;
            cmd("cls");
        }
        if (keyDown(VK_DOWN))
        {
            size -= 10;
            cmd("cls");
        }
        if (keyDown(VK_LEFT))
        {
            thres -= 0.1;
            cmd("cls");
        }
        if (keyDown(VK_RIGHT))
        {
            thres += 0.1;
            cmd("cls");
        }
        if (keyDown(VK_ESCAPE))
        {
            cmd("cls");
        }
        frame = img2console(now, size, thres);
    }

    end_ = true;
    if (keyCodeTrackerProcess.joinable())
    {
        keyCodeTrackerProcess.join();
    }
    return;
}

int main(int argc, char *argv[])
{
    system("title Screen2Console");
    setFontSize(14);
    string argv2;
    if (argc > 1)
    {
        usage();
    }
    ui();
    return 0;
}
