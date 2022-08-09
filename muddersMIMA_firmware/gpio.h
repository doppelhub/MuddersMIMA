#ifndef gpio_h
	#define gpio_h
	
	#define BUTTON_PRESSED      0
	#define BUTTON_NOT_PRESSED  1

	#define TOGGLE_POSITION0 3
	#define TOGGLE_POSITION1 1
	#define TOGGLE_POSITION2 2
	#define TOGGLE_SHORTED   0

	void gpio_begin(void);

	bool gpio_getButton_momentary(void);
	uint8_t gpio_getButton_toggle(void);

	void gpio_turnBuzzer_on(void);
	void gpio_turnBuzzer_off(void);

#endif