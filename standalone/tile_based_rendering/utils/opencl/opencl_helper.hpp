/**
 * @file opencl_helper.hpp
 * @brief contains the helper functions for opencl that pretty much make life 
 *        easy
 * @author Ryan Ben
*/
#ifndef opencl_helper.hpp
#define opencl_helper.hpp
#include <CL/opencl.hpp>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cassert>
/**
 * @brief reads a text file and returns a string
 * @param file_location
 * @return string file contents 
 */
std::string readFile(std::string file_location){
    std::fstream fileStream(file_location,std::ios_base::in);
    std::stringstream stream;
    stream << fileStream.rdbuf();    
    return stream.str();
}

/**
 * @brief Creates a cl::Program from an input file location
 * @param string program source
 * @return cl::Program
 * 
*/
cl::Program createClProgram(std::string source_file){
    // Log Event 
    std::cout << "Loading shader from:  " << source_file << std::endl;

    // Variables
    std::vector<cl::Device> all_devices;
    std::vector<cl::Platform> all_platforms;

    cl::Platform default_platform;
    cl::Device default_device;

    // Get Platforms 
    cl::Platform::get(&all_platforms);
    assert(all_platforms.size() > 0);
    default_platform = all_platforms.front(); // Get a platform

    // Select the device
    default_platform.getDevices(CL_DEVICE_TYPE_GPU,&all_devices);
    assert(all_devices.size() > 0);
    default_device = all_devices.front();

    // For Debug 
    std::cout << "Host: 11th Gen Intel® Core™ i3-1115G4 × 4" << std::endl;
    std::cout << "Using GPU Device: " << default_device.getInfo<CL_DEVICE_NAME>() << std::endl;
    std::cout << "GPU Compute Units: " << default_device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << "\n" << std::endl;

    // Context 
    cl::Context context({default_device});

    // Open the source file
    std::string source_text = readFile(source_file); 

    // Program source code that goes into a program
    cl::Program::Sources sources;
    sources.push_back({ source_text.c_str(),source_text.length() });

    // Create the program
    cl::Program program(context, sources);

    // Try to build the program
    if (program.build() != CL_SUCCESS){
        std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device);
        std::cout << "Failed to build the source file " << source_file << std::endl;
        std::cout << log << std::endl;
        exit(1);
    }

    // Shaders Now Loaded 
    std::cout << "Shaders Loaded " << std::endl;
 
    return program;
}

#endif