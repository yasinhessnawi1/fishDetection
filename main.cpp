
#include <string>
#include "headers/VideoProcessor.h" // Ensure this includes your VideoProcessor class definition

int main() {
    //install opencv with brew install opencv in clion terminal
    // update this path based on your system, right click testvid.mp4 and copy ABSOLUTE path
    std::string videoPath = R"(/Users/yasinhessnawi/Downloads/fishDetection/testvid.mp4)";

        VideoProcessor processor(videoPath);
        processor.process();
}


