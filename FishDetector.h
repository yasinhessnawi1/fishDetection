//
// Created by yasin hessnawi on 18/02/2024.
//

#ifndef FISHDETECTION_FISHDETECTOR_H
#define FISHDETECTION_FISHDETECTOR_H


#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>
#include <vector>
#include <map>

class FishDetector {
public:
    FishDetector() {
        backgroundSubtractor = cv::createBackgroundSubtractorMOG2();
    }

    void detectAndTrack(cv::Mat frame) {
        cv::Mat fgMask;
        backgroundSubtractor->apply(frame, fgMask);

        std::vector<std::vector<cv::Point>> contours = findContours(fgMask);
        std::vector<cv::Rect> currentFrameObjects = extractRectangles(filterBySizeAndShape(contours));

        updateObjectHistory(currentFrameObjects);
        bool fishDetected = checkForFluidMovement(currentFrameObjects);

        cv::cvtColor(frame, fgMask, cv::COLOR_BGR2HSV);
        if (fishDetected) {
            for (auto &rect : currentFrameObjects) {
                cv::rectangle(frame, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2);
            }
            std::cout << "Fish detected!" << std::endl;
        }

        cleanupHistory();
    }

private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> backgroundSubtractor;
    std::map<int, std::vector<cv::Rect>> objectHistory;
    int frameCounter = 0;

    void updateObjectHistory(std::vector<cv::Rect> &currentFrameObjects) {
        int objectId = 0;
        for (auto &rect : currentFrameObjects) {
            objectHistory[objectId].push_back(rect);
            objectId++;
        }
    }

    bool checkForFluidMovement(std::vector<cv::Rect> &currentFrameObjects) {
        updateObjectHistory(currentFrameObjects);
        for (auto &entry : objectHistory) {
            auto &positions = entry.second;
            if (positions.size() > 2) {
                bool isSmooth = checkTrajectorySmoothness(positions);
                if (isSmooth) {
                    return true;
                }
            }
        }
        frameCounter++;
        if (frameCounter % 30 == 0) {
            cleanupHistory();
        }
        return false;
    }

    bool checkTrajectorySmoothness(std::vector<cv::Rect> &positions) {
        double totalAngleChange = 0.0;
        int changesCounted = 0;
        for (size_t i = 2; i < positions.size(); i++) {
            cv::Point prevCenter = centerOfRect(positions[i - 2]);
            cv::Point currCenter = centerOfRect(positions[i - 1]);
            cv::Point nextCenter = centerOfRect(positions[i]);

            double angle1 = std::atan2(currCenter.y - prevCenter.y, currCenter.x - prevCenter.x);
            double angle2 = std::atan2(nextCenter.y - currCenter.y, nextCenter.x - currCenter.x);
            double angleChange = std::abs(angle1 - angle2);

            totalAngleChange += angleChange;
            changesCounted++;
        }

        double averageAngleChange = (changesCounted > 0) ? totalAngleChange / changesCounted : 0;
        // Convert 20 degrees to radians manually
        double maxAverageAngleChange = 20 * (3.14159265358979323846 / 180.0);
        return averageAngleChange < maxAverageAngleChange; // Allow up to 20 degrees change on average
    }


    cv::Point centerOfRect(const cv::Rect &rect) {
        return cv::Point(rect.x + rect.width / 2, rect.y + rect.height / 2);
    }

    std::vector<cv::Rect> extractRectangles(std::vector<std::vector<cv::Point>> filteredContours) {
        std::vector<cv::Rect> rectangles;
        for (auto &contour : filteredContours) {
            rectangles.push_back(cv::boundingRect(contour));
        }
        return rectangles;
    }

    void cleanupHistory() {
        int historyThreshold = 30;
        for (auto it = objectHistory.begin(); it != objectHistory.end();) {
            if (it->second.size() > historyThreshold && frameCounter - getLastSeenFrame(it->second) > historyThreshold) {
                it = objectHistory.erase(it);
            } else {
                ++it;
            }
        }
    }

    int getLastSeenFrame(std::vector<cv::Rect> &positions) {
        return frameCounter - positions.size();
    }

    std::vector<std::vector<cv::Point>> findContours(cv::Mat &frame) {
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        return contours;
    }

    std::vector<std::vector<cv::Point>> filterBySizeAndShape(std::vector<std::vector<cv::Point>> &contours) {
        std::vector<std::vector<cv::Point>> filteredContours;
        for (auto &contour : contours) {
            double area = cv::contourArea(contour);
            double minArea = approximatePixelArea(3); // Min size in cm to pixels
            double maxArea = approximatePixelArea(10); // Max size in cm to pixels

            if (area >= minArea && area <= maxArea) {
                cv::Rect boundingRect = cv::boundingRect(contour);
                double aspectRatio = static_cast<double>(boundingRect.width) / boundingRect.height;

                if (aspectRatio > 2 && aspectRatio < 6) {
                    filteredContours.push_back(contour);
                }
            }
        }
        return filteredContours;
    }

    double approximatePixelArea(double sizeInCm) {
        double pixelsPerCm = 60; // Example conversion based on assumed camera setup
        return sizeInCm * sizeInCm * pixelsPerCm * pixelsPerCm;
    }
};



#endif //FISHDETECTION_FISHDETECTOR_H
