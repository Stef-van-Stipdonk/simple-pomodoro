# Simple Pomodoro Timer
Set a timer, get a notification when the timer is up.
Tested... my machine <3

Pass flags to set length of the timer:\
-s for seconds\
-m for minutes\
-h for hours\
\
The total time will be summed.\
The program will sleep for the total summed duration, then notify you.\
\
## EXAMPLE:
`pomo -m 40 -s 3 <This will sleep the thread for 40 minutes and 3 seconds>`.
\
ctrl+c stops the timer.

## INSTALLATION
Use a compiler put the output somewhere fun.

## Feature list
- [x] Set an actual timer
- [x] List active timers
- [ ] Kill active timer
- [ ] Specify effects on completing the timer (such as an alarm) instead of the current popup
