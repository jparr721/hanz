#include "camera.h"
#include "hand.h"
#include "filter.h"
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <opencv2/opencv.hpp>

namespace hanz {
  int Camera::capture() {
    Filter f;
    Hand h;

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
      throw std::runtime_error("Failed to open webcam");
    }

    const int frame_width{cap.get(cv::CAP_PROP_FRAME_WIDTH)};
    const int frame_height{cap.get(cv::CAP_PROP_FRAME_HEIGHT)};
    const cv::Size box_size(frame_width, frame_height);
    int background_frame = 500;
    std::vector<std::pair<cv::Point, double>> palm_centers;

    cv::Mat frame;
    cv::Mat back;
    cv::Mat fore;
    cv::Ptr<cv::BackgroundSubtractorMOG2> background;
    background = cv::createBackgroundSubtractorMOG2();

    background->setNMixtures(3);
    background->setDetectShadows(false);
    cv::namedWindow("Frame");
    cv::namedWindow("Background");

    bool calibrated = false;
    for (int i = 0;;++i) {
      std::vector<std::vector<cv::Point>> contours;
      cap >> frame;
      if (frame.empty()) {
        std::cerr << "Failed to read image from stream, exiting" << std::endl;
        break;
      }

      // Begin calculating the background frames
      if (background_frame > 0) {
        background->apply(frame, fore);
        --background_frame;
      } else {
        background->apply(frame, fore, 0);
      }

      // Extract the background frame
      background->getBackgroundImage(back);

      // Erode and dilate the foreground
      cv::erode(fore, fore, cv::Mat());
      cv::dilate(fore, fore, cv::Mat());

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
      f.remove_face(frame);

      // Now we track the hands
      cv::findContours(fore, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
      h.detect_hand(frame, palm_centers, contours);

      cv::imshow("Camera Feed", frame);
      cv::imshow("Background", back);
      if (cv::waitKey(10) == 27) break;
    }

    cap.release();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
  }
} // namespace hanz
