import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32

class TemperatureMonitor(Node):
    def __init__(self):
        super().__init__('stud_mychka_temperature_monitor')
        self.last_celsius = None
        self.last_fahrenheit = None

        self.subscription_celsius = self.create_subscription(
            Float32, '/stud_mychka/temperature/celsius', self.celsius_callback, 10
        )
        self.subscription_fahrenheit = self.create_subscription(
            Float32, '/stud_mychka/temperature/fahrenheit', self.fahrenheit_callback, 10
        )

    def celsius_callback(self, msg):
        self.last_celsius = msg.data
        self.print_temperature()

    def fahrenheit_callback(self, msg):
        self.last_fahrenheit = msg.data
        self.print_temperature()



    def print_temperature(self):
        if self.last_celsius is not None and self.last_fahrenheit is not None:
            print(f"Temperature:\n  Celsius:    {self.last_celsius:.1f} °C\n  Fahrenheit: {self.last_fahrenheit:.1f} °F\n" + "-"*20)

def main(args=None):
    rclpy.init(args=args)
    monitor = TemperatureMonitor()
    rclpy.spin(monitor)
    monitor.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()