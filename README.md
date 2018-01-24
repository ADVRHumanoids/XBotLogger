# XBotLogger
Package providing a simple, RT safe, logging utility. It supports both console logging and logging of data to a .mat file.

# Installation
1) `sudo apt-get install libmatio-dev`
2) standard cmake build flow

# Using *XBotLogger* in external CMake project
1) `find_package(XBotLogger REQUIRED)`
2) `include_directories(${XBotLogger_INCLUDE_DIRS})`
3) `target_link_libraries(TargetName ${XBotLogger_LIBRARIES})`
