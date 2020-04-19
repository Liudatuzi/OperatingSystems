Problem2 Test your new system call

Write a simple C program which calls ptree

Print the entire process tree (in DFS order) using tabs to indent children with respect to their parents.

cd pro2/jni
ndk-build
emulator –avd OsPrj-516021910212 -kernel ~/Desktop/kernel/goldfish/arch/arm/boot/zImage –show-kernel
~/Desktop/homework/pro2/libs/armeabi# adb push ./psShowARM /data/misc
adb shell
cd /data/misc
./psShowARM
