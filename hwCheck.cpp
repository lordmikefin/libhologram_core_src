
/*!
* \file hwCheck.cpp
* \brief checks available hardware (GPUs)
* \author Ville Pitkäkangas, ville.pitkakangas@oulu.fi
*  \par      Copyright:
*  Copyright (c) 2012-2013, University of Oulu.
*  All rights reserved.
*/

#include "hwCheck.h"

// TODO: move this to header file. Does some other header already contain this <-> #include <cufft.h>
#include "cuda_runtime.h" // needed for cudaGetDeviceCount() and others

#include <cstdio> // for printf()

#include <cstring>	// for memcpy() and memset()


/*
Notice about the CUDA hardware name lengths in this file and
HardwareChecker.cu:
The length is taken from the file "device_types.h" in the CUDA SDK.
It has not been defined there, so the value 256 must be used.
The lines where the length is mentioned in "device_types.h"
(copyright 1993-2012 NVIDIA corporation, all rights reserved):
struct __device_builtin__ cudaDeviceProp
{
char   name[256];
...
};
*/

int checkHardware(int &mode, int &devices, int **deviceIDs, char **deviceNames, bool force)
{
	//cuInit(0);

	int status = 0;

	int firstDevice = -1;
	int devicesFound = 0;
	//int potentialUVADevices = 0, actualUVADevices = 0;

	// Find GPU devices.
	int gpuDeviceCount = 0;
	int deviceCount, device;

	struct cudaDeviceProp properties;
	cudaError_t cudaResultCode = cudaGetDeviceCount(&deviceCount);

	if (cudaResultCode != cudaSuccess)
		deviceCount = 0;

	/* machines with no GPUs can still report one emulation device */
	for (device = 0; device < deviceCount; ++device) {
		cudaGetDeviceProperties(&properties, device);
		if (properties.major != 9999) /* 9999 means emulation only */
		{
			if (firstDevice == -1)
				firstDevice = device;
			++gpuDeviceCount;
		}
	}

#ifdef _DEBUG
	printf("%d GPU CUDA device(s) found\n", gpuDeviceCount);
#endif


	int *device_ids = NULL;

	if (gpuDeviceCount > 0)
		device_ids = new int[gpuDeviceCount];

	char *device_names = NULL;

	if (gpuDeviceCount > 0)
	{
		// 256 is defined as maximum length for device name
		// (see file driver_types.h in CUDA SDK)
		device_names = new char[256 * gpuDeviceCount];
		/*device_names = */memset(device_names, 0, 256 * gpuDeviceCount);
	}

	if (gpuDeviceCount == 0)
	{

#ifdef _DEBUG
		printf("***WARNING! No CUDA-capable devices found!***\n");
		system("pause");
#endif

		mode = 0;	// no devices


		// TODO: create our own static Error states -> new header file
		status = -2;	// Error
	}
	else
	{

		/*
		Check device capabilities (most importantly P2P).
		*/
		for (device = 0; device < deviceCount; ++device) {
#ifdef _DEBUG
			if (device == 0)
				printf("\n--------------------\n");
#endif
			cudaGetDeviceProperties(&properties, device);
			if (properties.major != 9999)
			{
				device_ids[devicesFound++] = device;
				// 256 is defined as maximum length for device name
				// (see file driver_types.h in CUDA SDK)
				strcpy(&device_names[256 * (devicesFound - 1)], properties.name);

				// TODO: VS recomends using strcpy_s()
				//strcpy_s(&device_names[256 * (devicesFound - 1)], properties.name);
			}
#ifdef _DEBUG
			printf("Device %d properties:\n", device);
			printf("Name: %s\n", properties.name);
			printf("CUDA Capability Version: %d.%d\n", properties.major, properties.minor);
			printf("TCC? %s\n", properties.tccDriver ? "Yes" : "No");
#endif

		}
	}

	return status;
}