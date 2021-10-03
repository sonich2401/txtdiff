#pragma once

//This to disable assert statements so that we do not waste prcession power

#ifndef DEBUG
	#define assert(expression) ((void)0)
#else
	#include <assert.h>
#endif