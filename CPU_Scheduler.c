#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#define MAX_PROCESS_NUMBER 3

typedef struct process {
	int pid; // 프로세스 번호
	int arrival; // 도착시간
	int cpu_burst; // 실행시간
	int priority; // 우선순위
}process;//io 구현은 본 스케줄러 구현에서 제외했으므로, 관련된 필드는 정의하지 않았다.

process process_list[MAX_PROCESS_NUMBER];//프로세스를 담은 구조체 배열

void process_info(process* list) {
    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {
        printf("PID : %d  Arrival Time : %d  CPU Burst : %d  Priority : %d\n", process_list[i].pid, process_list[i].arrival, process_list[i].cpu_burst, process_list[i].priority);
    }
}//프로세스 목록 정보 출력

bool time_dup_check(int arrival) {//arrival time이 겹치지 않게 함
    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {
        if (process_list[i].arrival == arrival) {
            return true;  // 이미 같은 arrival 시간이 있음
        }
    }
    return false;  // 겹치는 arrival 시간이 없음
}

void create_process() {
    int process_count = 0;
    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {
        process p = { 0, };
        int arrival_time;
        do {
            arrival_time = rand() % 6;  // 0 ~ 5 범위에서 랜덤으로 도착 시간 생성
        } while (time_dup_check(arrival_time));  // 이미 있는 arrival 시간인지 확인
        p.arrival = arrival_time;
        p.pid = process_count + 1;
        p.cpu_burst = (rand() % 6) + 5;       // 5 ~ 10
        p.priority = (rand() % 4) + 1;        // 1 ~ 4

        process_list[process_count++] = p;
    }
}
int compare(process* a, process* b) {
    process* p1 = a;
    process* p2 = b;

    return p1->arrival - p2->arrival;
}
// FCFS 스케줄링 함수


void FCFS(process* list, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (list[j].arrival < list[j + 1].arrival) {
                process temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }//순서대로 프로세스 정렬
    int current_time = 0;//
    int total_waiting_time = 0;//waiting time 계산
    int total_turnaround_time = 0;//turnaroud time 계산

    printf("FCFS Scheduling:\n");

    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {//모든 프로세스에 대해 반복
        process p = process_list[i];
        if (current_time < p.arrival) {
            current_time = p.arrival;
        }//현재 시간이 프로세스 p의 arrival time보다 작다면(cpu idle) arrival time으로 업데이트함

        int start_time = current_time;//프로세스의 시작시간을 현재 시간으로 초기화
        int end_time = start_time + p.cpu_burst;//프로세스 종료 시간->시작시간+cpu burst time
        int turnaround_time = end_time - p.arrival;//프로세스 turnaround time. FCFS에서는 종료시간-arrival time
        int waiting_time = start_time - p.arrival;//waiting time. FCFS는 마지막 종료시간-arrival time

        total_turnaround_time += turnaround_time;
        total_waiting_time += waiting_time;//average turnaround time과 waiting time 계산을 위한 변수

        printf("PID: %d  Arrival Time: %d  CPU Burst: %d\n", p.pid, p.arrival, p.cpu_burst);
        current_time = end_time;//종료시간 초기화->이후 프로세스 연산에 사용하기 위함
    }

    float avg_waiting_time = (float)total_waiting_time / MAX_PROCESS_NUMBER;
    float avg_turnaround_time = (float)total_turnaround_time / MAX_PROCESS_NUMBER;//평균 waiting time과 turnaround time

    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
}

