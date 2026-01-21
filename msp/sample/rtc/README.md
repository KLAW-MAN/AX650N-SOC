sample_rtc /dev/rtc0 power_on
Test complete is printed every 15 seconds, and Test failed is displayed if it fails.
sample_rtc /dev/rtc0 shut_down
Print at a scheduled time of 15 seconds, the machine will shut down when the time is up
sample_rtc /dev/rtc0 soft_restart
Print at a scheduled time of 15 seconds, the machine will restart when the time is up
sample_rtc /dev/rtc0 hard_restart
Print at a scheduled time of 15 seconds, the machine will restart when the time is up
sample_rtc /dev/input/event0
Press the button for 1 second and release it to see the button report
