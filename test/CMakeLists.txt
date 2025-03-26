set(TARGET_LIST
  test_bitboard
)

foreach(TARGET_NAME ${TARGET_LIST})
  add_executable(${TARGET_NAME} ${TARGET_NAME}.cpp)
  
  target_link_libraries(${TARGET_NAME} jcl)
  target_link_libraries(${TARGET_NAME} GTest::gtest_main)
endforeach()