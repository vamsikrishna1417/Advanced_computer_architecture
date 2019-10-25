#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <string>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

#define MAX_SOURCE_SIZE (0x100000)

// C version of matrix multiplcation. Use this function for result validation and execution time comaprison
void matrix_mul_sequence (int *A_mat,
                          int *B_mat,
                          int *C_mat)
{
	for (int j=0; j<SIZE; j++) {
		for (int i=0; i<SIZE; i++)
			for (int k=0; k<SIZE; k++)
				C_mat[j*SIZE + i] += A_mat[j*SIZE + k] * B_mat[k*SIZE + i];
	}
}


void get_time(cl_device_id device_id) 
{
	// A, B are input matrix, C is the output matrix for OpenCL, C_seq is the output matrix for reference implementation.
    int *A = new int[SIZE*SIZE];
    int *B = new int[SIZE*SIZE];
    int *C = new int[SIZE*SIZE];
    int *C_seq = new int[SIZE*SIZE];

    //Initialize matrix
    for(int j=0; j<SIZE; j++) {
		for(int i=0; i<SIZE; i++) {
			A[j*SIZE + i] = 1;
			B[j*SIZE + i] = i+1;
			C[j*SIZE + i] = 0;
			C_seq[j*SIZE + i] = 0;
		}
    }

	std::chrono::high_resolution_clock::time_point t1, t2;
	t1 = std::chrono::high_resolution_clock::now();
    matrix_mul_sequence(A, B, C_seq);
    t2 = std::chrono::high_resolution_clock::now();
    std::cout << "Reference C matrix multiplication: "
		<< (float)(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count())/1000000
		<< " sec"
		<< std::endl;

    // Load the kernel source code into the array source_str
	FILE *fp;
    char *source_str;
    size_t source_size;
    cl_int ret;


    fp = fopen("matrix_mul.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

  // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);

    // Create a command queue with the capability of performance profiling for target device
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);

    // Create memory buffers on the device for each matrix
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE*SIZE*sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE*SIZE*sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, SIZE*SIZE*sizeof(int), NULL, &ret);

    // Copy the matrix A, B and C to each device memory counterpart
    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), B, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), C, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    // Build and compile the OpenCL kernel program
    std::string build_option = "-DTILE_SIZE=" + std::to_string(TILE_SIZE);
    ret = clBuildProgram(program, 1, &device_id, build_option.c_str(), NULL, NULL);
    if (ret == CL_BUILD_PROGRAM_FAILURE) { // If compile failed, print the error message
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		char *log = (char *) malloc(log_size);

		// Get the log and print it
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
		printf("%s\n", log);
	}

    // Create the OpenCL kernel
    cl_kernel kernel;
#ifdef ENABLE_TILE_KERNEL
    kernel = clCreateKernel(program, "matrix_mul_tile", &ret);
#else
	kernel = clCreateKernel(program, "matrix_mul", &ret);
#endif

    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    int dimention = 2; // In this example, We will use 2 dimention index
    size_t global_item_size[] = {SIZE, SIZE, 1};
    size_t local_item_size[] = {TILE_SIZE, TILE_SIZE, 1};

	cl_event perf_event;
	cl_ulong start, end;

	// Execute the OpenCL kernel
    ret = clEnqueueNDRangeKernel(command_queue, kernel, dimention, NULL, global_item_size, local_item_size, 0, NULL, &perf_event);
    // Capture performance event from target device. In this case the event is to retrive the execution time.
    ret = clWaitForEvents(1, &perf_event);
    ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_START, sizeof(start), &start, NULL);
    ret = clGetEventProfilingInfo(perf_event, CL_PROFILING_COMMAND_END, sizeof(end), &end, NULL);
	std::cout << "OpenCL matrix multiplication: " << (float)(end - start)/1000000000 << " sec" << std::endl;

    // Read the memory buffer C from the device into the local variable C
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, SIZE*SIZE*sizeof(int), C, 0, NULL, NULL);

	// Make sure all the command in the command queue has been executed
    ret = clFinish(command_queue);

    bool validate = true;
    for(int j=0; j<SIZE; j++) {
		for(int i=0; i<SIZE; i++) {
			if (C[j*SIZE + i] != C_seq[j*SIZE + i])
				validate = false;
		}
	}

	if (validate == false)
		std::cout << "The results are mismatched !!" << std::endl;

    // Clean up
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

	//std::cout << "Press Enter to finish..." << std::endl;
	//getchar();
}


int main(void)
{
    struct
    {
        cl_device_type type;
        const char* name;
        cl_uint count;
    }
    devices[] =
    {
        { CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", 0 },
        { CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", 0 }
    };

    const int NUM_OF_DEVICE_TYPES = sizeof(devices)/sizeof(devices[0]);

    // Get platform and device information
    cl_uint ret_num_platforms = 0;

    cl_int ret = clGetPlatformIDs(0, 0, &ret_num_platforms);
    std::cout << "Number of available platforms: " << ret_num_platforms << std::endl;

    cl_platform_id* platform_id = new cl_platform_id[ret_num_platforms];
    // get IDs for all platforms:
    ret = clGetPlatformIDs(ret_num_platforms, platform_id, 0);

	for(cl_uint i = 0; i < ret_num_platforms; i++) 
	{
		cl_platform_id platform = platform_id[i];
		for(int j = 0; j < NUM_OF_DEVICE_TYPES; ++j)
    	{
        	ret = clGetDeviceIDs(
            platform,
            devices[j].type,
            0,
            0,
            &devices[j].count
            );

        if(CL_DEVICE_NOT_FOUND == ret)
        {
            // that's OK to fall here, because not all types of devices, which
            // you query for may be available for a particular system
            devices[i].count = 0;
            ret = CL_SUCCESS;
        }

        //std::cout<< "    " << devices[i].name << ": " << devices[i].count << std::endl;


        cl_uint cur_num_of_devices = devices[j].count;

        if(cur_num_of_devices == 0)
        {
            // there is no devices of this type; move to the next type
            continue;
        }

        // Retrieve a list of device IDs with type selected by type_index
        cl_device_id* device_id = new cl_device_id[cur_num_of_devices];
        ret = clGetDeviceIDs(
            platform,
            devices[j].type,
            cur_num_of_devices,
            device_id,
            0
            );

        // Iterate over all devices of the current device type
        for(cl_uint device_index = 0;
            device_index < cur_num_of_devices;
        ++device_index
            )
        {
            std::cout
                << "\n"
                << devices[j].name
                << "[" << device_index << "]\n";

            cl_device_id device = device_id[device_index];
            get_time(device);
    	}
		}
	}

    return 0;
}
