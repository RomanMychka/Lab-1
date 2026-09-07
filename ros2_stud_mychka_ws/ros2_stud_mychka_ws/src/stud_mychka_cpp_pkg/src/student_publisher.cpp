#include "stud_mychka_cpp_pkg/student_publisher.hpp"
#include <chrono>

using namespace std::chrono_literals;

StudentPublisher::StudentPublisher() : Node("stud_mychka_publisher"), counter(1) {

    publisher = this->create_publisher<std_msgs::msg::String>("/stud_mychka/message", 10);
    timer = this->create_wall_timer(
        500ms, std::bind(&StudentPublisher::timer_callback, this));
}

void StudentPublisher::timer_callback() {
    auto message = std_msgs::msg::String();
    message.data = "Mychka Message #" + std::to_string(counter++);
    RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
    publisher->publish(message);
}