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
      void detect_hand(
          cv::Mat& frame,
          std::vector<std::pair<cv::Point, double>>& centers,
          std::vector<std::vector<cv::Point>>& contours);

      std::pair<cv::Point, double> circle_from_points(cv::Point p1, cv::Point p2, cv::Point p3);

      void read_gesture(int fingers, int x, int y);

      double euclidean_distance(cv::Point x, cv::Point y);
  };
} // namespace hanz
