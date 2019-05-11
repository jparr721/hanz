#include "hand.h"
#include <cmath>
#include <memory>
#include <stdexcept>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace hanz {
  double Hand::euclidean_distance(cv::Point x, cv::Point y) {
    return std::pow((x.x - y.x), 2) + std::pow((x.y - y.y), 2);
  }

  int read_gesture(cv::RotatedRect palm_position) {
    return 1;
  }

  void Hand::mouse_click() {
    int button = Button1;
    auto display = std::make_unique(XOpenDisplay(nullptr));
    /* Display *display = XOpenDisplay(nullptr); */
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
    auto display = std::make_unique(XOpenDisplay(nullptr));
    /* Display *display = XOpenDisplay(nullptr); */
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

  void move_mouse(int x, int y) {
    auto display = std::make_unique(XOpenDisplay(0));
    Window root = DefaultRootWindow(display);
    XWarpPointer(display, None, root, 0, 0, 0, 0, x, y);
    XFlush(display);
    XCloseDisplay(display);
  }
} // namespace hanz
