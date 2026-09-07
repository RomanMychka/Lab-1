#include "rclcpp/rclcpp.hpp"
#include "stud_mychka_cpp_pkg/student_publisher.hpp"
#include <memory>


int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StudentPublisher>());
    rclcpp::shutdown();
    return 0;
}