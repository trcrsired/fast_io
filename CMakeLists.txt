cmake_minimum_required(VERSION 3.15)

project(fast_io C CXX)

set(CMAKE_CXX_STANDARD 20)

include(GNUInstallDirs)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include TYPE INCLUDE)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/man DESTINATION ${CMAKE_INSTALL_MANDIR}/man3 PATTERN "*.3")

# fast_io_i18n — the shared locale-blob cache library.
# src/locale/lcblob.cc owns the process-wide locale cache (thread-local +
# global swiss maps over never-unloaded mmaps). BUILD_SHARED_LIBS selects
# shared vs static.
option(BUILD_I18N_LIB "build the fast_io_i18n locale library" ON)
if(BUILD_I18N_LIB)
	add_library(fast_io_i18n src/locale/lcblob.cc)
	target_include_directories(fast_io_i18n PUBLIC ${CMAKE_SOURCE_DIR}/include)
	set_target_properties(fast_io_i18n PROPERTIES CXX_STANDARD 26 CXX_STANDARD_REQUIRED ON)
	target_compile_definitions(fast_io_i18n PRIVATE FAST_IO_I18N_BUILDING)
	if(BUILD_SHARED_LIBS)
		target_compile_definitions(fast_io_i18n PUBLIC FAST_IO_I18N_SHARED)
	endif()
	if(I18N_INSTALL_DIR)
	else()
		set(I18N_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
	endif()
	install(TARGETS fast_io_i18n
		LIBRARY DESTINATION ${I18N_INSTALL_DIR}
		ARCHIVE DESTINATION ${I18N_INSTALL_DIR}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

option(ENABLE_TESTS "Enable tests" OFF)

if(${ENABLE_TESTS})
	include(CTest)
	include_directories(include)
	set(CMAKE_CXX_STANDARD 23)
	set(CMAKE_CXX_STANDARD_EXTENSION OFF)
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
	endif()
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows" OR CMAKE_SYSTEM_NAME STREQUAL "Cygwin" OR CMAKE_SYSTEM_NAME STREQUAL "Msys")
		link_libraries(ntdll)
		link_libraries(psapi)
	endif()

	option(TESTS_PREBUILD "prebuild cmake files for test" OFF)

	if(${TESTS_PREBUILD})
		add_executable(tests_prebuild tests/0000.tests_prebuild/gentests.cc)
		add_custom_target(run_tests_prebuild
			COMMAND tests_prebuild
			DEPENDS tests_prebuild
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	else()
		include(${CMAKE_CURRENT_SOURCE_DIR}/benchmark/CMakeLists.txt)
		include(${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt)
		include(${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt)
	endif()
endif()
