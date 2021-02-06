#ifndef BGPS_H
#define BGPS_H

void checkBackgroundProcessStatus(int backgroundPIDs[]);
void addBackgroundPID(int backgroundPIDs[], int pid);
void removeBackgroundPID(int backgroundPIDs[], int pid);
void killBackgroundProcesses(int backgroundPIDs[]);

#endif