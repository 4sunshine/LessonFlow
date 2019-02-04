#include "display.h"



display::display(int ms, int ck, int ld){
	driver_n = DEF_DRIVER_N;
	display_n = DEF_DISPLAY_N;
	mosi_pin = ms;
	clk_pin = ck;
	load_pin = ld;
	init();
}

display::init(){
	lc = LedControl(mosi_pin, clk_pin, load_pin, driver_n);
	for(int addr = 0; addr < driver_n; addr++){
		lc.shutdown(addr,false);
		lc.setIntensity(addr,15);
		lc.clearDisplay(addr);
	}
}

void display::clearAll(){
	for(int addr = 0; addr < driver_n; addr++){
		lc.clearDisplay(addr);
	}
}

void display::putNumber(int disp, int number){
	int addr = disp / 2;	// driver addr
	int digit = disp % 2 ? 3 : 7; // digit offset
	int d = number;
	for(int i = 0; i < 4; i++){
		setDigit(addr, digit, d%10, false);	
		digit--;
		if(!d) break;
		d /= 10;
	}
}
