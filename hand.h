#pragma once

#include <algorithm>
#include <opencv2/opencv.hpp>

namespace hanz {
  class Hand {
    public:
      void mouse_click();
      void mouse_release();

      std::pair<cv::Point, double>
      palm_circle_from_points(cv::Point p1, cv::Point p2, cv::Point p3);

      int read_gesture(cv::RotatedRect palm_position);
  };
} // namespace hanz
