# Google Test
if (NOT TARGET gtest::gtest)
  set(gtest_force_shared_crt on)
  add_subdirectory(googletest)
endif()