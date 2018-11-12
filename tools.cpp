//
// Created by devin on 10/29/18.
//

#include "tools.h"
static int encodeYUV420SP(std::string iName, uchar iBgr[], int iWidth, int iHeight)
{
    int rtv = SUCCESS;
    const int frameSize = iWidth * iHeight;
    uchar *yuv420sp = new uchar[iWidth * iHeight * 3 / 2];

    int yIndex = 0;
    int uvIndex = frameSize;

    int R, G, B, Y, U, V;
    int idx = 0;
    for (int j = 0; j < iHeight; j++) {
        for (int i = 0; i < iWidth; i++) {
            B = (iBgr[3 * (j * iWidth + i)] & 0xff);
            G = (iBgr[3 * (j * iWidth + i) + 1] & 0xff);
            R = (iBgr[3 * (j * iWidth + i) + 2] & 0xff);

            // Well known RGB to YUV algorithm
            Y = ((  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
            U = (( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
            V = (( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

            // NV21 has a plane of Y and interleaved planes of VU each sampled by a factor of 2
            // Meaning for every 4 Y pixels there are 1 V and 1 U.
            // Note the sampling is every other pixel AND every other scanline.
            yuv420sp[yIndex++] = (uchar)((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
            if (0 == j % 2 && 0 == idx % 2) {
                yuv420sp[uvIndex++] = (uchar)((V < 0) ? 0 : ((V > 255) ? 255 : V));
                yuv420sp[uvIndex++] = (uchar)((U < 0) ? 0 : ((U > 255) ? 255 : U));
            }
            idx++;
        }
    }

    std::ofstream ofile;
    ofile.open(iName, std::ios::binary);
    for (int i = 0; i < iWidth * iHeight * 3 / 2; i++) {
        ofile << yuv420sp[i];
    }
    ofile.close();
    delete[] yuv420sp;
    return rtv;
}

int readFrames(std::string iSource, const std::string iDst)
{
    int rtv = SUCCESS;
    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(100);
    bool isRecording = false;
    int kbv = -1;
    std::string tmp0S;
    int tmp0I = 0;
    int tmp1I = 0;
    const cv::Size dstFrame(1280, 720);
    cv::Mat frame;
    cv::VideoCapture cap;
    cv::namedWindow("Show", cv::WINDOW_NORMAL);
    cv::resizeWindow("Show", dstFrame.width, dstFrame.height);
    cv::moveWindow("Show", 200, 200);

    if (-1 == access(iSource.c_str(), F_OK)) {
        cv::VideoWriter writer;
        cap.open(0);
        writer.open(iSource, CV_FOURCC('M', 'J', 'P', 'G'), 30.0, \
        cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT)));
        if (cap.isOpened() && writer.isOpened()) {
            while (1) {
                cap >> frame;
                writer << frame;
                imshow("Show", frame);
                kbv = cv::waitKey(30) & 0xff;
                if (!cap.grab() || 'q' == kbv) {
                    break;
                }
            }
            cap.release();
            writer.release();
            return SUCCESS;
        }else{
            std::cout << "Writer not opened" << std::endl;
            return ERROR;
        }
    }else{
        cap.open(iSource);
    }

    if (!cap.isOpened()) {
        rtv = ERROR;
        return rtv;
    }else if (-1 == access(iDst.c_str(), F_OK)) {
        if (-1 == mkdir(iDst.c_str(), 0777)) {
            rtv = ERROR;
            return rtv;
        }else{
            std::cout << "Create the fold~" << std::endl;
        }
    }else{
        ;
    }
    const time_t t = time(0);
    struct tm* current_time = localtime(&t);
    tmp0I = current_time->tm_hour * 60 * 60 + current_time->tm_min * 60 + current_time->tm_sec;
    tmp0I *= 1000;

    while (1) {
        cap >> frame;
        kbv = cv::waitKey(30) & 0xff;

        if (frame.empty() || 'q' == kbv) {
            isRecording = false;
            break;
        }else if ('b' == kbv) {
            isRecording = !isRecording;
        }else{
            imshow("Show", frame);
            if (isRecording) {
                tmp0S = std::to_string(tmp0I % 1000);
                while (tmp0S.size() < 3) {
                    tmp0S = "0" + tmp0S;
                }

                tmp0S = '-' + tmp0S;
                tmp1I = tmp0I / 1000;
                tmp0S = '-' + std::to_string(tmp1I % 60) + tmp0S;
                tmp1I = tmp1I / 60;
                tmp0S = '-' + std::to_string(tmp1I % 60) + tmp0S;
                tmp1I = tmp1I / 60;
                tmp0S = std::to_string(tmp1I) + tmp0S;

//              std::cout << tmp0S << std::endl;
//              std::cout << tmp0I << std::endl;
//              std::cout << frame.channels() << std::endl;
                tmp0I += 33;

                cv::resize(frame, frame, dstFrame, 0, 0, cv::INTER_LANCZOS4);
                tmp0S = iDst + '/' + tmp0S + ".nv21";
                encodeYUV420SP(tmp0S, frame.data, dstFrame.width, dstFrame.height);
//                cv::imwrite(iDst + '/' + tmp0S + ".jpg", frame, compression_params);
            }
        }
    }
    cap.release();
    return rtv;
}