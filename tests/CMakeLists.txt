cmake_minimum_required(VERSION 3.0)

# Set all source compilation to CPP in MSVC
set(SOURCE_FILES test-sokol.c
                 test-ini.c
                 test-fiber.c
                 test-cmdline.c
                 test-timer.c
                 test-threads.c)
if (MSVC)
    set_source_files_properties(${SOURCE_FILES} PROPERTIES LANGUAGE CXX)
endif()

add_executable(test-sokol WIN32 test-sokol.c)
target_link_libraries(test-sokol PRIVATE sx)

add_executable(test-ini test-ini.c)
target_link_libraries(test-ini PRIVATE sx)

add_executable(test-fiber test-fiber.c)
target_link_libraries(test-fiber PRIVATE sx)

add_executable(test-cmdline test-cmdline.c)
target_link_libraries(test-cmdline PRIVATE sx)

add_executable(test-timer test-timer.c)
target_link_libraries(test-timer PRIVATE sx)

add_executable(test-threads test-threads.c)
target_link_libraries(test-threads PRIVATE sx)
