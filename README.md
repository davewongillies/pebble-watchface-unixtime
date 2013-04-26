pebble-watchface-unixtime
=========================

![Unix Time](https://github.com/davewongillies/pebble-watchface-unixtime/raw/master/unixtime.jpg)

A Pebble watch face that displays seconds since the Unix epoch

Due to the (current) lack of time zone awareness, it must be compiled
with the correct GMT offset. Set the correct offset by changing the
TIME_ZONE_OFFSET constant.

Notes:
* Thanks to orviwan for set_container_image and the methodologies I borrowed.
* The seconds on my watch are not perfectly accurate.
* The code is what it is. If you have an improvement then submit the code for it.
* The menu icon is left as an exercise for the reader.

Original source: [WhyIsThisOpen](http://forums.getpebble.com/profile/153/WhyIsThisOpen): [Watch Face: Unix Time - Pebble Forums](http://forums.getpebble.com/discussion/4324/watch-face-unix-time)
