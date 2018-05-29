[![Build Status](https://travis-ci.org/ESROCOS/tools-stream_aligner.svg?branch=master)](https://travis-ci.org/ESROCOS/tools-stream_aligner) [![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

# ESROCOS Stream Aligner

Stream Aligner and Time stamp utility for the ESROCOS framework. This utility is
only useful in the dataflow of component-based systems, this is in port-driven
functions of [TASTE](https://taste.tuxfamily.org). The processing is done
asynchronously: components process data as it arrives. Since different
processing chains have different computation times, it means that our carefully
timestamped data will most probably arrive out-of-order at the components that
need it.

Let’s see, as an example, the following processing chain:

![Rock StreamAligner Figure](https://www.rock-robotics.org/documentation/data_processing/stream_aligner_chain.png)

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

* one callback is registered for each data stream.
* data is pushed as it arrives in the corresponding streams. The leading
assumption is that, on each stream, the timestamps are monotonous (they don’t go
back in time).
* the respective callbacks get called when it is time to process the relevant
samples. This is done only when the stream aligner determined that no sample
from other streams can arrive with an earlier timestamp then the one that is
being passed to a callback.

## Period, latency and stream aligner timeout
Let’s look at the example of the laser filter again:

![Rock Latency Figure](https://www.rock-robotics.org/documentation/data_processing/stream_aligner_period_latency_timeout_1.png)

If, as this diagram shows, the laser filter starts processing as soon as the
lidar sample arrives, it means that the stream aligner would, in this case, have
determined that it is impossible that a motion tracker sample arrives in between
the first motion tracker and first lidar samples.

With no additional information, however, the stream aligner would have to wait
that another sample arrives on the motion tracker before processing the first
laser sample:

![Rock Latency Delay Figure](https://www.rock-robotics.org/documentation/data_processing/stream_aligner_period_latency_timeout_2.png)


Which builds up latency...To avoid this, the stream aligner allows to set a
period parameter on each stream. This period parameter (which is also called
lookahead in event-based systems) is the time after a sample in which there is a
guarantee that no other sample arrives. It is called period in the stream
aligner as it is the value of the input periods, for periodic inputs such as
common robotic sensors. When visualizing the period, the above example looks
therefore like:

![Rock Timeout Figure](https://www.rock-robotics.org/documentation/data_processing/stream_aligner_period_latency_timeout.png)


---
**NOTE**

Because processing based on the stream aligner is based on the fact that samples
are passed in-order, the stream aligner must drop samples that arrive “in the
past”, i.e. that arrive with an earlier timestamp that the last sample “played”
by the stream aligner. So, it is better to give a period that is lower than the
actual sensor period so that the aligner does not drop samples unnecessarily.

---

In order to not wait forever for samples that will never arrive (lost samples),
the stream aligner also allows to set a timeout parameter, which is the highest
latency that the aligner would allow to build up. When the latency induced by
the stream aligner is higher than this value, the aligner starts playing queued
samples regardless of the fact that, in principle, some samples should arrive on
the other streams. If these samples do arrive anyway, they will therefore get
dropped. This parameter is therefore a trade-off between the maximum latency
that the processing chain can accept and how exact the result needs to be.

## Example of Usage
There is an example of usage in the test folder
[here](test/test_example_usage.cpp).
The example has three streams, one for `string`, one for `double` and a third
one for `integer` type.  The Stream Aligner processes the samples and executes
them in the right order according to the given information. The example is
graphically depicted in the following:

![Example Usage Figure](doc/figures/stream_aligner_example.png)

Right configuration values are given in order for the Stream Aligner to
interpret the timing. The first number to consider is the `NUMBER_OF_STREAMS`
because the Stream Aligner is a static and fully templated class. The following
values are `WINDOW_SIZE` and samples period (e.g., `S1_PERIOD`) in order to size
the circular buffer. Those values define how many samples you want to store in
the buffer. Once these values are defined, the priority (optional) can be set.
Normally, the stream with highest frequency has the highest priority. The code
with the configuration values is here:

```cpp
/** Number of streams or provided interfaces (config value) **/
#define NUMBER_OF_STREAMS 3

/** Windows size in seconds **/
#define WINDOW_SIZE 5

/** Stream periods (configuration value) **/
#define S1_PERIOD 2.0
#define S2_PERIOD 0.5
#define S3_PERIOD 1.0

/** Stream aligner timeout (config value)
* this defines the highest larency **/
#define TIMEOUT S1_PERIOD //the lowest period

/** Buffer size as a computation of timeout and period scaled witha factor
 * typical two in order to store two cycles of timeout**/
#define BUFFER_SIZE_S1 CEILING(WINDOW_SIZE, S1_PERIOD)
#define BUFFER_SIZE_S2 CEILING(WINDOW_SIZE, S2_PERIOD)
#define BUFFER_SIZE_S3 CEILING(WINDOW_SIZE, S3_PERIOD)

/** When samples have the same time, the priority defines which one to choose at
 * first **/
#define HIGH_PRIORITY 1
#define MEDIUM_PRIORITY 2
#define LOW_PRIORITY 3
```


