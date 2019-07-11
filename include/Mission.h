//
// Created by chenyuan on 6/26/19.
//

#ifndef _MISSION_H
#define _MISSION_H

#include <iostream>
#include <vector>

#include "opencv2/opencv.hpp"
#include "Command.h"
#include "CTime.h"

#define PRINT(x, arg...) do{ \
                                char buff[20];struct tm *sTm;time_t now = time (0);sTm = gmtime (&now);\
                                strftime (buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", sTm);\
                                if(false) \
                                    fprintf(stdout,"[DEBUG %s ]" x,buff,##arg); \
                            }while(0)

using namespace cv;

class Mission {
    //Singleton ref: https://zhuanlan.zhihu.com/p/37469260 C++11 Thread-Safe
private:
    Mission();

    ~Mission();

    Mission(const Mission &);

    Mission &operator=(const Mission &);

public:
    static Mission &getInstance() {
        static Mission instance;
        return instance;
    }

public:

    std::string commandRecognize(const cv::Mat &in);

    void addCommand(const Command &command);

    void clearCommands();

    std::string exec(bool reverse) {
        json mission;
        if (reverse) {
            for (Command command:commands_) {
                mission.push_back(command.getCommand());
            }
        } else {
            for (auto rit = commands_.rbegin(); rit != commands_.rend(); ++rit)
                mission.push_back((*rit).getCommand());
        }
        return mission.dump(2);
    }

private:
    std::vector<Command> commands_;

    int status_ = 0;  //按下按钮事件去抖标识
    double scale_ = 1;//缩放因子，模板匹配时需要该参数缩放模板
    std::vector<cv::Mat> temps_;//模板匹配步骤中待匹配的图像集合
    std::vector<cv::Mat> num;//数字模板
    std::vector<cv::Mat> direct;//方向模板
    std::vector<cv::Mat> action;//动作模板

    //HSV 绿色的特征值
    int HSV_GREEN[6] = {
            30, 90,
            90, 255,
            0, 200
    };
    CTime ct_;


    void bgr2hsv(const cv::Mat &input, cv::Mat &out, int value[6], bool bit_not) {
        assert(!input.empty());
        cv::Mat hsv_tmp;
        cvtColor(input, hsv_tmp, cv::COLOR_BGR2HSV);
        cv::inRange(hsv_tmp, cv::Scalar(value[0], value[2], value[4]), cv::Scalar(value[1], value[3], value[5]), out);
        if (bit_not)
            bitwise_not(out, out);
    }


    double getmatch(cv::Mat &img, cv::Mat &templ) {

        if (img.cols < templ.cols || img.rows < templ.rows) {
            //perror("template match error");
            return 1;
        }

        int result_cols = img.cols - templ.cols + 1;
        int result_rows = img.rows - templ.rows + 1;


        cv::Mat result;
        result.create(result_rows, result_cols, CV_32FC1);

        /// Do the Matching and Normalize
        matchTemplate(img, templ, result, CV_TM_SQDIFF_NORMED);
        //normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

        /// Localizing the best match with minMaxLoc
        double minVal;
        double maxVal;
        Point minLoc;
        Point maxLoc;
        Point matchLoc;

        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

        return minVal;
    }

    int getbestnummatch(cv::Mat &img, std::vector<cv::Mat> &templs, double scale) {
        std::vector<double> v;
        for (cv::Mat templ:templs) {
            resize(templ, templ, Size(templ.cols * scale, templ.rows * scale));
            v.push_back(getmatch(img, templ));
        }
        auto smallest = std::min_element(std::begin(v), std::end(v));
        if ((*smallest) > 0.2) {
            return 1;
        }
        return std::distance(std::begin(v), smallest) + 1;
    }

    int getbestmatch(cv::Mat &img, std::vector<cv::Mat> &templs, double scale) {
        std::vector<double> v;
        for (cv::Mat templ:templs) {
            resize(templ, templ, Size(templ.cols * scale, templ.rows * scale));
            v.push_back(getmatch(img, templ));
        }
        auto smallest = std::min_element(std::begin(v), std::end(v));
        return std::distance(std::begin(v), smallest) + 1;
    }

    double getbestmatch2(cv::Mat &img, std::vector<cv::Mat> &templs) {
        std::vector<double> v;
        for (cv::Mat templ : templs) {
            v.push_back(getmatch(img, templ));
        }
        auto smallest = std::min_element(std::begin(v), std::end(v));
        return *smallest;
    }

    double getbestmatch3(cv::Mat &img, std::vector<cv::Mat> &templs, double scale) {
        std::vector<double> v;
        for (cv::Mat templ : templs) {
            resize(templ, templ, Size(templ.cols * scale, templ.rows * scale));
            v.push_back(getmatch(img, templ));
        }
        auto smallest = std::min_element(std::begin(v), std::end(v));
        return *smallest;
    }

};

#endif //_MISSION_H
