/* 
 * "Small Hello World" example. 
 * 
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example 
 * designs. It requires a STDOUT  device in your system's hardware. 
 *
 * The purpose of this example is to demonstrate the smallest possible Hello 
 * World application, using the Nios II HAL library.  The memory footprint
 * of this hosted application is ~332 bytes by default using the standard 
 * reference design.  For a more fully featured Hello World application
 * example, see the example titled "Hello World".
 *
 * The memory footprint of this example has been reduced by making the
 * following changes to the normal "Hello World" example.
 * Check in the Nios II Software Developers Manual for a more complete 
 * description.
 * 
 * In the SW Application project (small_hello_world):
 *
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 * In System Library project (small_hello_world_syslib):
 *  - In the C/C++ Build page
 * 
 *    - Set the Optimization Level to -Os
 * 
 *    - Define the preprocessor option ALT_NO_INSTRUCTION_EMULATION 
 *      This removes software exception handling, which means that you cannot 
 *      run code compiled for Nios II cpu with a hardware multiplier on a core 
 *      without a the multiply unit. Check the Nios II Software Developers 
 *      Manual for more details.
 *
 *  - In the System Library page:
 *    - Set Periodic system timer and Timestamp timer to none
 *      This prevents the automatic inclusion of the timer driver.
 *
 *    - Set Max file descriptors to 4
 *      This reduces the size of the file handle pool.
 *
 *    - Check Main function does not exit
 *    - Uncheck Clean exit (flush buffers)
 *      This removes the unneeded call to exit when main returns, since it
 *      won't.
 *
 *    - Check Don't use C++
 *      This builds without the C++ support code.
 *
 *    - Check Small C library
 *      This uses a reduced functionality C library, which lacks  
 *      support for buffering, file IO, floating point and getch(), etc. 
 *      Check the Nios II Software Developers Manual for a complete list.
 *
 *    - Check Reduced device drivers
 *      This uses reduced functionality drivers if they're available. For the
 *      standard design this means you get polled UART and JTAG UART drivers,
 *      no support for the LCD driver and you lose the ability to program 
 *      CFI compliant flash devices.
 *
 *    - Check Access device drivers directly
 *      This bypasses the device file system to access device drivers directly.
 *      This eliminates the space required for the device file system services.
 *      It also provides a HAL version of libc services that access the drivers
 *      directly, further reducing space. Only a limited number of libc
 *      functions are available in this configuration.
 *
 *    - Use ALT versions of stdio routines:
 *
 *           Function                  Description
 *        ===============  =====================================
 *        alt_printf       Only supports %s, %x, and %c ( < 1 Kbyte)
 *        alt_putstr       Smaller overhead than puts with direct drivers
 *                         Note this function doesn't add a newline.
 *        alt_putchar      Smaller overhead than putchar with direct drivers
 *        alt_getchar      Smaller overhead than getchar with direct drivers
 *
 */

#include "system.h"
#include "sys/alt_stdio.h"
#include "sys/alt_irq.h"
#include "priv/alt_legacy_irq.h"
#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "altera_avalon_pio_regs.h"

volatile unsigned int* audio_control;
volatile unsigned int* audio_fifospace;
volatile unsigned int* audio_leftdata;
volatile unsigned int* audio_rightdata;

volatile unsigned int* config_control;
volatile unsigned int* config_status;
volatile unsigned int* config_address;
volatile unsigned int* config_data;

void wait() {
	while ((*config_status & 0x102) == 0);
}

void config_wm8731(alt_u8 addr, alt_u16 data) {
	wait();

	*config_address = addr;
	*config_data = data;

	*config_control = 0x340002;
}

void init_wm8731() {
	config_wm8731(0x0F, 0x000); // Reset CODEC

	wait();

	config_wm8731(0x00, 0x097); // Left Line In default
	config_wm8731(0x01, 0x097); // Right Line In default
	config_wm8731(0x02, 0x07F);	// Left output full volume
	config_wm8731(0x03, 0x07F);	// Right output full volume
	config_wm8731(0x04, 0x012);	// Analog Audio Config: Using DAC, Line In
	config_wm8731(0x05, 0x000); // Digital Audio Config: Output unmuted, no filter
	config_wm8731(0x06, 0x047); // Power off inputs and clock output (not needed)
	config_wm8731(0x07, 0x009); // A bunch of config
	config_wm8731(0x08, 0x000); // Sampling rate 48kHz normal
	config_wm8731(0x09, 0x001); // Activate
}

/*
 *
 */
int main() {
	audio_control = (unsigned int *) AUDIO_BASE;
	audio_fifospace = audio_control + 1;
	audio_leftdata = audio_control + 2;
	audio_rightdata = audio_control + 3;

	config_control = (unsigned int *) AUDIO_CONFIG_BASE;
	config_status = config_control + 1;
	config_address = config_control + 2;
	config_data = config_control + 3;

	*config_control = 0x340003;
	*config_control = 0x340002;

	init_wm8731();

	*audio_control = 0xE;	// Set clears to 1
	*audio_control = 0x2;	// Set clears to 0 for normal flow

	while (1) {
		if ((*audio_control & 0x200) != 0) {
			// Play
		}
	}

    return 0;
}

