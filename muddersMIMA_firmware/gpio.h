#ifndef gpio_h
	#define gpio_h
	
	#define BUTTON_PRESSED      0
	#define BUTTON_NOT_PRESSED  1

	#define TOGGLE_POSITION0 3
	#define TOGGLE_POSITION1 1
	#define TOGGLE_POSITION2 2
	#define TOGGLE_UNDEFINED 0

	void gpio_begin(void);

	bool gpio_getButton_momentary(void);

	uint8_t gpio_getButton_toggle(void);

	void gpio_setMAMODE1_percent(uint8_t newPercent);
	
	void gpio_setCMDPWR_percent(uint8_t newPercent);

	bool gpio_getMAMODE2_bool(void);
	
	void gpio_setMAMODE2_bool(bool mode);

#endif