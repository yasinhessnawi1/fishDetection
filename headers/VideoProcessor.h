#ifndef FISHDETECTION_VIDEOPROCESSOR_H
#define FISHDETECTION_VIDEOPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include "MovementDetector .h "// Include the MovementDetector header

class VideoProcessor {
public:
    VideoProcessor(const std::string& videoPath) {
        capture.open(videoPath);
        if (!capture.isOpened()) {
            throw std::runtime_error("Could not open video file: " + videoPath);
        }
        // Initialize the MovementDetector
        detector = MovementDetector();
    }

    void process() {
        cv::Mat frame;
        while (capture.read(frame)) {
            // Use MovementDetector to detect movements in the frame
            detector.detectAndCompare(frame);

            cv::resize(frame, frame, cv::Size(800, 600)); // Optional: Resize for display
            cv::imshow("Video Frame", frame);
            if (cv::waitKey(20) >= 0) break; // Exit if a key is pressed
        }
        capture.release();
        cv::destroyAllWindows();
    }

private:
    cv::VideoCapture capture;
    MovementDetector detector; // Use MovementDetector for movement detection
};

#endif //FISHDETECTION_VIDEOPROCESSOR_H
