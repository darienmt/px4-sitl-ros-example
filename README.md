# PX4 SITL ROS Example
This repo contains examples of [PX4 SITL with Gazebo](https://dev.px4.io/en/simulation/gazebo.html).

## Cloning the repo

The [PX4 firmware](https://github.com/PX4) is installed as [git submodule](https://blog.github.com/2016-02-01-working-with-submodules/). The examples here assume to use that code without modifications, and it is separated from the `src/` directory. To clone this repo, you need to use the following command instead of the regular `git clone`:

```
git clone --recursive <project_url>
```

## Making the PX4.

Before running anything, you need to make the PX4 Firmware, follow the instruction [here](https://dev.px4.io/en/setup/dev_env.html) executed in the directory `/px4-firmware`. The last step of those instructions could be:

```
make posix_sitl_default gazebo
```
And then ctl+c to turn off the execution when successfully you see the drone on Gazebo.

More information about other targets could be found [here](https://dev.px4.io/en/simulation/gazebo.html)

## Dependencies

This code depends on a few packages most of them are already installed on a full-desktop-ROS installation and the compilation of the PX4 firmware. In addition to that, you need to install [MAVROS](https://dev.px4.io/en/ros/mavros_installation.html).

## Compiling the examples

To compile the examples use catkin:

```
catkin_make
```

## Running the examples

Each example will have a `launch-*.sh` script that will set up the environment variables as described [here](https://dev.px4.io/en/simulation/ros_interface.html) to run the example package and PX4. Those common variables are expressed in [launch-common.sh](./launch-common.sh).

# Examples description

- **Running PX4 + Gazebo**: [`launch-px4-sitl.sh`](./launch-px4-sitl.sh)

- **Offboard example**: [`launch-offb.sh`](./launch-offb.sh). This is the [MAVROS Offboard control example](https://dev.px4.io/en/ros/mavros_offboard.html) wrapped on a [ROS package `offb`](./src/offb). The script uses the launch file [`offb.launch`](./src/offb/launch/offb.launch) including the [`mavros_posix_stil.launch`](https://github.com/PX4/Firmware/blob/master/launch/mavros_posix_sitl.launch).
