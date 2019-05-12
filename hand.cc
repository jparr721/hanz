#include "hand.h"
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace hanz {
  double Hand::euclidean_distance(cv::Point x, cv::Point y) {
    return (x.x - y.x) * (x.x - y.x) + (x.y - y.y) * (x.y - y.y);
  }

  void Hand::read_gesture(int fingers, int x, int y) {
    move_mouse(x, y);
    if (fingers < 4) {
      mouse_click();
    } else {
      mouse_release();
    }
  }

  std::pair<cv::Point, double> Hand::circle_from_points(cv::Point p1, cv::Point p2, cv::Point p3) {
    double offset = std::pow(p2.x, 2) + std::pow(p2.y, 2);
    double bc = (std::pow(p1.x, 2) + std::pow(p1.y, 2) - offset) / 2.0;
    double cd = (offset - std::pow(p3.x, 2) - std::pow(p3.y, 2)) / 2.0;
    double det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p2.y);
    double TOL = 0.0000001;

    if (std::abs(det) < TOL) {
      std::cout << "Points too close..." << std::endl;
      return std::make_pair(cv::Point(0, 0), 0);
    }

    double idet = 1/det;
    double centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
    double centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
    double radius = std::sqrt(std::pow(p2.x - centerx, 2) + std::pow(p2.y - centery, 2));

    return std::make_pair(cv::Point(centerx, centery), radius);
  }

  void Hand::mouse_click() {
    int button = Button1;
    Display *display = XOpenDisplay(nullptr);
    XEvent event;

    if (display == nullptr) {
      throw std::runtime_error("Failed to attach to display");
    }

    event.type = ButtonPress;
    event.xbutton.button = button;
    event.xbutton.same_screen = true;
    XQueryPointer(
        display,
        RootWindow(display, DefaultScreen(display)),
        &event.xbutton.root,
        &event.xbutton.window,
        &event.xbutton.x_root,
        &event.xbutton.y_root,
        &event.xbutton.x,
        &event.xbutton.y,
        &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;
    while (event.xbutton.subwindow) {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer(
          display,
          event.xbutton.window,
          &event.xbutton.root,
          &event.xbutton.subwindow,
          &event.xbutton.x_root,
          &event.xbutton.y_root,
          &event.xbutton.x,
          &event.xbutton.y,
          &event.xbutton.state);
    }

    if (XSendEvent(display, PointerWindow, true, 0xfff, &event) == 0) {
      std::cerr << "Error relasing event" << std::endl;
    }

    XFlush(display);
    XCloseDisplay(display);
  }

  void Hand::mouse_release() {
    int button = Button1;
    Display *display = XOpenDisplay(nullptr);
    XEvent event;

    if (display == nullptr) {
      throw std::runtime_error("Failed to attach to display");
    }

    event.xbutton.button = button;
    event.xbutton.same_screen = true;
    XQueryPointer(
        display,
        RootWindow(display, DefaultScreen(display)),
        &event.xbutton.root,
        &event.xbutton.window,
        &event.xbutton.x_root,
        &event.xbutton.y_root,
        &event.xbutton.x,
        &event.xbutton.y,
        &event.xbutton.state);

    event.xbutton.subwindow = event.xbutton.window;
    while (event.xbutton.subwindow) {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer(
          display,
          event.xbutton.window,
          &event.xbutton.root,
          &event.xbutton.subwindow,
          &event.xbutton.x_root,
          &event.xbutton.y_root,
          &event.xbutton.x,
          &event.xbutton.y,
          &event.xbutton.state);
    }
    event.type = ButtonRelease;
    event.xbutton.state = 0x100;

    if (XSendEvent(display, PointerWindow, true, 0xfff, &event) == 0) {
      std::cerr << "Error relasing event" << std::endl;
    }

    XFlush(display);
    XCloseDisplay(display);
  }

  void Hand::move_mouse(int x, int y) {
    Display *display = XOpenDisplay(nullptr);
    Window root = DefaultRootWindow(display);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);
    XCloseDisplay(display);
  }

  void Hand::detect_hand(
      cv::Mat& frame,
      std::vector<std::pair<cv::Point, double>>& centers,
      std::vector<std::vector<cv::Point>>& contours) {
    // Iterate through the contours to find the fingers
    for (auto i = 0u; i < contours.size(); ++i) {
      // Ignore small contours
      if (cv::contourArea(contours[i]) >= 5000) {
        // Draw the contour on the image
        std::vector<std::vector<cv::Point>> tcontours;
        tcontours.push_back(contours[i]);
        cv::drawContours(frame, tcontours, -1, cv::Scalar(0, 0, 255), 2);

        // Locate convex hull and count fingers
        std::vector<std::vector<cv::Point>> hulls(1);
        std::vector<std::vector<int>> hulls_I(1);

        cv::convexHull(cv::Mat(tcontours[0]), hulls[0], false);
        cv::convexHull(cv::Mat(tcontours[0]), hulls_I[0], false);

        // After isolating the hand, draw the contours
        cv::drawContours(frame, hulls, -1, cv::Scalar(0, 255, 0), 2);

        // Find the palm of the hand via a minimum width rectangle
        cv::RotatedRect rect = cv::minAreaRect(cv::Mat(tcontours[0]));

        // Find convex defects in palm
        std::vector<cv::Vec4i> defects;

        if (hulls_I[0].size() > 0) {
          cv::Point2f rect_points[4];
          rect.points(rect_points);

          for (int j = 0; j < 4; ++j) {
            cv::line(frame, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(255, 0, 0), 1, 8);
          }

          cv::Point rough_palm_center;
          cv::convexityDefects(tcontours[0], hulls_I[0], defects);

          if (defects.size() >= 3) {
            std::vector<cv::Point> palm_points;
            for (int j = 0; j < defects.size(); ++j) {
              int startidx = defects[j][0];
              int endidx = defects[j][1];
              int faridx = defects[j][2];
              cv::Point pt_start(tcontours[0][startidx]);
              cv::Point pt_end(tcontours[0][endidx]);
              cv::Point pt_far(tcontours[0][faridx]);

              // Sum hull and defect points and compute the average
              rough_palm_center += pt_far + pt_start + pt_end;
              palm_points.push_back(pt_far);
              palm_points.push_back(pt_start);
              palm_points.push_back(pt_end);
            }

            // Get initial center by esitmating the average of all defect points
            // Then choose 3 closest points and make the circle from them
            rough_palm_center.x /= defects.size() * 3;
            rough_palm_center.y /= defects.size() * 3;
            cv::Point closest_pt = palm_points[0];
            std::vector<std::pair<double, int>> distvec;
            for (int i = 0; i < palm_points.size(); ++i) {
              distvec.push_back(std::make_pair(euclidean_distance(rough_palm_center, palm_points[i]), i));
            }

            std::sort(distvec.begin(), distvec.end());

            // Keep choosing until we find what we need
            std::pair<cv::Point, double> solution_circle;
            for (int i = 0; i + 2 < distvec.size(); ++i) {
              cv::Point p1 = palm_points[distvec[i + 0].second];
              cv::Point p2 = palm_points[distvec[i + 1].second];
              cv::Point p3 = palm_points[distvec[i + 2].second];
              solution_circle = circle_from_points(p1, p2, p3);
              if (solution_circle.second != 0) {
                break;
              }
            }

            // Find average centers to stabilize palm tracker
            centers.push_back(solution_circle);
            if (centers.size() > 10) {
              centers.erase(centers.begin());
            }

            cv::Point palm_center;
            double radius = 0;

            for (int i = 0; i < centers.size(); ++i) {
              palm_center += centers[i].first;
              radius += centers[i].second;
            }

            palm_center.x /= centers.size();
            palm_center.y /= centers.size();
            radius /= centers.size();

            // Draw the palm center and the cirtcle into the frame
            cv::circle(frame, palm_center, 5, cv::Scalar(144, 144, 255), 3);
            cv::circle(frame, palm_center, radius, cv::Scalar(144, 144, 255), 2);

            int fingers = 0;
            for (int j = 0; j < defects.size(); ++j) {
              int startidx = defects[j][0];
              int endidx = defects[j][1];
              int faridx = defects[j][2];
              cv::Point pt_start(tcontours[0][startidx]);
              cv::Point pt_end(tcontours[0][endidx]);
              cv::Point pt_far(tcontours[0][faridx]);
              // X o-----------------------o Y
              double X_dist = std::sqrt(euclidean_distance(palm_center, pt_far));
              double Y_dist = std::sqrt(euclidean_distance(palm_center, pt_start));
              double length = std::sqrt(euclidean_distance(pt_far, pt_start));

              double ret_length = std::sqrt(euclidean_distance(pt_end, pt_far));

              if (length <= 3 * radius &&
                  Y_dist >= 0.4 * radius &&
                  length >= 10 && ret_length >= 10 &&
                  std::max(length, ret_length)/std::min(length, ret_length) >= 0.8) {
                if (std::min(X_dist, Y_dist) / std::max(X_dist, Y_dist) <= 0.8) {
                  if ((X_dist >= 0.1 * radius &&
                       X_dist <= 1.3 * radius &&
                       X_dist < Y_dist) ||
                      (Y_dist >= 0.1 * radius &&
                       Y_dist <= 1.3 * radius &&
                       X_dist > Y_dist)) {
                    cv::line(frame, pt_end, pt_far, cv::Scalar(0, 255, 0), 1);
                    ++fingers;
                  }
                }
              }
            }
            fingers = std::min(5, fingers);
            std::cout << "fingers: " << fingers << std::endl;
            read_gesture(fingers, palm_center.x, palm_center.y);
          }
        }
      }
    }
  }
} // namespace hanz
