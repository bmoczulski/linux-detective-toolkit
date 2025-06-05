add_executable(001-syscalls 001-syscalls.c)

add_library(read-another-file SHARED 002-read-another-file.c)
target_link_libraries(read-another-file PRIVATE dl)

add_library(back-to-the-future SHARED 003-back-to-the-future.c)
target_link_libraries(back-to-the-future PRIVATE dl)

add_library(ipv6-only SHARED 004-ipv6-only.c)
target_link_libraries(ipv6-only PRIVATE dl)

add_executable(005-fork 005-fork.c)

add_executable(006-abba 006-abba.c)
add_executable(007-multi-thread-deadlock 007-multi-thread-deadlock.c)
