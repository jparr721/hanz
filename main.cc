#include <chrono>
#include <stdexcept>
#include <opencv2/opencv.hpp>

int capture() {
  cv::VideoCapture cap(0);
  if (!cap.isOpened()) {
    throw std::runtime_error("Failed to open webcam");
  }

  const int frame_width{cap.get(cv::CAP_PROP_FRAME_WIDTH)};
  const int frame_height{cap.get(cv::CAP_PROP_FRAME_HEIGHT)};
  const cv::Size box_size(frame_width, frame_height);

  for (;;) {
    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) {
      break;
    }

    cv::imshow("Camera Feed", frame);
    if (cv::waitKey(10) == 27) break;
  }

  cap.release();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
  return capture();
}
