// rtl_rfm: FSK1200 Decoder
// R. Suchocki

#include "rtl_rfm.h"

#include "hardware.h"
#include "squelch.h"
#include "fm.h"
#include "fsk.h"
#include "rfm_protocol.h"

bool quiet = false;
bool debugplot = false;
int freq = 869412500;
int gain = 50;
int ppm = 43;
int baudrate = 4800;




void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx) {
	for (uint32_t k = 0; k < len; k+=(DOWNSAMPLE*2)) {
		uint16_t countI = 0;
		uint16_t countQ = 0;

		for (uint32_t j = k; j < k+(DOWNSAMPLE*2); j+=2) {
			countI += ((uint8_t) buf[j]);
			countQ += ((uint8_t) buf[j+1]);
		}

		int8_t avgI = countI / DOWNSAMPLE - 128; // divide and convert to signed
		int8_t avgQ = countQ / DOWNSAMPLE - 128;

		int32_t fm_magnitude_squared = avgI * avgI + avgQ * avgQ;

		if (squelch(fm_magnitude_squared)) {
			int16_t fm = fm_demod(avgI, avgQ);
			int8_t bit = fsk_decode(fm, sqrt(fm_magnitude_squared));
			if (bit >= 0) {
				rfm_decode(bit);
			}
		}
	}
}




void intHandler(int dummy) {
    run = 0;
    rtlsdr_cancel_async(dev);
}

int main (int argc, char **argv) {

	int c;

	char *helpmsg = "RTL_RFM, (C) Ryan Suchocki\n"
			"\nUsage: rtl_rfm [-hsqd] [-f freq] [-g gain] [-p error] \n\n"
			"Option flags:\n"
			"  -h    Show this message\n"
			"  -q    Quiet. Only output good messages\n"
			"  -d    Show Debug Plot\n"
			"  -f    Frequency [869412500]\n"
			"  -g    Gain [50]\n"
			"  -p    PPM error [47]\n";

	while ((c = getopt(argc, argv, "hqdf:g:p:")) != -1)
	switch (c)	{
		case 'h':	fprintf(stdout, "%s", helpmsg); exit(EXIT_SUCCESS); break;
		case 'q':	quiet = true;										break;
		case 'd':	debugplot = true;									break;
		case 'f':	freq = atoi(optarg);								break;
		case 'g':	gain = atoi(optarg);								break;
		case 'p':	ppm = atoi(optarg);									break;
		case '?':
		default:
			exit(EXIT_FAILURE);
	}

	//signal(SIGINT, intHandler);



	if (!quiet) printf(">> STARTING RTL_FM ...\n\n");

	fsk_init();
	hw_init();

	rtlsdr_read_async(dev, rtlsdr_callback, NULL, 0, 262144);

	while (run) {

	}

	fsk_cleanup();

	if (!quiet) printf("\n>> RTL_FM FINISHED. GoodBye!\n");

	return(0);
}

