//
// Created by devin on 10/29/18.
//

#ifndef PCFRAMERECORDING_TOOLS_H
#define PCFRAMERECORDING_TOOLS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <fstream>
#include "opencv2/opencv.hpp"
enum returnValue
{
    SUCCESS = 0,
    ERROR
};
int readFrames(std::string iSource, const std::string iDst);
#endif //PCFRAMERECORDING_TOOLS_H
