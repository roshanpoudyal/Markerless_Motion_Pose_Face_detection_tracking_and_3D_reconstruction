/*  
 * TODO : Test OpenCV application with CMake.
 * Test application to get OpenCV build information and version.
 * ref : https://docs.opencv.org/4.1.0/db/df5/tutorial_linux_gcc_cmake.html
 * ref : https://www.learnopencv.com/get-opencv-build-information-getbuildinformation/
 * ref : https://www.learnopencv.com/how-to-find-opencv-version-python-cpp/
 */

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv)
{
    // print opencv build information
    std::cout << "##################### OpenCV Build Information ##########################" << std::endl;
    std::cout << cv::getBuildInformation() << std::endl;

    // print opencv version information
    std::cout << "##################### OpenCV Version Information ##########################" << std::endl;
    std::cout << "OpenCV version : " << CV_VERSION << std::endl;
    std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
    std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
    std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;
    
    return 0;
}