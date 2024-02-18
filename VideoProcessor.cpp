#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio/videoio.hpp>
#include "FishDetector.h" // Make sure to include the header file for the FishDetector class

class VideoProcessor {
public:
    VideoProcessor(const std::string& videoPath) {
        capture.open(videoPath);
        detector = FishDetector();
        // Ensure the FishDetector is initialized if needed
    }

    void process() {
        cv::Mat frame;
        while (capture.read(frame)) {
            detector.detectAndTrack(frame);

            cv::resize(frame, frame, cv::Size(800, 600));
            cv::imshow("Video Frame", frame);
            if (cv::waitKey(20) >= 0) break; // Exit if a key is pressed
        }
        capture.release();
        cv::destroyAllWindows();
    }

private:
    cv::VideoCapture capture;
    FishDetector detector; // Assuming you have a corresponding C++ FishDetector class
};

