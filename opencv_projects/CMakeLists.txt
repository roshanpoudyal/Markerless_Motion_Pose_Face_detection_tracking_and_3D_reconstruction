cmake_minimum_required(VERSION 2.8)
project( opencv_projects )

file(GLOB PROJECT_HEADERS header/*.h) # headers for this project
file(GLOB PROJECT_SOURCES src/*.cpp) # sources for this project

# we need CMake to first find OpenCV
find_package( OpenCV REQUIRED )

# set chapters and sub chapters to be used for generating specific binaries later
# ---> (ref.B)

# chapters
set(CHAPTERS
    getting_started_opencv
    camera_calibration_3d_reconstruction
)

# sub chapters
# name of the sub chapter should be the file name of source cpp else your executable does not get build

# sub chapters for getting_started_opencv chapter
set(getting_started_opencv
    opencv_application_with_cmake_test
    video_read_offline_online_opencv
)

# sub chapters for camera_calibration_3d_reconstruction chapter
set(camera_calibration_3d_reconstruction
    camera_calibration
    undistortion_image_with_camera_params
)


# then create a project file per tutorial
# ---> (ref.B)
foreach(CHAPTER ${CHAPTERS})
    foreach(SUB_CHAPTER ${${CHAPTER}})
        set(NAME "${CHAPTER}_${SUB_CHAPTER}")
        include_directories( ${OpenCV_INCLUDE_DIRS} )
        add_executable(${NAME} src/${SUB_CHAPTER}.cpp ${PROJECT_HEADERS})
        target_link_libraries(${NAME} ${OpenCV_LIBS})
    endforeach(SUB_CHAPTER)
endforeach(CHAPTER)