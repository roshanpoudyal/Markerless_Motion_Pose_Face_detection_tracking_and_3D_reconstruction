/*
    * TODO : camera calibration
    * ref 1: https://docs.opencv.org/4.1.0/dc/dbb/tutorial_py_calibration.html
    * ref 1.2 : https://docs.opencv.org/4.1.0/d4/d94/tutorial_camera_calibration.html : camera calib using c++ opencv
    * ref 2: https://www.cds.caltech.edu/~murray/wiki/images/6/68/Assignment_1_MECS132A.pdf
    * ref 3: https://stackoverflow.com/a/10346021
    * ref 4: https://docs.opencv.org/4.1.0/d9/d0c/group__calib3d.html#ga93efa9b0aa890de240ca32b11253dd4a : sample usage of detecting and drawing chessboard corners section in this link
    * ref 5: https://docs.opencv.org/4.1.0/dd/d1a/group__imgproc__feature.html#ga354e0d7c86d0d9da75de9b9701a9a87e : for cornerSubPix() function definition
    * ref 6: https://docs.opencv.org/3.4/da/d54/group__imgproc__transform.html#ga47a974309e9102f5f08231edc7e7529d : resize() api for image in opencv
    * ref 7: https://docs.opencv.org/4.1.0/d9/d0c/group__calib3d.html#ga3207604e4b1a1758aa66acb6ed5aa65d : calibrateCamera() api and its parameters description
    * ref 8: https://docs.opencv.org/4.1.0/dd/d74/tutorial_file_input_output_with_xml_yml.html : read, write xml/yml files with opencv
    * 
    * Tip : while calibrating camera from live video feed, like we are doing. It was found that if the room was illuminated better, for example in bright daylight,
    *       the chessboard patterns were found easily (I do not have the reason for it now). If you do not have bright sunny day or similar, it can help if you can,
    *       point a direct light to the camera you are using for calibration and then feed the chessboard pattern to the camera.
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
    namedWindow("drawcorners", WINDOW_NORMAL); // initialze a named window to display chessboard with corners drawn on it
    resizeWindow("drawcorners", 426, 240); // resize the window to smaller size

    // create cv::Point3f vector to store 3D object points from all images
    // to learn what are objectPoints, see calibrateCamera() api documentation and its parameter description in ref 7 above
    std::vector<std::vector<Point3f>> objectPoints;

    // create cv::Point2f vector to store 2D image points from all images
    // to learn what are imagePoints, see calibrateCamera() api documentation and its parameter description in ref 7 above
    std::vector<std::vector<Point2f>> imagePoints;

    // create Size object instance that will store the pattern size of our input chessboard image
    // pattern size is the number of corners in a row by number of corners in a column
    Size chessboard_pattern_size(7,7);

    // size of the image we use for camera calibration and for initialization of intrinsic camera parameters
    // we shall use the dimension WIDTH=426 && HEIGHT=240
    Size imageSize(426,240);

    // create vector of opencv type Point3f to store detected object corners (in 3d)
    std::vector<Point3f> object_corners;

    // create vector of opencv type Point2f to store detected object corners (in 2d) in chessboard image,
    // this will first be udpated by findChessboardCorners() api and later refined with cornerSubPix() api, if chessboard pattern is found
    std::vector<Point2f> chessboard_image_corners;

    // now initialize object_corners, when all the images used for calibration will have similar corner size, Object corners will be same for all images and can be reused
    // Additionally, the third dimension is zero because we have our object (chessboard pattern image) in 3D with no z-dimension
    for(int i=0; i < chessboard_pattern_size.height; i++)
    {
        for(int j=0; j < chessboard_pattern_size.width; j++)
        {
            object_corners.push_back(Point3f(i,j,0.0f)); 
        }
    }

    // define rotation and translation matrix, camera matrix, distrotion coffecient, to be used as parameters for calibrateCamera() api
    Mat camera_matrix;
    Mat distortion_coefficients;
    Mat rotation_vectors;
    Mat translation_vectors;

    // counter to keep count of number of chessboard pattern found, initialized to zero
    u_int chessboard_pattern_found_count = 0;

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

        // now let us find chess board corners and return the status if found as well
        bool chessboard_pattern_found = findChessboardCorners(gray_live_camera_frame, 
                                                    chessboard_pattern_size,
                                                    chessboard_image_corners,
                                                    CALIB_CB_ADAPTIVE_THRESH
                                                    + CALIB_CB_NORMALIZE_IMAGE
                                                    + CALIB_CB_FAST_CHECK);
        
        // create a condition to break this never ending loop : if ESC (ASCII code is 27) key is pressed
        if(waitKey(30) == 27 ) 
        {
            destroyAllWindows();   
            break; // break the while loop
        }
        else if(chessboard_pattern_found) // if chessboard pattern is found do these computations in this scope 
        {
            std::cout << "number of chessboard pattern found = " << ++chessboard_pattern_found_count << std::endl;

            // the detected corners with findChessboardCorners() function is approximate
            // cornerSubPix() function iterates to find the sub-pixel accurate location of corners
            // and refines corner locations for more accuracy
            cornerSubPix(gray_live_camera_frame, chessboard_image_corners, Size(11,11), Size(-1,-1),
                    TermCriteria(TermCriteria::EPS + TermCriteria::MAX_ITER, 30, 0.1));
            
            // if number of corners found in image is equal to size of chessboard_pattern
            // push image corners to imagePoints and push object corners to objectPoints for current image
            // imagePoints and objectPoints will be used to calibrate camera later
            if(chessboard_image_corners.size() == chessboard_pattern_size.area()){
                imagePoints.push_back(chessboard_image_corners);
                objectPoints.push_back(object_corners);
            }

            // now draw chess board corners on every frame a pattern is found but on not grayscale but colorful video frame
            // so that the drawing is seen colorful
            drawChessboardCorners(live_camera_frame, chessboard_pattern_size, chessboard_image_corners, chessboard_pattern_found);

            imshow("drawcorners", live_camera_frame);
        }        
    }

    // you are here! it means that the while loop has broken, we no longer read any images
    std::cout << " you have stopped the while loop. We are no longer taking image inputs. " << std::endl;
    // now we shall calibrate the camera, as we have the required objectPoints and corresponding imagePoints
    std::cout << " Now, calibrating camera... " << std::endl;
    calibrateCamera(objectPoints, imagePoints, imageSize, camera_matrix, distortion_coefficients, 
            rotation_vectors, translation_vectors, 0);

    // finally we save the parameter information in an xml file
    std::cout << " Now, we try to write camera parameter information in a xml/yml file... " << std::endl;
    
    // open camera_parameters.yml file to write the camera parameters in it
    FileStorage file_camera_param("../resources/result_opencv_xml_yml_files/camera_calibration_and_3d_reconstruction/camera_parameters.yml", FileStorage::WRITE);
    
    if(file_camera_param.isOpened()) // if the file above was opened to write then we do the following block
    {
        std::cout << "we were able to read file - camera_parameters.yml- now we write camera parameters in this file..." << std::endl;
        std::cout << "writing camera matrix - the intrinsic camera parameters..." << std::endl;
        file_camera_param << "camera_matrix" << camera_matrix; // write intrinsic camera parameters : camera matrix

        std::cout << "writing camera distortion coefficients..." << std::endl;
        file_camera_param << "distortion_coefficients" << distortion_coefficients; // write distortion coefficients

        std::cout << "closing the opened file..." << std::endl;
        file_camera_param.release();

        std::cout << "exiting the application..." << std::endl;
    }
    else
    {
        std::cout << "NOT able to read file. Could not save camera parameters. Exiting the application..." << std::endl;
    }

    return 0;
}