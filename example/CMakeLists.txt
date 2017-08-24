set(SRCS
  example.cpp
)

set(HDRS
  example.hpp
)

set(RESOURCES
    example.qrc
)

qt5_add_resources(RESOURCES_RCC ${RESOURCES})

include_directories(${CMAKE_SOURCE_DIR}/lib)

add_executable(example ${SRCS} ${HDRS} ${RESOURCES_RCC})
target_link_libraries(example Qt5::Widgets GdbCrashHandler)
