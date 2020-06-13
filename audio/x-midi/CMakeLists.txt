cmake_minimum_required(VERSION 3.16)
project(format_commons_audio_x_midi)

set(CMAKE_CXX_STANDARD 17)

add_library(format_commons_audio_x_midi INTERFACE)
target_include_directories(format_commons_audio_x_midi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(format_commons_audio_x_midi_test test/main.cpp)
target_link_libraries(format_commons_audio_x_midi_test PUBLIC format_commons_audio_x_midi)

add_executable(format_x_midi_log main.cpp)
target_link_libraries(format_x_midi_log PUBLIC format_commons_audio_x_midi)

add_executable(format_x_midi_ref ref_impl.cpp)
target_link_libraries(format_x_midi_ref PUBLIC format_commons_audio_x_midi)

install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/format-commons
        DESTINATION include)

