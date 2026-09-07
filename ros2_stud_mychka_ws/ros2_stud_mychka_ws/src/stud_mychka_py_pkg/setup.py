from setuptools import find_packages, setup

package_name = 'stud_mychka_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ros',
    maintainer_email='ros@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'stud_mychka_subscriber = stud_mychka_py_pkg.student_subscriber:main',
            'stud_mychka_temperature_monitor = stud_mychka_py_pkg.temp_subscriber:main',
        ],
    },
)
