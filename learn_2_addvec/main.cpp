#include <iostream>
#include <cstdlib>
#include <cassert>
#include <fstream>
#include <sstream>

// https://stackoverflow.com/questions/28500496/opencl-function-found-deprecated-by-visual-studio
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

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

cl_platform_id selectPlatform()
{
    cl_int err;
    cl_uint num_platform;

    // get number of platform
    err = clGetPlatformIDs(0, NULL, &num_platform);
    if (err != CL_SUCCESS || num_platform <= 0)
    {
        std::cout << "failed to find any OpenCL platforms." << std::endl;
        std::abort();
    }
    std::cout << "num_platform: " << num_platform << std::endl;

    // get all platform
    cl_platform_id *platform_array;
    platform_array = new cl_platform_id[num_platform];
    err = clGetPlatformIDs(num_platform, platform_array, NULL);

    // get platform info
    for (int i = 0; i < num_platform; i++)
    {
        size_t platform_name_length = 0;
        err = clGetPlatformInfo(platform_array[i], CL_PLATFORM_NAME, 0, 0, &platform_name_length);
        if (err != CL_SUCCESS)
        {
            std::cout << "get platform info failed" << std::endl;
        }
        char *platform_name = new char[platform_name_length];
        err = clGetPlatformInfo(platform_array[i], CL_PLATFORM_NAME, platform_name_length, platform_name, NULL);
        if (err != CL_SUCCESS)
        {
            std::cout << "get platform info failed" << std::endl;
        }
        std::cout << "platform[" << i << "]: " << platform_name << std::endl;
        delete[] platform_name;
    }

    std::cout << "select platform success." << std::endl;

    cl_platform_id platform;
    platform = platform_array[0];
    delete[] platform_array;
    return platform;
}

cl_device_id selectDevice(cl_platform_id platform)
{
    // get number of device
    cl_device_type device_type = CL_DEVICE_TYPE_ALL;
    cl_uint num_of_devices = -1;
    cl_int err = clGetDeviceIDs(platform,
                                device_type,
                                0,
                                0,
                                &num_of_devices);
    if (err != CL_SUCCESS)
    {
        std::cout << "get device id failed" << std::endl;
        std::abort();
    }
    if (num_of_devices <= 0)
    {
        std::cout << "no device" << std::endl;
        std::abort();
    }
    std::cout << "num device: " << num_of_devices << std::endl;

    // get all device
    cl_device_id *device_array = NULL;
    device_array = new cl_device_id[num_of_devices];
    err = clGetDeviceIDs(platform, device_type, num_of_devices, device_array, 0);
    if (err != CL_SUCCESS)
    {
        std::cout << "get device id failed" << std::endl;
        std::abort();
    }

    // get each device information
    for (int i = 0; i < num_of_devices; i++)
    {
        size_t device_name_length = 0;
        err = clGetDeviceInfo(device_array[i], CL_DEVICE_NAME, 0, 0, &device_name_length);
        if (err != CL_SUCCESS)
        {
            std::cout << "get device info failed" << std::endl;
            std::abort();
        }

        char *device_name = new char[device_name_length];
        err = clGetDeviceInfo(device_array[i], CL_DEVICE_NAME, device_name_length, device_name, 0);
        if (err != CL_SUCCESS)
        {
            std::cout << "get device info failed" << std::endl;
            std::abort();
        }
        std::cout << "device[" << i << "]: " << device_name << std::endl;
        delete[] device_name;
    }

    // choose the first device
    cl_device_id device = NULL;
    device = device_array[0];

    delete[] device_array;
    return device;
}

cl_context createContext(cl_platform_id platform)
{
    cl_device_type device_type = CL_DEVICE_TYPE_CPU;
    std::string device_type_str = getDeviceTypeString(device_type);
    cl_int err;

    cl_context context = NULL;
    cl_context_properties context_properties[] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)platform,
        0};

    context = clCreateContextFromType(context_properties, device_type, 0, 0, &err);
    if (err == CL_SUCCESS)
    {
        std::cout << "create " << device_type_str << " context success." << std::endl;
    }
    else
    {
        std::cout << "create " << device_type_str << " context failed." << std::endl;
        std::abort();
    }
    return context;
}

cl_command_queue createCommandQueue(cl_context context, cl_device_id device)
{
    cl_command_queue queue;
    cl_command_queue_properties queue_properties = 0;
    cl_int err = 0;
    queue = clCreateCommandQueue(context, device, queue_properties, &err);
    if (err != CL_SUCCESS)
    {
        std::cout << "create command queue failed" << std::endl;
        std::abort();
    }
    return queue;
}

cl_program createProgram(cl_context context, cl_device_id device, const char *file_name)
{
    cl_int err = 0;
    cl_program program = NULL;

    std::ifstream kernel_file(file_name, std::ios::in);
    if (!kernel_file.is_open())
    {
        std::cout << "failed to open kernel file" << file_name << std::endl;
        std::abort();
    }

    std::ostringstream oss;
    oss << kernel_file.rdbuf();
    std::string src_std_str = oss.str();
    const char *src_str = src_std_str.c_str();

    program = clCreateProgramWithSource(context, 1, (const char **)&src_str, NULL, NULL);

    if (program == NULL)
    {
        std::cout << "create program from source failed" << std::endl;
    }

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        char build_log[16384];
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(build_log), build_log, NULL);
        std::cout << "Error in kernel:" << std::endl;
        std::cout << build_log << std::endl;
        clReleaseProgram(program);
        std::abort();
    }
    return program;
}

int main()
{
    cl_platform_id platform = selectPlatform();
    cl_device_id device = selectDevice(platform);
    cl_context context = createContext(platform);
    cl_command_queue queue = createCommandQueue(context, device);

    cl_program program = createProgram(context, device, "vec_add.cl");
    cl_kernel kernel = clCreateKernel(program, "vec_add", NULL);

    // allocate memory space on host
    unsigned int n = 10;
    float *h_c = new float[n];
    float *h_a = new float[n];
    float *h_b = new float[n];

    // init memory space on host
    for (int i = 0; i < n; i++)
    {
        h_a[i] = i;
        h_b[i] = i * 2;
    }

    // allocate memory space on device
    cl_mem d_a;
    cl_mem d_b;
    cl_mem d_c;
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(float), NULL, NULL);
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(float), NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, n * sizeof(float), NULL, NULL);

    // copy data from host to device
    cl_int err;
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, n * sizeof(float), h_a, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, n * sizeof(float), h_b, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        std::cout << "copy data to device failed!" << std::endl;
        std::abort();
    }

    // set args of kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);
    if (err != CL_SUCCESS)
    {
        std::cout << "set args of kernel failed!" << std::endl;
        std::abort();
    }

    // set execute size
    size_t global_work_size[1] = {n};
    size_t local_work_size[1] = {1};

    // submit kernel to queue
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        std::cout << "submit to queue failed!" << std::endl;
        std::abort();
    }

    // copy h_c back to host
    err = clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, n * sizeof(float), h_c, 0, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        std::cout << "error reading h_c buffer." << std::endl;
        std::abort();
    }
    // h_c output
    for (int i = 0; i < n; i++)
    {
        std::cout << h_c[i] << std::endl;
    }

    free(h_a);
    free(h_b);
    free(h_c);
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
