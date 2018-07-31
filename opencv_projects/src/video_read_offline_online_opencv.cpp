/*  
 * TODO : Read video using opencv offline or online
 * Test application to get OpenCV build information and version.
 * ref : https://docs.opencv.org/4.1.0/d5/dc4/tutorial_video_input_psnr_ssim.html -> section : How to read a video stream (online-camera or offline-file)? - has important informations
 * ref : https://docs.opencv.org/3.0-beta/modules/videoio/doc/reading_and_writing_video.html -> has just a simple implementation from older version but can be used for newer version also
 */

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int main()
{
    VideoCapture live_camera(0); // use this format to read live (online) camera device - usually the device id start from 0, if you have more camera it can +1 on the id
    // VideoCapture offline_video("some_video_filename.some_video_format"); // use this format if you want to read from a video file

    if(!live_camera.isOpened()) // check if we succeded in opening camera for video or video file
    {
        std::cout << "error opening live camera stream or file" << std::endl;
        return -1;
    }

    namedWindow("live_camera",1); // initialize a named window to display live camera feed

    // display new live camera frame on every iteration of the never ending loop such that we see a video
    while(1)
    {
        Mat live_camera_frame; // Create new mat object to store an image
        live_camera >> live_camera_frame; // get new frame from camera for this loop iteration
        imshow("live_camera", live_camera_frame); // show this live camera frame in our previously initialized named window 

        // if an ESC (ASCII code is 27) key is pressed or video stream online (live) or offline is finished we break from the loop
        if(waitKey(30) == 27 || live_camera_frame.empty()) break;
        // if(waitKey(30) == 27 || offline_video_stream.empty()) break;
    }
    
    return 0;
}