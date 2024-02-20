#include <opencv2/opencv.hpp>
#include <vector>

class MovementDetector {
public:
    MovementDetector() {
        backgroundSubtractor = cv::createBackgroundSubtractorMOG2();
        fishShapeContour = {
                {10, 10}, // Tail
                {20, 20}, // Upper body start
                {40, 10}, // Upper body mid
                {60, 20}, // Near head upper
                {70, 30}, // Head top
                {60, 40}, // Near head lower
                {40, 50}, // Lower body mid
                {20, 40}, // Lower body start
                {10, 50}  // Back to tail
        };
    }

    void detectAndCompare(cv::Mat frame) {
        cv::Mat fgMask;
        backgroundSubtractor->apply(frame, fgMask);
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(fgMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto &contour: contours) {
            double area = cv::contourArea(contour);
            if (area >= approximatePixelArea(3) && area <= approximatePixelArea(15)) {
                cv::Rect boundingRect = cv::boundingRect(contour);
                cv::Mat roi = frame(boundingRect);
                cv::Mat binaryRoi;
                cv::cvtColor(roi, binaryRoi, cv::COLOR_BGR2GRAY);
                cv::threshold(binaryRoi, binaryRoi, 50, 255, cv::THRESH_BINARY);

                std::vector<std::vector<cv::Point>> roiContours;
                cv::findContours(binaryRoi, roiContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

                for (const auto &roiContour: roiContours) {
                    double similarity = cv::matchShapes(roiContour, fishShapeContour, cv::CONTOURS_MATCH_I1, 0.0);
                    if (similarity < 0.2) { // Adjust for 80% similarity, lower value means more similar
                        cv::rectangle(frame, boundingRect, cv::Scalar(0, 255, 0), 2);
                        cv::putText(frame, "Fish Detected", cv::Point(boundingRect.x, boundingRect.y - 10),
                                    cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 255, 0), 2);
                    }
                }
            }
        }
    }

private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> backgroundSubtractor;
    std::vector<cv::Point> fishShapeContour;

    double approximatePixelArea(double sizeInCm) {
        double pixelsPerCm = 60; // Adjust based on your camera setup and distance
        return sizeInCm * sizeInCm * pixelsPerCm * pixelsPerCm;
    }
};