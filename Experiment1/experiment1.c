/*
 * @file experiment1.c
 * @author Maíra Canal (@mairacanal)
 * @date 02 september 2021
 * @brief A userspace application that replies a GPIO signal
 */

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

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

int main(int argc, char **argv) {

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
        return EXIT_FAILURE;
    }

	// Sets the gpiod_line to output
    if (gpiod_line_request_output(outputLine, "gpio_event", value) < 0) {
        perror("Request output failed");
        return EXIT_FAILURE;
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

    return EXIT_SUCCESS;

}

