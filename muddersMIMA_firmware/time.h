//JTS2doLater: eeprom.h isn't wrapped into MVP yet

#ifndef time_h
#define time_h

	#define PERIOD_AFTER_KEYON_WHERE_PRESTART_ALLOWED_ms 2000

	#define START_TIMER true
	#define STOP_TIMER false

	void time_waitForLoopPeriod(void);

	void time_stopwatch(bool timerAction);

	uint16_t time_hertz_to_milliseconds(uint8_t hertz);

	void time_loopPeriod_ms_set(uint8_t period_ms);
	uint8_t time_loopPeriod_ms_get(void);

	uint32_t time_latestKeyOn_ms(void);

	void time_handler(void);

#endif
