#include "lame_aux_internal.h"

extern "C" void* reallocbuf(void* buf, int floatbuf, size_t length, size_t newlength)
{
	if (floatbuf)
	{
		float* newbuf = new float[newlength];
		memmove(newbuf, buf, sizeof(float) * length);
		delete[] buf;
		return (void*)newbuf;
	}
	else
	{
		double* newbuf = new double[newlength];
		memmove(newbuf, buf, sizeof(double) * length);
		delete[] buf;
		return (void*)newbuf;
	}
}