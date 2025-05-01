#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdint.h>
typedef struct process {
	int pid = 0; // 프로세스 번호
	int arrival = 0; // 도착시간
	int cpu_burst = 0; // 실행시간
	int priority = 0; // 우선순위
}process;
process Create_Process();
process FCFS();
process NP_SJF();
process P_SJF();
process NP_P();
process P_P();
process RR();
int waiting_time();
int Turnaround_time();
int main(){
	int rand();

}
