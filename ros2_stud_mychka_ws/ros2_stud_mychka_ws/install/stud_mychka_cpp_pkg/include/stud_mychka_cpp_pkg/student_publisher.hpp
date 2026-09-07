#ifndef STUDENT_PUBLISHER_HPP
#define STUDENT_PUBLISHER_HPP

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"


class StudentPublisher : public rclcpp::Node {
public:
    StudentPublisher();

private:
    void timer_callback();
    

    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher;
    rclcpp::TimerBase::SharedPtr timer;
    size_t counter;
};

#endif