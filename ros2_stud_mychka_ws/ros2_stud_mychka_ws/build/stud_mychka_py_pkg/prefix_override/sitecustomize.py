import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/ros/ros2_stud_mychka_ws/install/stud_mychka_py_pkg'
