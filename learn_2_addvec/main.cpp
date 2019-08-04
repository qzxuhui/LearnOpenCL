#include <iostream>
#include <CL/cl.h>
#include <cstdlib>

std::string getDeviceTypeString(const cl_device_type &device_type)
{
    std::string device_type_str;
    if (device_type == CL_DEVICE_TYPE_GPU)
    {
        device_type_str = "GPU";
    }
    if (device_type == CL_DEVICE_TYPE_CPU)
    {
        device_type_str = "CPU";
    }
    return device_type_str;
}

cl_context createContext()
{
    cl_device_type device_type = CL_DEVICE_TYPE_CPU;
    std::string device_type_str = getDeviceTypeString(device_type);
    cl_int err;
    cl_uint num_platform;
    cl_platform_id first_platform_id;
    cl_context context = NULL;

    // choose the first platform
    int num_entity = 1;
    err = clGetPlatformIDs(num_entity, &first_platform_id, &num_platform);
    if (err != CL_SUCCESS || num_platform <= 0)
    {
        std::cout << "failed to find any OpenCL platforms." << std::endl;
        std::abort();
    }
    std::cout << "num_platform: " << num_platform << std::endl;

    cl_context_properties context_properties[] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)first_platform_id,
        0};

    context = clCreateContextFromType(context_properties, device_type, NULL, NULL, &err);
    if (err == CL_SUCCESS)
    {
        std::cout << "create " << device_type_str << " context success." << std::endl;
    }
    else
    {
        std::cout << "create " << device_type_str << " context failed." << std::endl;
        std::abort();
    }
}

cl_command_queue createCommandQueue(cl_context context, cl_device_id *device)
{
    cl_int err;
    cl_device_id *devices;
    cl_command_queue command_queue = NULL;
    size_t device_buffer_size = -1;

    // get buffer size for store device id information
    err = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &device_buffer_size);
    // if (err != CL_SUCCESS)
    // {
    //     std::cout << "get context info failed" << std::endl;
    //     std::abort();
    // }
    // if (device_buffer_size <= 0)
    // {
    //     std::cout << " no device availabe" << std::endl;
    //     std::abort();
    // }
    std::cout << "device_buffer_size" << std::endl;
    // allocate memory for buffer
    // devices = new cl_device_id[device_buffer_size / sizeof(cl_device_id)];
    // err = clGetContextInfo(context, CL_CONTEXT_DEVICES, device_buffer_size, devices, NULL);
    // if (err != CL_SUCCESS)
    // {
    //     std::cout << "failed to get device IDs" << std::endl;
    //     std::abort();
    // }

    // // choose the first device
    // command_queue = clCreateCommandQueue(context, device[0], 0, NULL);

    // if (command_queue == NULL)
    // {
    //     std::cout << "create command queue failed" << std::endl;
    //     std::abort();
    // }

    // std::cout << "create command queue success" << std::endl;
    // *device = devices[0];
    // delete[] devices;
    return command_queue;
}

int main()
{
    // create context at platform
    cl_context context = NULL;
    context = createContext();

    // create a command queue on first device and attached to context
    cl_device_id device = NULL;
    cl_command_queue command_queue = NULL;
    command_queue = createCommandQueue(context, &device);
    return 0;
}