void NP_SJF(process* list, int size) //프로세스 종료 시의 current time과 모든 프로세스의 arrival 비교. 
//time>arrival인 프로세스 중 가장 cpu burst가 작은 프로세스 실행
{
    int current_time = 0;  // 현재 시간
    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    // 동적 배열 할당

    bool* terminated = (bool*)malloc(size * sizeof(bool));
    // terminated 배열 초기화.해당 배열에 완료된 프로세스의 인덱스에 true가 대입되어 중복없이 실행되게 함.

    for (int i = 0; i < size; i++) {
        terminated[i] = false;  // 초기에는 모든 프로세스가 완료되지 않음
    }

    printf("Non-Preemptive SJF Scheduling:\n");

    int terminated_count = 0;

    while (terminated_count < size) {//모든 프로세스 종료 시 까지 반복
        int index = -1;
        int shortest_burst = 100;//임의의 큰 수(최대로 설정한 cpu_burst보다 크게만) 

        //조건을 만족하는 프로세스 탐색
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {//종료되지 않았으며 현재 시간 내 도착한 프로세스만 고려
                if (list[i].cpu_burst < shortest_burst ||
                    (list[i].cpu_burst == shortest_burst && list[i].arrival < list[index].arrival)) {//탐색 중 가장 짧은 프로세스를 만나거나(||의 좌측)
                    //cpu_busrt가 최소인 다른 프로세스를 만나면 arrival이 더 빠른 것으로 선택한다
                    shortest_burst = list[i].cpu_burst;//cpu_burst 최신화
                    index = i;//index 최신화(cpu burst가 최소인 프로세스의 인덱스)
                }
            }
        }

        if (index == -1) {
            current_time++;  
            continue;
        }//실행할 프로세스 없으면 current time 증가 후 다시 for문으로(cpu idle 상태)

        //프로세스 실행
        process p = list[index];
        int start_time = current_time;
        int end_time = start_time + p.cpu_burst;
        int turnaround_time = end_time - p.arrival;  
        int waiting_time = start_time - p.arrival; //FCFS와 동일한 방식으로 각 time 계산

        total_turnaround_time += turnaround_time;
        total_waiting_time += waiting_time;

        //프로세스 종료 및 종료정보 갱신
        current_time = end_time;
        terminated[index] = true;  
        terminated_count++;
    }

    float avg_waiting_time = (float)total_waiting_time / size;
    float avg_turnaround_time = (float)total_turnaround_time / size;

    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);

    free(terminated);
}

void NP_Priority(process* list, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (list[j].priority > list[j + 1].priority ||
                (list[j].priority == list[j + 1].priority && list[j].arrival > list[j + 1].arrival)) {
                process temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
    //순서대로 프로세스 정렬. priority가 같을 시에는 FCFS순서대로 정렬
    int current_time = 0;//
    int total_waiting_time = 0;//waiting time 계산
    int total_turnaround_time = 0;//turnaroud time 계산

    printf("Non-Preemptive Priority Scheduling:\n");

    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {//모든 프로세스에 대해 반복
        process p = process_list[i];

        if (current_time < p.arrival) {
            current_time = p.arrival;
        }//현재 시간이 프로세스 p의 arrival time보다 작다면(cpu idle) arrival time으로 업데이트함

        int start_time = current_time;//프로세스의 시작시간을 현재 시간으로 초기화
        int end_time = start_time + p.cpu_burst;//프로세스 종료 시간->시작시간+cpu burst time
        int turnaround_time = end_time - p.arrival;//프로세스 turnaround time. FCFS에서는 종료시간-arrival time
        int waiting_time = start_time - p.arrival;//waiting time. FCFS는 마지막 종료시간-arrival time

        total_turnaround_time += turnaround_time;
        total_waiting_time += waiting_time;//average turnaround time과 waiting time 계산을 위한 변수

        printf("PID: %d  Arrival Time: %d  CPU Burst: %d  Priority: %d\n", p.pid, p.arrival, p.cpu_burst,p.priority);
        current_time = end_time;//종료시간 초기화->이후 프로세스 연산에 사용하기 위함
    }

    float avg_waiting_time = (float)total_waiting_time / MAX_PROCESS_NUMBER;
    float avg_turnaround_time = (float)total_turnaround_time / MAX_PROCESS_NUMBER;//평균 waiting time과 turnaround time

    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);
}


//int main(){
//    int num;
//    printf("input the number of the process: ");
//    scanf_s("%d", &num);
//    while (num <=0||num>MAX_PROCESS_NUMBER) {
//        printf("Wrong number! input again:");
//        scanf_s("%d",& num);
//    }
//
//}
int main() {
    srand((time(NULL)));

    // 프로세스 생성
        create_process();

    process_info(process_list);
    //FCFS(process_list, MAX_PROCESS_NUMBER);
    //NP_SJF(process_list,MAX_PROCESS_NUMBER);
    NP_Priority(process_list, MAX_PROCESS_NUMBER);
    return 0;
}




//process P_SJF();
//process P_P();
//process RR();
//int waiting_time();
//int turnaround_time();
//void gantt();
