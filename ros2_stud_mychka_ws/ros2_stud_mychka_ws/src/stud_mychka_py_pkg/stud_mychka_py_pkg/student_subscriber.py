import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class StudentSubscriber(Node):
    def __init__(self):
        super().__init__('stud_mychka_subscriber')
        
        self.subscription = self.create_subscription(
            String,
            '/stud_mychka/message',
            self.listener_callback,
            10
        )

    def listener_callback(self, msg):
        self.get_logger().info(f'I heard: "{msg.data}"')

def main(args=None):
    rclpy.init(args=args)
    
    subscriber = StudentSubscriber()
    rclpy.spin(subscriber)
    
    subscriber.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()