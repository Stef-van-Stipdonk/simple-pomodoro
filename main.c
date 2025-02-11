#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

char *description_message = "\
	Simple Pomodoro Timer\n\
	Pass flags to set length of the timer:\n\
	-s for seconds\n\
	-m for minutes\n\
	-h for hours\n\
	\n\
	The total time will be summed.\n\
	The program will sleep for the total summed duration, then notify you.\n\
	\n\
	EXAMPLE:\n\
	pomo -m 40 <This will sleep the thread for 40 minutes>\n\
";

size_t is_valid_number(const char *arg) {
	char *endptr;
	errno = 0;
	size_t ret = strtol(arg, &endptr, 10);

	if (errno != 0) {
		perror("strtol");
		exit(1);
	}

	if (ret < 1) {
		return 0;
	}

	return ret;
}

void print_time_remaining(size_t total_seconds) {
	size_t hours = total_seconds / 3600;
	size_t minutes = (total_seconds % 3600) / 60;
	size_t seconds = total_seconds % 60;

	printf("\r%02lu:%02lu:%02lu", hours, minutes, seconds);
	fflush(stdout);
}

int main(int argc, char** argv) {
	if (argc == 1) {
		printf("%s\n", description_message);
		return 0;
	}

	bool seconds_flag = false;
	char *seconds_opt_value = NULL;
	size_t seconds_value = 0;

	bool minutes_flag = false;
	char *minutes_opt_value = NULL;
	size_t minutes_value = 0;

	bool hours_flag = false;
	char *hours_opt_value = NULL;
	size_t hours_value = 0;

	size_t c;

	opterr = 0;

	while ((c = getopt(argc, argv, "s:m:h:")) != -1) {
		switch (c) {
		case 's':
			seconds_flag = true;
			seconds_opt_value = optarg;
			if ((seconds_value = is_valid_number(seconds_opt_value)) == 0) {
				fprintf(stderr, "Option -s expects a value made up of digits not: %s", seconds_opt_value);
				return 1;
			}
			break;
		case 'm':
			minutes_flag = true;
			minutes_opt_value = optarg;
			if ((minutes_value = is_valid_number(minutes_opt_value)) == 0) {
				fprintf(stderr, "Option -m expects a value made up of digits not: %s", minutes_opt_value);
				return 1;
			}
			break;
		case 'h':
			hours_flag = true;
			hours_opt_value = optarg;
			if ((hours_value = is_valid_number(hours_opt_value)) == 0) {
				fprintf(stderr, "Option -h expects a value made up of digits not: %s", hours_opt_value);
				return 1;
			}
			break;
		case '?':
			if (optopt == 's' || optopt == 'm' || optopt == 'h')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option -%c.\n", optopt);
			else
				fprintf(stderr, "Unknown option character \\x%x.\n", optopt);
			return 1;
		default:
			abort();
		}
	}

	size_t total_seconds = (hours_value * 3600) + (minutes_value * 60) + seconds_value;

	if (total_seconds == 0) {
		fprintf(stderr, "Invalid duration. Timer must be at least 1 second.\n");
		return 1;
	}

	printf("Timer started! Counting down from:\n");

	while (total_seconds > 0) {
		print_time_remaining(total_seconds);
		sleep(1);
		total_seconds--;
	}

	printf("\rTime is up!\n");
	system("notify-send 'Break time!'");

	return 0;
}
