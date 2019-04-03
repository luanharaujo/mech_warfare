#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "jstick.c"

#define DEBUG_JS 1

int keep_running = 1;	// end of program flag. it is controlled by the
						// joystick thread. if set to 0, all threads will
						// finish up and set their own flags to 1 so that
						// the main program can clean everything up and end.

int joystick_finished = 1;
int mira_finished = 1;

int shutdown_flag = 0, reboot = 0, close_program=0;	// flags set by joystick
													// commands so that the
													// program knows what to
													// do when finishing up.

/*
This is the main thread. In it, we are supposed to put everything that doesn't
belong in the infrastructure threads below it. Generally, it is used to test
new features using the joystick controller.
*/

/*
//Butterworth Filter - A manteiga que vale
#define A0 1.000000000000000
#define A1 -1.968427786938518
#define A2 1.735860709208886
#define A3 -0.724470829507362
#define A4 0.120389599896245
#define B0 0.010209480791203
#define B1 0.040837923164813
#define B2 0.061256884747219
#define B3 0.040837923164813
#define B4 0.010209480791203

double A[5] = {A0, A1, A2, A3, A4};
double B[5] = {B0, B1, B2, B3, B4};
double bw_filtered[4];
double bw_raw[5];
*/


/*
This is the joystick thread. It runs the code behind reading and interpreting
joystick commands. The joystick is the primary way of commanding the robot and
will be eventually used as safety trigger. Aside from that, it allows the
operator to control the robot without the need for opening a terminal.
All joystick support functions available in the jstick.c file.
*/
PI_THREAD(joystick)
{
    joystick_finished = 0;
	piHiPri(0);

    init_joystick(&js, devname);
    if(DEBUG_JS) init_print_js();
 
	// START+SELECT finishes the program
    while((!(js.select && js.start)) && (keep_running)) 
    {
		if(DEBUG_JS) update_print_js();
		if(js.disconnect)
        {
        	//conectar novamente caso desconect
		    init_joystick(&js, devname);
		}
		
        update_joystick(&js);
	}
	// If a D-Pad key is pressed along with START+SELECT when finishing
	// the program, special finishing up routines are called inside the
	// clean_up() function. They are:

	// DOWN+START+SELECT: shuts the Raspberry Pi Zero W down
	if(js.dpad.down) shutdown_flag = 1;
	// UP+START+SELECT: reboots the Raspberry Pi Zero W
	if(js.dpad.up) reboot = 1;

	keep_running = 0;
	joystick_finished = 1;
}

#define DIR 37
#define STEP 35 
#define MOD0 11
#define MOD1 13
#define MOD2 15

#define PERIOD 10


PI_THREAD(mira)
{
    mira_finished = 0;
	piHiPri(0);

	//void setup
	pinMode(DIR, OUTPUT);
	pinMode(STEP, OUTPUT);
	pinMode(MOD0, OUTPUT);
	pinMode(MOD1, OUTPUT);
	pinMode(MOD2, OUTPUT);

	//setep
	digitalWrite(MOD0, LOW);
	digitalWrite(MOD1, LOW);
	digitalWrite(MOD2, LOW);

	//direção
	digitalWrite(DIR, LOW);
	//digitalWrite(DIR, HIGH);

    while(keep_running)
    {
		//void loop
		digitalWrite(STEP, HIGH);
		delay(PERIOD/2);
		digitalWrite(STEP, LOW);
		delay(PERIOD/2);

	}
	
	mira_finished = 1;
}



int am_i_su()
{
    if(geteuid())
    	return 0;
    return 1;
}

void clean_up()
{
	int nao_terminou = 1;
	
	do {
		nao_terminou = !(joystick_finished && mira_finished); 
		/*
		printf("%d...", main_finished);
		printf("%d...", joystick_finished);
		printf("%d...", debug_finished);
		printf("%d...", sensors_finished);
		printf("%d...", supervisory_finished);
		printf("%d...", plot_finished);
		printf("%d...", motors_finished);
		printf("%d...\n", nao_terminou);
		*/
	} while(nao_terminou);

	system("clear&");
	if(shutdown_flag) system("sudo shutdown now&");
	else if(reboot) system("sudo shutdown -r now&");
}

int main(int argc, char* argv[])
{
	if(!am_i_su())
	{
		printf("Restricted area. Super users only.\n");
		return 0;
	}

	wiringPiSetupPhys();

	
	piThreadCreate(joystick);
	//piThreadCreate(mira);


	while(keep_running) delay(100);
	clean_up();

	return 0;
}