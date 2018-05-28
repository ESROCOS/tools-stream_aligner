[![Build Status](https://travis-ci.org/ESROCOS/tools-stream_aligner.svg?branch=master)](https://travis-ci.org/ESROCOS/tools-stream_aligner) [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

# ESROCOS Stream Aligner

Stream Aligner and Time stamp utility for the ESROCOS framework. This utility is
only useful in the dataflow of component-based systems, this is in port-driven
functions of [TASTE] (https://taste.tuxfamily.org). The processing is done
asynchronously: components process data as it arrives. Since different
processing chains have different computation times, it means that our carefully
timestamped data will most probably arrive out-of-order at the components that
need it.

Let’s see, as an example, the following processing chain:

![Rock StreamAligner Picture](https://www.rock-robotics.org/documentation/data_processing/stream_aligner_chain.png)

In this chain, sensors are fused through two paths. One path processes lidar
data to remove outliers and body parts (laser filter). In another path, a stereo
camera rig is used to generate a separate point clouds. The two informations are
then merged to form a pointcloud that is represented in a frame that is centered
on the body, but aligned to the world (i.e. a tabletop would look horizontal
even if the robot’s body is on an incline). To achieve this, a motion tracker
gives the pose of the body in the reference frame.

## Stream Aligner Principles
The Stream Aligner ensures that data is processed in order by (1) queueing
timestamped data, and (2) calling registered callbacks in the order of the
timestamps. The general principle is therefore that:

..* one callback is registered for each data stream
..* data is pushed as it arrives in the corresponding streams. The leading
assumption is that, on each stream, the timestamps are monotonous (they don’t go
back in time).
..* the respective callbacks get called when it is time to process the relevant
samples. This is done only when the stream aligner determined that no sample
from other streams can arrive with an earlier timestamp then the one that is
being passed to a callback.

## Period, latency and stream aligner timeout
Let’s look at the example of the laser filter again:

If, as this diagram shows, the laser filter starts processing as soon as the
lidar sample arrives, it means that the stream aligner would, in this case, have
determined that it is impossible that a motion tracker sample arrives in between
the first motion tracker and first lidar samples.

With no additional information, however, the stream aligner would have to wait
that another sample arrives on the motion tracker before processing the first
laser sample:



## Example of Usage

There is an example of Usage with 



