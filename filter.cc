#include "filter.h"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace hanz {
  Filter::Filter() {
    if (!face_finder.load(classifier_path)) {
      throw std::runtime_error("Failed to load classifier model");
    }
  }

  void Filter::calculate_thresholds(cv::Mat& sample1, cv::Mat& sample2) {
    auto HSV_min = cv::mean(sample1);
    auto HSV_max = cv::mean(sample2);

    h_low_threshold = HSV_min[0] - offset_low_threshold;
    h_high_threshold = HSV_max[0] + offset_high_threshold;
    s_low_threshold = HSV_min[1] - offset_low_threshold;
    s_high_threshold = HSV_max[1] + offset_high_threshold;
    v_low_threshold = HSV_min[2] - offset_low_threshold;
    v_high_threshold = HSV_max[2] + offset_high_threshold;
  }

  void Filter::calibrate(cv::Mat& image) {
    cv::Mat HSV_input;
    cv::cvtColor(image, HSV_input, cv::COLOR_BGR2HSV);
    cv::Mat sample1 = cv::Mat(HSV_input, skin_color_rect_1);
    cv::Mat sample2 = cv::Mat(HSV_input, skin_color_rect_2);

    calculate_thresholds(sample1, sample2);
    calibrated = true;
  }

  void Filter::sample_skin_color(cv::Mat& image) {
    int frame_width{image.size().width};
    int frame_height{image.size().height};

    skin_color_rect_1 = cv::Rect(frame_width / 5, frame_height / 2, RECT_SIZE, RECT_SIZE);
    skin_color_rect_2 = cv::Rect(frame_width / 5, frame_height / 3, RECT_SIZE, RECT_SIZE);

    // Add the rectangle to our images
    cv::rectangle(image, skin_color_rect_1, RECT_COLOR);
    cv::rectangle(image, skin_color_rect_2, RECT_COLOR);
  }

  cv::Mat Filter::process(cv::Mat& image) {
    cv::Mat skin_mask;
    if (!calibrated) {
      throw std::runtime_error("Must calibrate before processing an image!");
    }

    auto low_HSV = cv::Scalar(h_low_threshold, s_low_threshold, v_low_threshold);
    auto high_HSV = cv::Scalar(h_high_threshold, s_high_threshold, v_high_threshold);
    cv::inRange(image, low_HSV, high_HSV, skin_mask);

    morph_image(skin_mask, cv::MORPH_ELLIPSE, {3, 3});
    cv::dilate(skin_mask, skin_mask, cv::Mat(), cv::Point(-1, -1), 3);

    return skin_mask;
  }

  void Filter::morph_image(cv::Mat binary_image, int kernel_shape, cv::Point kernel_size) {
    const auto structuring_element = cv::getStructuringElement(kernel_shape, kernel_size);
    cv::morphologyEx(binary_image, binary_image, cv::MORPH_OPEN, structuring_element);
  }

  void Filter::remove_face(cv::Mat& input_image, cv::Mat& output_image) {
    std::vector<cv::Rect> faces;
    cv::Mat gray_frame;

    cv::cvtColor(input_image, gray_frame, cv::COLOR_BGR2GRAY);
    face_finder.detectMultiScale(
        gray_frame, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(120, 120));
    std::cout << "Found: " << faces.size() << " faces." << std::endl;

    for (size_t i = 0; i < faces.size(); ++i) {
      // Add rectangles in each detected face location
      cv::rectangle(
          input_image,
          cv::Point(faces[i].x, faces[i].y),
          cv::Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height * 2),
          cv::Scalar(0, 0, 0),
          -1);
    }
  }
} // namespace hanz
