
ADD_LIBRARY(osmo-core SHARED Mission.cpp Command.cpp)
TARGET_LINK_LIBRARIES(osmo-core ${OpenCV_LIBS})
