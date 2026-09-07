# Самостійна робота 1. ROS 2: workspace, ООП-nodes та ESP32 через micro-ROS

**Виконав:** Мичка Роман, група РТ-3

---

## 1. Опис складових системи

- **Схема системи:**
    
    !Screenshot 2026-09-07 161714.png
    
- **stud_mychka_cpp_pkg:** С++ пакет, що містить ноду publisher для генерації та відправки пронумерованих повідомлень.
- **stud_mychka_py_pkg:** Python пакет, що містить ноду subscriber для отримання повідомлень, та ноду-монітор температури.
- **ESP32 Firmware:**
    
    ```cpp
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
    ```
    

## 2. Списки Nodes та Topics

**Nodes:**

- /stud_mychka_publisher
- /stud_mychka_subscriber

**Topics:**

- /stud_mychka/message

## 3. Таблиця класів

| Клас | Батьківський | Поля | Методи |
| --- | --- | --- | --- |
| StudentPublisher | rclcpp::Node | publisher, timer, counter | timer_callback() |
| StudentSubscriber | Node | subscription | listener_callback() |
| TemperatureMonitor | Node | **last_celsius, last_fahrenheit** | **celsius_callback, fahrenheit_callback** |

## 4. Команди збірки та запуску[cite: 1]

- **Збірка:** `colcon build` (або `colcon build --packages-select <назва_пакета>`)
- **C++ Publisher:** `ros2 run stud_mychka_cpp_pkg stud_mychka_publisher`
- **Python Subscriber:** `ros2 run stud_mychka_py_pkg stud_mychka_subscriber`
- **micro-ROS Agent:** `sudo docker run -it --rm -v /dev:/dev -v /dev/shm:/dev/shm --privileged --net=host microros/micro-ros-agent:jazzy serial --dev /dev/ttyACM0 -v6`

---

## 5. Відповіді на пункти «У звіті» з кожного етапу

### Етап 1. Власний workspace

- **Вивід `printenv | grep ROS`:** *(встав сюди свій вивід)*
- **Вивід `echo $AMENT_PREFIX_PATH`:** *(встав сюди свій вивід)*
- **Опис каталогів:**
    - `src/`: містить вихідний код пакетів.
    - `build/`: містить проміжні файли, створені під час компіляції.
    - `install/`: містить готові виконувані файли та бібліотеки після збірки.
    - `log/`: містить файли журналів (логів) для кожного запуску `colcon build`.
- **Чому `install/setup.bash` підключається після системного:** Щоб власні пакети з робочого простору (overlay) накладалися поверх системних пакетів ROS 2 (underlay), що дозволяє системі бачити та пріоритезувати наш код.

### Етап 5. Дослідження системи через ROS 2 CLI[cite: 1]

1. `ros2 node list`: 
    
    ```cpp
    /stud_mychka_publisher
    /stud_mychka_subscriber
    ```
    
2. `ros2 topic list`: 
    
    ```cpp
    /parameter_events
    /rosout
    /stud_mychka/message
    ```
    
3. `ros2 node info /stud_mychka_publisher`: 
    
    ```cpp
    /stud_mychka_publisher
    Subscribers:
    /parameter_events: rcl_interfaces/msg/ParameterEvent
    Publishers:
    /parameter_events: rcl_interfaces/msg/ParameterEvent
    /rosout: rcl_interfaces/msg/Log
    /stud_mychka/message: std_msgs/msg/String
    Service Servers:
    /stud_mychka_publisher/describe_parameters: rcl_interfaces/srv/DescribeParameters
    /stud_mychka_publisher/get_parameter_types: rcl_interfaces/srv/GetParameterTypes
    /stud_mychka_publisher/get_parameters: rcl_interfaces/srv/GetParameters
    /stud_mychka_publisher/get_type_description: type_description_interfaces/srv/GetTypeDescription
    /stud_mychka_publisher/list_parameters: rcl_interfaces/srv/ListParameters
    /stud_mychka_publisher/set_parameters: rcl_interfaces/srv/SetParameters
    /stud_mychka_publisher/set_parameters_atomically: rcl_interfaces/srv/SetParametersAtomically
    Service Clients:
    ```
    
    Action Servers:
    
    Action Clients:
    
4. `ros2 topic info /stud_mychka/message`: 
    
    ```cpp
    Type: std_msgs/msg/String
    Publisher count: 1
    Subscription count: 1
    ```
    
5. `ros2 interface show std_msgs/msg/String`: 
    
    ```cpp
    This was originally provided as an example message.
    It is deprecated as of Foxy
    It is recommended to create your own semantically meaningful message.
    However if you would like to continue using this please use the equivalent in example_msgs.
    ```
    
6. `ros2 topic echo /stud_mychka/message`: 

    
    ```powershell
    data: 'Mychka Message #935'
    ---
    data: 'Mychka Message #936'
    ---
    data: 'Mychka Message #937'
    ---
    data: 'Mychka Message #938'
    ---
    data: 'Mychka Message #939'
    ---
    data: 'Mychka Message #940'
    ---
    data: 'Mychka Message #941'
    ---
    data: 'Mychka Message #942'
    ---
    data: 'Mychka Message #943'
    ---
    data: 'Mychka Message #944'
    ---
    data: 'Mychka Message #945'
    ---
    ```
    
7. `ros2 topic hz /stud_mychka/message`: 
    
    ```cpp
    average rate: 1.993
    min: 0.456s max: 0.548s std dev: 0.03754s window: 3
    average rate: 1.999
    min: 0.456s max: 0.548s std dev: 0.02687s window: 6
    average rate: 1.989
    min: 0.456s max: 0.548s std dev: 0.02763s window: 8
    average rate: 1.987
    min: 0.456s max: 0.548s std dev: 0.02590s window: 11
    average rate: 2.000
    min: 0.456s max: 0.548s std dev: 0.02472s window: 14
    ```
    

### Етап 6. Три Subscribers з одного класу

- **Чому повідомлення доходить до всіх і скільки разів публікується:** Повідомлення від publisher публікується лише один раз у DDS (Data Distribution Service). Інші три ноди просто зчитують одне і те ж повідомлення
- **Коли використовувати service замість topic:** Service використовується для обміну командами, нерегулярними даними, тоді як Service добре підходить для моніторингу пакетів

### Етап 7. Три Subscribers з одного класу

### Етап 7. rqt та rqt_graph

!image.png

- **Крок 1 (зупинка subscriber_3):** З графа зник сам вузол `stud_mychka_subscriber_3` та стрілка від топіка до нього. Топік `/stud_mychka/message` залишився, оскільки publisher продовжує його публікувати, а інші підписники продовжують його слухати.

!image.png

- **Крок 3 (зупинка publisher):** З графа зник вузол `stud_mychka_publisher` та стрілка від нього до топіка. Проте сам топік `/stud_mychka/message` та підписники залишилися на графі, оскільки підписники все ще запущені й очікують на повідомлення з цього топіка, хоч джерело даних і зникло.

!image.png

### Етап 6-12:

---

## 6. Апаратна частина (ESP32)[cite: 1]

- **Модель датчика:** DHT22
- **Живлення:** 3.3v
- **Інтерфейс:** Serial
- **Діапазон / Точність / Роздільна здатність:** -40 °C до +80 °C / ±0,5 °C / 0,1 °C
- **Підключення:**

!image.png

## 7. Графіки та скріншоти (rqt_graph)[cite: 1]

!image.png

## 8. Висновки

Що спрацювало не з першого разу і чому:

- Довелося повозитися з підключенням USB і VM
