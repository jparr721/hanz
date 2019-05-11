#pragma once

#include <algorithm>
#include <opencv2/opencv.hpp>
#include <utility>

namespace hanz {
  class Hand {
    public:
      void mouse_click();
      void mouse_release();
      void move_mouse(int x, int y);

      std::pair<cv::Point, double>
      circle_from_points(cv::Point p1, cv::Point p2, cv::Point p3);

      int read_gesture(cv::RotatedRect palm_position);

      double euclidean_distance(cv::Point x, cv::Point y);
  };
} // namespace hanz
