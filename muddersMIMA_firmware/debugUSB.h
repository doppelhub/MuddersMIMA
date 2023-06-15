//Copyright 2022-2023(c) John Sullivan


#ifndef debugUSB_h
	#define debugUSB_h
	
	#define TWO_DECIMAL_PLACES 2
	#define FOUR_DECIMAL_PLACES 4

	#define DEBUG_USB_UPDATE_PERIOD_mS 100

	#define DEBUGUSB_STREAM_BUTTON      0x11
	#define DEBUGUSB_STREAM_OEM_SIGNALS 0x22
	#define DEBUGUSB_STREAM_NONE        0x44

	void debugUSB_displayUptime_seconds(void);
	void debugUSB_printButtonStates(void);
	void debugUSB_printOEMsignals(void);

	void     debugUSB_dataUpdatePeriod_ms_set(uint16_t newPeriod);
	uint16_t debugUSB_dataUpdatePeriod_ms_get(void);

	void    debugUSB_dataTypeToStream_set(uint8_t dataType);
	uint8_t debugUSB_dataTypeToStream_get(void);

	void debugUSB_printLatestData(void);
	
#endif