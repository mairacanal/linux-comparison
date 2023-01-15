/**
 * @file experiment4.c
 * @author Maíra Canal (@mairacanal)
 * @date 15 January 2023
 * @brief A userspace application that replies a GPIO signal in Xenomai
 */

#include <gpiod.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <alchemy/task.h>

#define INPUT_GPIO 16    // P9_15
#define OUTPUT_GPIO 20   // P9_41

RT_TASK loop_task;

/**
 * @brief Opens the gpio chip and get the requested gpio line
 * @param chipname The gpiochip name
 * @param gpio The gpiod_line number
 * @return returns the correspondent gpiod_line
 */
struct gpiod_line *get_gpio_line(const char* chipname, int gpio)
{
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

void loop_task_proc(void *arg)
{
	RT_TASK *curtask;
	RT_TASK_INFO curtaskinfo;

	RTIME tstart, now;

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
	return;
	}

	// Sets the gpiod_line to output
	if (gpiod_line_request_output(outputLine, "gpio_event", value) < 0) {
	perror("Request output failed");
	return;
	}

	curtask = rt_task_self();
	rt_task_inquire(curtask, &curtaskinfo);

	// Start the task loop
	while (1) {

		// Waits the event be triggered
		gpiod_line_event_wait(inputLine, NULL);

		if (gpiod_line_event_read(inputLine, &interrupt) != 0)
		    continue;

		// Copies the input value to the output
		value = gpiod_line_get_value(inputLine);
		gpiod_line_set_value(outputLine, value);
	}
}

int main(int argc, char **argv)
{
	char str[20];

	// Lock the memory to avoid memory swapping for this program
	mlockall(MCL_CURRENT | MCL_FUTURE);

	printf("Starting experiment4...\n");

	// Create the real time task
	sprintf(str, "experiment4");
	rt_task_create(&loop_task, str, 0, 50, 0);

	// Since task starts in suspended mode, start task
	rt_task_start(&loop_task, &loop_task_proc, 0);

	// Wait for Ctrl-C
	pause();

	return 0;
}
