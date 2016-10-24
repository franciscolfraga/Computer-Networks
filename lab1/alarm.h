#ifndef ALARM_H_   /* Include guard */
#define ALARM_H_


extern int alarmFired;

void alarmHandler(int signal);

void setAlarm();

void stopAlarm();

#endif // ALARM_H_