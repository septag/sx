cmake_minimum_required(VERSION 3.0)

add_executable(test-ini test-ini.c)
target_link_libraries(test-ini PRIVATE sx)
set_target_properties(test-ini PROPERTIES FOLDER tests)

add_executable(test-cmdline test-cmdline.c)
target_link_libraries(test-cmdline PRIVATE sx)
set_target_properties(test-cmdline PROPERTIES FOLDER tests)

add_executable(test-timer test-timer.c)
target_link_libraries(test-timer PRIVATE sx)
set_target_properties(test-timer PROPERTIES FOLDER tests)

add_executable(test-virtualmem test-virtualmem.c)
target_link_libraries(test-virtualmem PRIVATE sx)
set_target_properties(test-virtualmem PROPERTIES FOLDER tests)

add_executable(test-hash test-hash.c)
target_link_libraries(test-hash PRIVATE sx)
set_target_properties(test-hash PROPERTIES FOLDER tests)

add_executable(test-jobs test-jobs.c)
target_link_libraries(test-jobs PRIVATE sx)
set_target_properties(test-jobs PROPERTIES FOLDER tests)

add_executable(test-bheap test-bheap.c)
target_link_libraries(test-bheap PRIVATE sx)
set_target_properties(test-bheap PROPERTIES FOLDER tests)

add_executable(test-tlsf test-tlsf.c)
target_link_libraries(test-tlsf PRIVATE sx)
set_target_properties(test-tlsf PROPERTIES FOLDER tests)

# skip test-fiber, test-threads in emscripten
if (NOT EMSCRIPTEN)
    add_executable(test-fiber test-fiber.c)
    target_link_libraries(test-fiber PRIVATE sx)
    set_target_properties(test-fiber PROPERTIES FOLDER tests)
    
    add_executable(test-threads test-threads.c)
    target_link_libraries(test-threads PRIVATE sx)
    set_target_properties(test-threads PROPERTIES FOLDER tests)
    #set_target_properties(test-threads PROPERTIES LINKER_FLAGS "-s USE_PTHREADS=1 -s PTHREAD_POOL_SIZE=2")
endif()

