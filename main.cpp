#include <iostream>
#include "tools.h"

int main() {
    std::cout << "Hello!" << std::endl;
//    readFrames("/home/devin/Downloads/20181024ynsd/FirstVideo.avi");
    readFrames("/home/devin/Desktop/tmp/123.avi", "/home/devin/Desktop/tmp/");
    return 0;
}