/*
 * @file experiment4.c
 * @author Maíra Canal (@mairacanal)
 * @date 17 March 2022
 * @brief A userspace application that replies a GPIO signal in realtime
 * @ref https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base
 */

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <gpiod.h>
#include <unistd.h>
#include <sys/mman.h>

#define INPUT_GPIO 16    // P9_15
#define OUTPUT_GPIO 20   // P9_41

/*  
 *  @brief Opens the gpio chip and get the requested gpio line
 *  @param chipname The gpiochip name
 *  @param gpio The gpiod_line number
 *  @return returns the correspondent gpiod_line
 */
struct gpiod_line *get_gpio_line(const char* chipname, int gpio) {

    struct gpiod_chip *chip;
    struct gpiod_line *line;

    // Open GPIO chip
    chip = gpiod_chip_open_by_name(chipname);
    if (chip == NULL) {
        perror("Error opening GPIO chip");
        return NULL;
    }
    
    // Open GPIO line
    line = gpiod_chip_get_line(chip, gpio);
    if (line == NULL) {
        perror("Error opening GPIO line");
        return NULL;
    }

    return line;

}

void *gpio_response(void* data)
{
    const char *inputChipname = "gpiochip1";
    const char *outputChipname = "gpiochip0";
    
    struct gpiod_line *inputLine, *outputLine;
    struct gpiod_line_event interrupt;

    int value = 0;

    inputLine = get_gpio_line(inputChipname, INPUT_GPIO);
    outputLine = get_gpio_line(outputChipname, OUTPUT_GPIO);

	// Request a interrupt at the gpiod_line
    if (gpiod_line_request_both_edges_events(inputLine, "gpio_event") < 0) {
        perror("Request event failed");
        return NULL;
    }

	// Sets the gpiod_line to output
    if (gpiod_line_request_output(outputLine, "gpio_event", value) < 0) {
        perror("Request output failed");
        return NULL;
    }

    while (1) {

		// Waits the event be triggered
        gpiod_line_event_wait(inputLine, NULL);

        if (gpiod_line_event_read(inputLine, &interrupt) != 0) 
            continue;

        // Copies the input value to the output
        value = gpiod_line_get_value(inputLine);
        gpiod_line_set_value(outputLine, value);

    }

    return NULL;
}

int main(int argc, char** argv)
{
    struct sched_param param;
    pthread_attr_t attr;
    pthread_t thread;

    int ret = 0;

    /* Lock Memory */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed: %m\n");
        exit(-2);
    }

    /* Initialize pthread attributes */
    if (ret = pthread_attr_init(&attr)) {
        perror("init pthread attributes failed\n");
        goto out;
    }

    /* Set a specific stack size */
    if (ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN)) {
        perror("pthread setstacksize failed\n");
        goto out;
    }

    /* Set scheduler policy and priority of pthread */
    if (ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO)) {
        printf("pthread setschedpolicy failed\n");
        goto out;
    }

    param.sched_priority = 99;
    if (ret = pthread_attr_setschedparam(&attr, &param)) {
        printf("pthread setschedparam failed\n");
        goto out;
    }

    /* Use scheduling parameters of attr */
    if (ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED)) {
        printf("pthread setinheritsched failed\n");
        goto out;
    }

    /* Create a pthread with specified attributes */
    if (ret = pthread_create(&thread, &attr, gpio_response, NULL)) {
        printf("create pthread failed\n");
        goto out;
    }

    /* Join the thread and wait until it is done */
    if (ret = pthread_join(thread, NULL))
        printf("join pthread failed: %m\n");

out:
    return ret;

}
