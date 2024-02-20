
#include <string>
#include "headers/VideoProcessor.h" // Ensure this includes your VideoProcessor class definition

int main() {
    // update this path based on your system, right click testvid.mp4 and copy ABSOLUTE path
    std::string videoPath = "C:\\Users\\ra91m\\Downloads\\ANADROM\\Video\\rami5.mp4";

    VideoProcessor processor(videoPath);
    processor.process();

}
