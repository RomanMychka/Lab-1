#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include "DHT.h"

#define DHTPIN 25
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

rcl_publisher_t publisher_celsius;
rcl_publisher_t publisher_fahrenheit;
std_msgs__msg__Float32 msg_celsius;
std_msgs__msg__Float32 msg_fahrenheit;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    delay(100);
  }
}


void timer_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    float t_c = dht.readTemperature();
    
    if (isnan(t_c)) {
      return; 
    }
    
    float t_f = (t_c * 9.0 / 5.0) + 32.0;

    msg_celsius.data = t_c;
    msg_fahrenheit.data = t_f;

    RCSOFTCHECK(rcl_publish(&publisher_celsius, &msg_celsius, NULL));
    RCSOFTCHECK(rcl_publish(&publisher_fahrenheit, &msg_fahrenheit, NULL));
  }
}

void setup() {
  set_microros_transports();
  dht.begin();

  allocator = rcl_get_default_allocator();
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  RCCHECK(rclc_node_init_default(&node, "stud_mychka_esp32", "", &support));

  RCCHECK(rclc_publisher_init_default(
    &publisher_celsius,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "/stud_mychka/temperature/celsius"));

  RCCHECK(rclc_publisher_init_default(
    &publisher_fahrenheit,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "/stud_mychka/temperature/fahrenheit"));

  RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(1000), timer_callback));

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void loop() {
  delay(10);
  RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
}