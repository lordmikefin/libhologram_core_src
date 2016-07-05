#pragma once

/*********  Original code from ->
* \file HardwareChecker.h
* \brief checks available hardware (GPUs)
* \author Ville Pitkäkangas, ville.pitkakangas@oulu.fi
*  \par      Copyright:
*  Copyright (c) 2012-2013, University of Oulu.
*  All rights reserved.
*/

#include <cuda.h>

int checkHardware(int &mode, int &devices, int **deviceIDs, char **deviceNames, bool force);
