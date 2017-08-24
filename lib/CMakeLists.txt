set(GDBCH_LIB_VERSION ${GDBCH_LIB_MAJOR}.${GDBCH_LIB_MINOR}.0)
SET(GDBCH_SO_VERSION ${GDBCH_LIB_MAJOR})

set(SRCS
  GdbCrashHandlerDialog.cpp
  GdbCrashHandler.cpp
)

set(HDRS
  GdbCrashHandlerDialog.hpp
  GdbCrashHandler.hpp
)

set(FORMS
    GdbCrashHandlerDialog.ui
)

qt5_wrap_ui(FORM_HEADERS ${FORMS})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QUAZIP_INCLUDE_DIR})

add_library(GdbCrashHandler SHARED ${SRCS} ${HDRS} ${FORM_HEADERS})
set_target_properties(GdbCrashHandler PROPERTIES
  VERSION ${GDBCH_LIB_VERSION}
  SOVERSION ${GDBCH_SO_VERSION}
)
target_link_libraries(GdbCrashHandler Qt5::Widgets Qt5::Network ${QUAZIP_LIBRARIES})

install(TARGETS GdbCrashHandler
    RUNTIME DESTINATION bin COMPONENT libraries
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT libraries
    LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT libraries
)
install(FILES GdbCrashHandler.hpp DESTINATION ${INCLUDE_INSTALL_DIR}/GdbCrashHandler/)
