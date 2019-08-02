// compile: g++ main.cpp -lOpenCL
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>
#include <CL/cl.h>

#define CAPSBASIC_CHECK_ERRORS(ERR)             \
    if (ERR != CL_SUCCESS)                      \
    {                                           \
        std::cerr                               \
            << "OpenCL error with code " << ERR \
            << " happened in file " << __FILE__ \
            << " at line " << __LINE__          \
            << ". Exiting...\n";                \
        exit(1);                                \
    }

int main()
{

    const char *required_platform_subname = "Intel";

    cl_int err = CL_SUCCESS;

    cl_uint num_of_platforms = 0;
    err = clGetPlatformIDs(0, 0, &num_of_platforms);
    CAPSBASIC_CHECK_ERRORS(err);
    std::cout << "Number of available platforms: " << num_of_platforms << std::endl;

    cl_platform_id *platforms = new cl_platform_id[num_of_platforms];
    // 得到所有平台的ID
    err = clGetPlatformIDs(num_of_platforms, platforms, 0);
    CAPSBASIC_CHECK_ERRORS(err);

    //列出所有平台
    cl_uint selected_platform_index = num_of_platforms;

    std::cout << "Platform names:\n";

    for (cl_uint i = 0; i < num_of_platforms; ++i)
    {
        size_t platform_name_length = 0;
        err = clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            0,
            0,
            &platform_name_length);
        CAPSBASIC_CHECK_ERRORS(err);

        // 调用两次，第一次是得到名称的长度
        char *platform_name = new char[platform_name_length];
        err = clGetPlatformInfo(
            platforms[i],
            CL_PLATFORM_NAME,
            platform_name_length,
            platform_name,
            0);
        CAPSBASIC_CHECK_ERRORS(err);

        std::cout << "    [" << i << "] " << platform_name;

        if (
            strstr(platform_name, required_platform_subname) &&
            selected_platform_index == num_of_platforms // have not selected yet
        )
        {
            std::cout << " [Selected]";
            selected_platform_index = i;
        }

        std::cout << std::endl;
        delete[] platform_name;
    }
    delete[] platforms;
    return 0;
}
