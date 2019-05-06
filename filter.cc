#include "filter.h"
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace hanz {
  void Filter::calculate_thresholds(const cv::Mat& sample1, const cv::Mat& sample2) {

    auto HSV_min = cv::mean(sample1);
    auto HSV_max = cv::mean(sample2);

    h_low_threshold = HSV_min[0] - offset_low_threshold;
    h_high_threshold = HSV_max[0] + offset_high_threshold;
    s_low_threshold = HSV_min[1] - offset_low_threshold;
    s_high_threshold = HSV_max[1] + offset_high_threshold;
    v_low_threshold = HSV_min[2] - offset_low_threshold;
    v_high_threshold = HSV_max[2] + offset_high_threshold;
  }

  void Filter::calibrate(const cv::Mat& image) {
    cv::Mat HSV_input;
    cv::cvtColor(image, HSV_input, cv::COLOR_BGR2HSV);
    cv::Mat sample1 = cv::Mat(HSV_input, skin_color_rect_1);
    cv::Mat sample2 = cv::Mat(HSV_input, skin_color_rect_2);

    calculate_thresholds(sample1, sample2);
    calibrated = true;
  }

  void Filter::sample_skin_color(const cv::Mat& image) {
    int frame_width{image.size().width};
    int frame_height{image.size().height};

    skin_color_rect_1 = cv::Rect(frame_width / 5, frame_height / 2, RECT_SIZE, RECT_SIZE);
    skin_color_rect_2 = cv::Rect(frame_width / 5, frame_height / 3, RECT_SIZE, RECT_SIZE);

    // Add the rectangle to our images
    cv::rectangle(image, skin_color_rect_1, RECT_COLOR);
    cv::rectangle(image, skin_color_rect_2, RECT_COLOR);
  }

  void Filter::process(const cv::Mat& image) {
    cv::Mat skin_mask;
    if (!calibrated) {
      throw std::runtime_error("Must calibrate before processing an image!");
    }

    auto low_HSV = cv::Scalar(h_low_threshold, s_low_threshold, v_low_threshold);
    auto high_HSV = cv::Scalar(h_high_threshold, s_high_threshold, v_high_threshold);
    cv::inRange(image, low_HSV, high_HSV, skin_mask);
  }

  void Filter::morph_image(cv::Mat binary_image, int kernel_shape, cv::Point kernel_size) {
    const auto structuring_element = cv::getStructuringElement(kernel_shape, kernel_size);
    cv::morphologyEx(binary_image, binary_image, cv::MORPH_OPEN, structuring_element);
  }
} // namespace hanz
