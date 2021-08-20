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

add_definitions(-DGDBCRASHHANDLER_LIBRARY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(GdbCrashHandler SHARED ${SRCS} ${HDRS} ${FORM_HEADERS})
set_target_properties(GdbCrashHandler PROPERTIES
  VERSION ${GDBCH_LIB_VERSION}
  SOVERSION ${GDBCH_SO_VERSION}
)
if(WIN32)
    set_target_properties(GdbCrashHandler PROPERTIES SUFFIX "-${GDBCH_SO_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(WIN32)
target_link_libraries(GdbCrashHandler Qt5::Widgets Qt5::Network QuaZip::QuaZip)

install(TARGETS GdbCrashHandler
    RUNTIME DESTINATION bin COMPONENT libraries
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT libraries
    LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT libraries
)
install(FILES GdbCrashHandler.hpp DESTINATION ${INCLUDE_INSTALL_DIR}/GdbCrashHandler/)

file(GLOB TS locale/*.ts)
qt5_add_translation(QM ${TS})
add_custom_target(translations ALL DEPENDS ${QM})
install(FILES ${QM} DESTINATION share/GdbCrashHandler)

configure_file(GdbCrashHandler.pc.in GdbCrashHandler.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GdbCrashHandler.pc DESTINATION ${LIB_INSTALL_DIR}/pkgconfig)
