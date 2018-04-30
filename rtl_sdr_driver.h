#ifndef _RTLSDRDRIVER_H_GUARD
    #define _RTLSDRDRIVER_H_GUARD

    #include <rtl-sdr.h>

	typedef void (*SampleHandler)(IQPair sample);

	typedef struct RTLSDRInfoS
	{
		SampleHandler samplehandler;
		rtlsdr_dev_t *dev;
		uint32_t acci, accq;
		uint16_t count;
	} RTLSDRInfo_t;

	int hw_init(RTLSDRInfo_t *target, int freq, int samplerate, int gain, int ppm, SampleHandler sh);
	void *driver_thread_fn(void *context);
	void rtl_sdr_cancel(RTLSDRInfo_t target);
	
#endif