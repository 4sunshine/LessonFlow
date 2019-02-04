#include "LedControl.h"
#define DEF_DRIVER_N 4
#define DEF_DISPLAY_N 8
#define DEF_MOSI 12
#define DEF_CLK 11
#define DEF_LOAD 10

class display{
private:
	LedControl lc;
	int driver_n, display_n, mosi_pin, clk_pin, load_pin;
	void init();

public:
	display(int ms=DEF_MOSI, int clk=DEF_CLK, int ld=DEF_LOAD);
	void putNumber(int disp, int num);
	void clearAll();

};

