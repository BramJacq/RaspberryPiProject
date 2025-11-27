#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <pigpio.h>

int main(void)
{
    //use gpio26
    static const unsigned int LED_GPIO_PIN = 26; 
    gpioInitialise();

    gpioSetMode(LED_GPIO_PIN, PI_OUTPUT);

    printf("Blinking LED on GPIO %d...\n", LED_GPIO_PIN);

    while (1)
    {
        gpioWrite(LED_GPIO_PIN, PI_ON); // LED ON
        sleep(1);

        gpioWrite(LED_GPIO_PIN, PI_OFF); // LED OFF
        sleep(1);
    }

    gpioTerminate();   
    return 0;
}
