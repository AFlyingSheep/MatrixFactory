enable_testing()

add_executable(testMatrixReader testMatrixReader.cc)
target_include_directories(testMatrixReader PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(testMatrixReader GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(testMatrixReader)
