#include <iostream>
#include <stdio.h>

#include "opencv2/opencv.hpp"
#include "Mission.h"
#include "CTime.h"


using namespace std;
using namespace cv;

int main(int, char **) {
    CTime ct;
    Mat frame;
    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;
    // open the default camera using default API
    cap.open(0);
    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API
    // open selected camera using selected API
    cap.open(deviceID + apiID);
    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 120);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 160);
    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl
         << "Press any key to terminate" << endl;
    for (;;) {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }
        // show live and wait for a key with timeout long enough to show images
        imshow("Live", frame);
        ct.restart();
        std::string result = Mission::getInstance().commandRecognize(frame);
        if (result != "null") {
            PRINT("%d ms\n", ct.restart());
            std::cout << result << endl;
        }
        if (waitKey(30) == 27)
            break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}