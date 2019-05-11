#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <vector>

namespace hanz {
  class Filter {
    public:
      Filter();
      ~Filter() = default;
      cv::Mat process(cv::Mat& image);
      void calibrate(cv::Mat& image);
      void calculate_thresholds(cv::Mat& sample1, cv::Mat& sample2);
      void sample_skin_color(cv::Mat& image);
      void remove_face(cv::Mat& input_image);

      bool is_calibrated() const { return calibrated; }

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

      cv::CascadeClassifier face_finder;

      static constexpr int offset_low_threshold = 80;
      static constexpr int offset_high_threshold = 30;
      static constexpr int HIGH_THRESH = 255;
      static constexpr int LOW_THRESH = 0;
      static constexpr int RECT_SIZE = 20;
      const std::string classifier_path = "../../haarcascase_frontalface_alt.xml";
      const cv::Scalar RECT_COLOR = cv::Scalar(255, 0, 255);

      void morph_image(cv::Mat binary_image, int kernel_shape, cv::Point kernel_size);
  };
} // namespace hanz
