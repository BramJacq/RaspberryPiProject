#include <stdio.h>
#include <unistd.h>
#include <pigpio.h>
#include <bcm2835.h>
#include <time.h>

// GPIO definities
#define LED_GPIO_PIN 26
#define BUTTON_GPIO_PIN 16
#define SERVO_GPIO_PIN RPI_V2_GPIO_P1_12

// Servo grenzen
#define SERVO_MIN 1000
#define SERVO_MAX 2000

// Modi
typedef enum
{
	MODE_IDLE = 0,
	MODE_SWEEP,
	MODE_CENTER
} system_mode_t;

// Globale variabelen
system_mode_t currentMode = MODE_IDLE;
int lastButtonState = 1;

// ---------- Hulpfuncties ----------

void setup_servo()
{
    // Set GPIO18 (pin 12) to PWM function
    bcm2835_gpio_fsel(SERVO_GPIO_PIN, BCM2835_GPIO_FSEL_ALT5);

    // PWM clock: 19.2 MHz / 192 = 100 kHz
    bcm2835_pwm_set_clock(192);

    // Use PWM channel 0 (GPIO18)
    bcm2835_pwm_set_mode(0, 1, 1);   // channel 0, mark-space mode, enabled
    bcm2835_pwm_set_range(0, 20000); // 20 ms period (50 Hz, standard servo)
}


void set_servo_pulse(int pulse)
{
	if (pulse < SERVO_MIN) pulse = SERVO_MIN;
	if (pulse > SERVO_MAX) pulse = SERVO_MAX;

	bcm2835_pwm_set_data(1, pulse);
}

void blink_led(int times, int delayMs)
{
	for (int i = 0; i < times; i++)
	{
		gpioWrite(LED_GPIO_PIN, PI_ON);
		usleep(delayMs * 1000);
		gpioWrite(LED_GPIO_PIN, PI_OFF);
		usleep(delayMs * 1000);
	}
}

void log_mode(system_mode_t mode)
{
	switch (mode)
	{
	case MODE_IDLE:
		printf("[MODE] IDLE\n");
		break;
	case MODE_SWEEP:
		printf("[MODE] SWEEP\n");
		break;
	case MODE_CENTER:
		printf("[MODE] CENTER\n");
		break;
	}
}

// ---------- Input ----------

void check_button()
{
	int state = gpioRead(BUTTON_GPIO_PIN);

	if (state == 0 && lastButtonState == 1)
	{
		currentMode++;
		if (currentMode > MODE_CENTER)
			currentMode = MODE_IDLE;

		log_mode(currentMode);
		blink_led(2, 100);
	}

	lastButtonState = state;
}

// ---------- Modi ----------

void mode_idle()
{
	gpioWrite(LED_GPIO_PIN, PI_OFF);
	set_servo_pulse(1500);
	usleep(200000);
}

void mode_center()
{
	gpioWrite(LED_GPIO_PIN, PI_ON);
	set_servo_pulse(1500);
	usleep(200000);
}

void mode_sweep()
{
	static int angle = 0;
	static int direction = 1;

	int pulse = SERVO_MIN + (angle * (SERVO_MAX - SERVO_MIN)) / 180;
	set_servo_pulse(pulse);

	gpioWrite(LED_GPIO_PIN, (angle % 20 < 10) ? PI_ON : PI_OFF);

	angle += direction * 5;
	if (angle >= 180) direction = -1;
	if (angle <= 0) direction = 1;

	usleep(100000);
}

// ---------- Main ----------

int main(void)
{
	printf("System start...\n");

	// Pigpio init
	if (gpioInitialise() < 0)
	{
		printf("Pigpio init failed\n");
		return 1;
	}

	gpioSetMode(LED_GPIO_PIN, PI_OUTPUT);
	gpioSetMode(BUTTON_GPIO_PIN, PI_INPUT);
	gpioSetPullUpDown(BUTTON_GPIO_PIN, PI_PUD_UP);

	// BCM init
	if (!bcm2835_init())
	{
		printf("BCM2835 init failed\n");
		return 1;
	}

	setup_servo();
	log_mode(currentMode);

	// Main loop
	while (1)
	{
		check_button();

		switch (currentMode)
		{
		case MODE_IDLE:
			mode_idle();
			break;

		case MODE_SWEEP:
			mode_sweep();
			break;

		case MODE_CENTER:
			mode_center();
			break;
		}
	}

	gpioTerminate();
	bcm2835_close();
	return 0;
}
