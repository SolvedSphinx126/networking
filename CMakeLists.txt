cmake_minimum_required(VERSION 3.0)
project(Networking)

# Add the client executable
add_executable(client client.c)

# Add the server executable
add_executable(server server.c)

# Specify the output directory for the executables
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
