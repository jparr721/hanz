#include "camera.h"
#include "filter.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>

namespace hanz {
  int Camera::capture() {
    Filter f;

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
      throw std::runtime_error("Failed to open webcam");
    }

    const int frame_width{cap.get(cv::CAP_PROP_FRAME_WIDTH)};
    const int frame_height{cap.get(cv::CAP_PROP_FRAME_HEIGHT)};
    const cv::Size box_size(frame_width, frame_height);
    cv::Mat frame;
    cv::Mat output_frame;

    bool calibrated = false;
    for (int i = 0;;++i) {
      cap >> frame;
      output_frame = frame.clone();

      if (frame.empty()) {
        std::cerr << "Failed to read image from stream, exiting" << std::endl;
        break;
      }

      // We want to first calibrate our system
      if (i < 1) {
        f.calibrate(frame);
        calibrated = f.is_calibrated();
        if (!calibrated) {
          throw std::runtime_error("Failed to calibrate, exiting");
        }
        std::cout << "Calibration complete" << std::endl;
        continue;
      }

      // Remove the face from the frame
      f.remove_face(frame, output_frame);

      cv::imshow("Camera Feed", frame);
      if (cv::waitKey(10) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
  }
} // namespace hanz
