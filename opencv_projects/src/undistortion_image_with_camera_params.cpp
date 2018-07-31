/*
    * TODO : Image undistortion with calibrated camera parameters
    * ref 1: https://docs.opencv.org/4.1.0/dc/dbb/tutorial_py_calibration.html
    * ref 1.2 : https://docs.opencv.org/4.1.0/d4/d94/tutorial_camera_calibration.html : camera calib using c++ opencv
    * ref 2: https://docs.opencv.org/4.1.0/dd/d74/tutorial_file_input_output_with_xml_yml.html : read/ write cml/yml files with opencv
    * ref 3: https://docs.opencv.org/4.1.0/d9/d0c/group__calib3d.html#ga7a6c4e032c97f03ba747966e6ad862b1 : getOptimalNewCameraMatrix() api
    * ref 4: https://docs.opencv.org/4.1.0/d9/d0c/group__calib3d.html#ga69f2545a8b62a6b0fc2ee060dc30559d : opencv undistort() api
*/

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>

using namespace cv; // namespace used for opencv

int main()
{
    VideoCapture live_camera(0); // let us take live feed from camera

    if(!live_camera.isOpened()) // check if we succeded in opening camera
    {
        std::cout << "error opening live camera" << std::endl;
        return -1;
    }

    namedWindow("livecamera", WINDOW_NORMAL); // initialize a named window to display live camera feed
    resizeWindow("livecamera", 426, 240); // resize the window to smaller size
    namedWindow("undistorted_live_camera", WINDOW_NORMAL); // initialze a named window to display chessboard with corners drawn on it
    resizeWindow("undistorted_live_camera", 426, 240); // resize the window to smaller size

    // size of the image we use image or camera frame undistortion
    // we shall use the dimension WIDTH=426 && HEIGHT=240
    Size imageSize(426,240);

    // define rotation and translation matrix, camera matrix, distrotion coffecient, to be used as parameters for calibrateCamera() api
    Mat camera_matrix;
    Mat distortion_coefficients;

    // open camera_parameters.yml file to write the camera parameters in it
    FileStorage file_camera_param("../resources/result_opencv_xml_yml_files/camera_calibration_and_3d_reconstruction/camera_parameters.yml", FileStorage::READ);
    
    if(file_camera_param.isOpened()) // if the file above was opened to write then we do the following block
    {
        std::cout << "FOUND the yml file you supplied for reading camera parameters from. We start reading it now..." << std::endl;
        file_camera_param["camera_matrix"] >> camera_matrix;
        std::cout << "Successfully read camera matrix..." << std::endl;
        file_camera_param["distortion_coefficients"] >> distortion_coefficients;
        std::cout << "Successfully read camera distortion coefficients. File read successful. Closing the file now..." << std::endl;
        file_camera_param.release();
        std::cout << "File closed successfully." << std::endl;
    }
    else
    {
        std::cout << "NOT able to read file. Could not save camera parameters. Exiting the application..." << std::endl;
        return 0;
    }

    // initialize an opencv crop rectangle which is a ROI (region of interest) for image to be undisorted
    Rect ROI;

    // initialize an opencv Mat type to store the refined camera matrix based on a free scaling parameter using getOptimalNewCameraMatrix()
    Mat refined_new_camera_matrix;

    // now we refine the camera_matrix based on a free scaling parameter using getOptimalNewCameraMatrix(), to get optimal camera matrix
    // notice that for image size we use same image size we used while calibrating camera
    refined_new_camera_matrix = getOptimalNewCameraMatrix(camera_matrix, distortion_coefficients, imageSize, 1, imageSize);

    // display new live camera frame on every iteration of the never ending loop such that we see a video
    while(1)
    {
        Mat original_live_camera_frame; // create a Mat object to store single frame from live camera feed
        Mat live_camera_frame; // resized live camera frame
        Mat gray_live_camera_frame; // Mat object to store the live camera frame converted to grayscale

        live_camera >> original_live_camera_frame; // get new frame from our initialized videocapture live_camera for this loop iteration
        
        // resize the live feed to 426 width and 240 height image (which we have already initialized as imageSize above) , 
        // we do not need other optional parameters of this resize api ,
        // resizing the image makes the processing faster
        resize(original_live_camera_frame, live_camera_frame, imageSize); 

        cvtColor(live_camera_frame, gray_live_camera_frame, COLOR_BGR2GRAY); // convert the color of live camera frames to grayscale

        imshow("livecamera", gray_live_camera_frame); // show this live camera frame in our previously initialized named window

        Mat undistorted_live_camera_frame; // this Mat object will store undistorted live camera frame

        // create a condition to break this never ending loop : if ESC (ASCII code is 27) key is pressed
        if(waitKey(30) == 27 ) 
        {
            destroyAllWindows();   
            break; // break the while loop
        }
        else
        {
            // now we undistort each live camera frame
            undistort(gray_live_camera_frame, undistorted_live_camera_frame, camera_matrix, /* find a way to send null parameter here */, refined_new_camera_matrix);

            imshow("undistorted_live_camera", undistorted_live_camera_frame);
        }        
    }

    return 0;
}