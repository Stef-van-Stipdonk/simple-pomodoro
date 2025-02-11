#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>
#include <poll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>

char *description_message = "\
	Simple Pomodoro Timer\n\
	To list all currently running timers:\n\
	`pomo -l`\n\
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

void find_daemons() {
    FILE *fp = popen("pgrep pomo_timer", "r");
    if (!fp) {
        perror("popen failed");
        return;
    }

    char pid[16];
    while (fgets(pid, sizeof(pid), fp)) {
        printf("PID = %s", pid);
    }

    pclose(fp);
}

size_t is_valid_number(const char *p_arg)
{
	char *endptr;
	errno = 0;
	size_t ret = strtol(p_arg, &endptr, 10);

	if (errno != 0) {
		perror("strtol");
		exit(1);
	}

	if (ret < 1) {
		return 0;
	}

	return ret;
}

int daemon_init(char* p_label, size_t p_label_size, size_t p_total_seconds) {
	prctl(PR_SET_NAME, "pomo_timer", 0, 0, 0);

	pid_t pid;

	if ((pid = fork()) < 0)
		return -1;
	else if (pid != 0) {
		exit(0);
	}

	setsid();
	chdir("/");
	umask(0);

	while (p_total_seconds > 0) {
		sleep(1);
		p_total_seconds--;
	}

	char cmd[1000];

	if (p_label_size < 1)
		sprintf(cmd, "notify-send \"Timer: %s\n\"", "generic");
	else
		sprintf(cmd, "notify-send \"\nTimer: %s\n\"", p_label);

	system(cmd);

	return 0;
}

int main(int p_argc, char** p_argv)
{
	if (p_argc == 1) {
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
	
	bool label_flag = false;
	char *label_opt_value = NULL;
	size_t label_size = 0;
	size_t c;

	opterr = 0;

	while ((c = getopt(p_argc, p_argv, "ls:m:h:n:")) != -1) {
		switch (c) {
		case 'l':
			find_daemons();
			return 0;
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
		case 'n':
			label_flag = true;
			label_opt_value = optarg;

			if ((label_size = strlen(label_opt_value)) < 1) {
				fprintf(stderr, "Option -l expects a value made up of at least one character");
			} 
			break;
		case '?':
			if (optopt == 's' || optopt == 'm' || optopt == 'h' || optopt == 'l')
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

	printf("Started timer\n");
	daemon_init(label_opt_value,label_size, total_seconds);

	return 0;
}
