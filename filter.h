#pragma once

#include <vector>
#include <opencv2/opencv.hpp>

namespace hanz {
  class Filter {
    public:
      void process(const cv::Mat& image);
      void calibrate(const cv::Mat& image);
      void calculate_thresholds(const cv::Mat& sample1, const cv::Mat& sample2);
      void sample_skin_color(const cv::Mat& image);

      bool is_calibrated() { return calibrated; }

    private:
      // Hue Threshold
      int h_low_threshold{0};
      int h_high_threshold{0};

      // Saturation Threshold
      int s_low_threshold{0};
      int s_high_threshold{0};

      // Value Channel Threshold
      int v_low_threshold{0};
      int v_high_threshold{0};

      bool calibrated = false;

      cv::Rect skin_color_rect_1;
      cv::Rect skin_color_rect_2;

      static constexpr int offset_low_threshold{80};
      static constexpr int offset_high_threshold{30};
      static constexpr int HIGH_THRESH = 255;
      static constexpr int LOW_THRESH = 0;
      static constexpr int RECT_SIZE = 20;
      const cv::Scalar RECT_COLOR = cv::Scalar(255, 0, 255);

      void morph_image(cv::Mat binary_image, int kernel_shape, cv::Point kernel_size);
  };
} // namespace hanz
