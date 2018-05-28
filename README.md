[![Build Status](https://travis-ci.org/ESROCOS/tools-stream_aligner.svg?branch=master)](https://travis-ci.org/ESROCOS/tools-stream_aligner) [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

## ESROCOS Stream Aligner

Stream Aligner and Time stamp utility for the ESROCOS framework. This utility is
only useful in the dataflow of component-based systems, this is in port-driven
functions in ESROCOS. The processing is done asynchronously: components process
data as it arrives. Since different processing chains have different computation
times, it means that our carefully timestamped data will most probably arrive
out-of-order at the components that need it.

Let’s see, as an example, the following processing chain:

![Rock StreamAligner Picture](https://www.rock-robotics.org/documentation/data_processing/stream_aligner_chain.png)

In this chain, sensors are fused through two paths. One path processes lidar
data to remove outliers and body parts (laser filter). In another path, a stereo
camera rig is used to generate a separate point clouds. The two informations are
then merged to form a pointcloud that is represented in a frame that is centered
on the body, but aligned to the world (i.e. a tabletop would look horizontal
even if the robot’s body is on an incline). To achieve this, a motion tracker
gives the pose of the body in the reference frame.





