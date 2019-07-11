//
// Created by chenyuan on 6/26/19.
//

#include "Mission.h"


Mission::Mission() {
    std::string path = "../template/";
    Mat num1 = imread(path + "1.bmp");
    Mat num2 = imread(path + "2.bmp");
    Mat num3 = imread(path + "3.bmp");
    Mat num4 = imread(path + "4.bmp");
    Mat num5 = imread(path + "5.bmp");
    num = {num1, num2, num3, num4, num5};

    Mat up = imread(path + "UP.bmp");
    Mat down = imread(path + "DOWN.bmp");
    Mat left = imread(path + "LEFT.bmp");
    Mat right = imread(path + "RIGHT.bmp");
    direct = {up, down, left, right};

    Mat walk = imread(path + "WALK.bmp");
    Mat jump = imread(path + "JUMP.bmp");
    Mat pick = imread(path + "PICK.bmp");
    Mat loop = imread(path + "LOOP.bmp");
    action = {walk, jump, pick, loop};
}

Mission::~Mission() {
    clearCommands();
}

void Mission::addCommand(const Command &command) {
    commands_.push_back(command);
}

void Mission::clearCommands() {
    if (!commands_.empty()) {
        commands_.clear();
    }
}

std::string Mission::commandRecognize(const cv::Mat &in) {
    //step1
    Mat step3, step4;
    Mat out;
    bgr2hsv(in, out, HSV_GREEN, false);
    //step2
    Mat result = out.clone();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours(result, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    Size rect_size;
    for (int i = 0; i < contours.size(); i++) {
        std::vector<cv::Point> contour = contours[i];
        if (contour.size() < 30) continue;
        RotatedRect rect = cv::minAreaRect(contour);
        // matrices we'll use
        Mat M, rotated, cropped;
        // get angle and size from the bounding box
        float angle = rect.angle;
        rect_size = rect.size;
        // thanks to http://felix.abecassis.me/2011/10/opencv-rotation-deskewing/
        if (rect.angle < -45.) {
            angle += 90.0;
            swap(rect_size.width, rect_size.height);
        }
        // get the rotation matrix
        M = getRotationMatrix2D(rect.center, angle, 1.0);
        // perform the affine transformation
        warpAffine(in, rotated, M, in.size(), INTER_CUBIC);
        // crop the resulting image
        getRectSubPix(rotated, rect_size, rect.center, cropped);
        if (cropped.empty()) continue;
        //cv::imshow("cropped", cropped);
        step3 = cropped.clone();

        //缩放因子，模板匹配时需要该参数缩放模板
        scale_ = cropped.rows / 128.0;

        auto x = rect.center.x - rect_size.width / 2;
        auto y = rect.center.y + rect_size.height / 2;
        auto width = rect_size.width;
        auto height = rotated.rows - rect.center.y - rect_size.height / 2;
        //exception
        if (rotated.empty() || x < 0 || y < 0 || width < 0 || height < 0) continue;
        step4 = rotated(Rect(x, y, width, height));
    }
    PRINT("crop cost %ld us\n", ct_.restart2());
    clearCommands();
    //step3 截取两个凹槽标识图像
    cv::Mat part1, part2;
    part1 = step3(cv::Rect(step3.cols * 0.08, step3.rows * 0.66, step3.cols * 0.18, step3.rows * 0.18));
    part2 = step3(cv::Rect(step3.cols * 0.74, step3.rows * 0.66, step3.cols * 0.18, step3.rows * 0.18));
    if (part1.empty() || part2.empty())
        return "null";
    cvtColor(part1, part1, COLOR_BGR2GRAY);
    cvtColor(part2, part2, COLOR_BGR2GRAY);
    cv::threshold(part1, part1, 80, 256, THRESH_BINARY);
    cv::threshold(part2, part2, 80, 256, THRESH_BINARY);
    //cv::imshow("part1", part1);
    //cv::imshow("part2", part2);
    //waitKey(0);
    //计算两个标识的灰度直方图，统计的灰度范围为[0,127]
    MatND hist1, hist2;
    int channels[] = {0};
    int histSize = 1;
    float range[] = {0, 128};
    const float *histRanges = {range};
    calcHist(&part1, 1, channels, Mat(), // do not use mask
             hist1, 1, &histSize, &histRanges,
             true, // the histogram is uniform
             false);
    calcHist(&part2, 1, channels, Mat(), // do not use mask
             hist2, 1, &histSize, &histRanges,
             true, // the histogram is uniform
             false);
    //std::cout << "M1 = " << " " << hist1.reshape(0, 1) << std::endl << std::endl;
    //std::cout << "M2 = " << " " << hist2.reshape(0, 1) << std::endl << std::endl;
    //std::cout << compareHist(hist1, hist2, CV_COMP_INTERSECT) << std::endl;
    //通过比较两直方图的交叉运算∑min(H1(i), H2(i)),判断是否按下按钮。注意，改变输入图像大小时会改变运算的值。
    if (compareHist(hist1, hist2, CV_COMP_INTERSECT) > 0 && status_ == 0) {
        status_ = 1;
        int ii = 0;
        double best;
        temps_.clear();
        do {
            Mat step4_1 = step4.clone();
            //exception
            if (rect_size.height * ii + rect_size.height * 1.2 > step4_1.rows) break;
            Mat temp = step4_1(Rect(0, rect_size.height * ii, rect_size.width, rect_size.height * 1.2));
            cv::rotate(temp, temp, ROTATE_180);
            //cv::imshow("temp", temp);
            //cv::imshow("step4", step4);
            //waitKey(0);
            best = getbestmatch3(temp, action, scale_);
            //std::cout << getbestmatch3(temp, action,scale) << std::endl;
            if (0.08 > best)
                temps_.push_back(temp);
            ii++;
        } while (0.08 > best);
        for (cv::Mat temp : temps_) {
            std::string a, d, n;
            Action action1;
            Direction direction1;
            Repeat repeat1;
            switch (getbestmatch(temp, action, scale_)) {
                case 1:
                    a = "WALK";
                    action1 = Action::WALK;
                    break;
                case 2:
                    a = "JUMP";
                    action1 = Action::JUMP;
                    break;
                case 3:
                    a = "PICK";
                    action1 = Action::PICK;
                    break;
                case 4:
                    a = "LOOP";
                    action1 = Action::LOOP;
                    break;
            }
            switch (getbestmatch(temp, direct, scale_)) {
                case 1:
                    d = "UP";
                    direction1 = Direction::UP;
                    break;
                case 2:
                    d = "DOWN";
                    direction1 = Direction::DOWN;
                    break;
                case 3:
                    d = "LEFT";
                    direction1 = Direction::LEFT;
                    break;
                case 4:
                    d = "RIGHT";
                    direction1 = Direction::RIGHT;
                    break;
            }
            switch (getbestnummatch(temp, num, scale_)) {
                case 1:
                    repeat1 = Repeat::ONE;
                    break;
                case 2:
                    repeat1 = Repeat::TWO;
                    break;
                case 3:
                    repeat1 = Repeat::THREE;
                    break;
                case 4:
                    repeat1 = Repeat::FOUR;
                    break;
                case 5:
                    repeat1 = Repeat::FIVE;
                    break;
            }
            Command command(action1, direction1, repeat1);
            addCommand(command);
        }
    } else if (compareHist(hist1, hist2, CV_COMP_INTERSECT) == 0) {
        status_ = 0;
    }
    return exec(false);
}
