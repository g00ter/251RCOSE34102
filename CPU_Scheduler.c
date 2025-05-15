#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#define MAX_PROCESS_NUMBER 5
typedef struct process {
	int pid; // 프로세스 번호
	int arrival; // 도착시간
	int cpu_burst; // 실행시간
	int priority; // 우선순위
}process;//io 구현은 본 스케줄러 구현에서 제외했으므로, 관련된 필드는 정의하지 않았다.

process process_list[MAX_PROCESS_NUMBER];//프로세스를 담은 구조체 배열
float waiting_time_array[MAX_PROCESS_NUMBER]={ 0, };
float turnaround_time_array[MAX_PROCESS_NUMBER] = { 0, };//각 알고리즘의 성능 비교를 위해 평균 waitingtime과 turnaroundtime 저장
void process_info(process* list,int size) {
    for (int i = 0; i < size; i++) {
        printf("PID : %d  Arrival Time : %d  CPU Burst : %d  Priority : %d\n", process_list[i].pid, process_list[i].arrival, process_list[i].cpu_burst, process_list[i].priority);
    }
    printf("\n");
}//프로세스 목록 정보 출력

bool time_dup_check(int arrival) {//arrival time이 겹치지 않게 함
    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {
        if (process_list[i].arrival == arrival) {
            return true;  // 이미 같은 arrival 시간이 있음
        }
    }
    return false;  // 겹치는 arrival 시간이 없음
}

void create_process(int num) {
    int process_count = 0;
    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {
        process p = { 0, };
        int arrival_time;
        do {
            arrival_time = rand() % 10;  // 0 ~ 5 범위에서 랜덤으로 도착 시간 생성
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
    printf("Gantt Chart for FCFS\n=============================\n");
    printf("PID             Time\n\n");
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (list[j].arrival > list[j + 1].arrival) {
                process temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }//순서대로 프로세스 정렬
    int current_time = 0;//

    for (int i = 0; i < size; i++) {//모든 프로세스에 대해 반복
        process p = process_list[i];
        if (current_time < p.arrival) {
            printf("idle             %d ~ %d\n", current_time, p.arrival);
            current_time = p.arrival;
        }//현재 시간이 프로세스 p의 arrival time보다 작다면(cpu idle) arrival time으로 업데이트함

        printf("%d                %d ~ %d\n", p.pid, current_time, current_time + p.cpu_burst);
        int start_time = current_time;//프로세스의 시작시간을 현재 시간으로 초기화
        int end_time = start_time + p.cpu_burst;//프로세스 종료 시간->시작시간+cpu burst time
        turnaround_time_array[0] += (float)(end_time - p.arrival);//프로세스 turnaround time. FCFS에서는 종료시간-arrival time
        waiting_time_array[0] += (float)(start_time - p.arrival);//waiting time. FCFS는 마지막 종료시간-arrival time

        //printf("PID: %d  Arrival Time: %d  CPU Burst: %d\n", p.pid, p.arrival, p.cpu_burst);
        current_time = end_time;//종료시간 초기화->이후 프로세스 연산에 사용하기 위함
    }

    waiting_time_array[0] /= size;
    turnaround_time_array[0] /= size;//평균 waiting time과 turnaround time
    printf("=============================\n");
    printf("Average Waiting Time: %.2f\n", waiting_time_array[0]);
    printf("Average Turnaround Time: %.2f\n", turnaround_time_array[0]);
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

    waiting_time_array[1] = (float)total_waiting_time / size;
    turnaround_time_array[1] = (float)total_turnaround_time / size;

    printf("Average Waiting Time: %.2f\n", waiting_time_array[1]);
    printf("Average Turnaround Time: %.2f\n", turnaround_time_array[1]);

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

    for (int i = 0; i < size; i++) {//모든 프로세스에 대해 반복
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

    waiting_time_array[2] = (float)total_waiting_time / size;
    turnaround_time_array[2] = (float)total_turnaround_time / size;//평균 waiting time과 turnaround time

    printf("Average Waiting Time: %.2f\n", waiting_time_array[2]);
    printf("Average Turnaround Time: %.2f\n", turnaround_time_array[2]);
}

void RR(process* list, int size) {
    int quantum = rand() % 3 + 1;  // 1~3 사이의 랜덤한 타임 퀀텀
    int current_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int completed = 0;
    int remaining_cpu_burst[MAX_PROCESS_NUMBER] = { 0 };

    // 각 프로세스의 남은 실행 시간 초기화
    for (int i = 0; i < size; i++) {
        remaining_cpu_burst[i] = list[i].cpu_burst;
    }

    printf("Round Robin Scheduling (Quantum = %d):\n", quantum);

    // 종료된 프로세스 수가 전체 수와 같아질 때까지 반복
    while (completed < size) {
        int idle = 1;

        // PID 순서대로 처리
        for (int i = 0; i < size; i++) {
            if (remaining_cpu_burst[i] > 0 && list[i].arrival <= current_time) {
                idle = 0;

                int exec_time = (remaining_cpu_burst[i] > quantum) ? quantum : remaining_cpu_burst[i];

                printf("PID: %d 실행 (Time: %d ~ %d)\n", list[i].pid, current_time, current_time + exec_time);

                current_time += exec_time;
                remaining_cpu_burst[i] -= exec_time;

                if (remaining_cpu_burst[i] == 0) {
                    int turnaround_time = current_time - list[i].arrival;
                    int waiting_time = turnaround_time - list[i].cpu_burst;

                    total_turnaround_time += turnaround_time;
                    total_waiting_time += waiting_time;
                    completed++;
                }
            }
        }

        // 실행할 프로세스가 없으면 idle 상태로 처리, 시간을 n ~ n+1 범위로 출력
        if (idle) {
            printf("CPU idle (Time: %d ~ %d)\n", current_time, current_time + 1); // idle 상태일 때 시간 범위 출력
            current_time++;
        }
    }

    // 평균 출력
    waiting_time_array[3] = (float)total_waiting_time / size;
    turnaround_time_array[3] = (float)total_turnaround_time / size;

    printf("Average Waiting Time: %.2f\n", waiting_time_array[3]);
    printf("Average Turnaround Time: %.2f\n", turnaround_time_array[3]);
}

void P_SJF(process* list, int size) {
    int current_time = 0;  // 현재 시간
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int terminated_count = 0;

    int* remaining_time = (int*)malloc(size * sizeof(int));
    bool* terminated = (bool*)malloc(size * sizeof(bool));

    for (int i = 0; i < size; i++) {
        remaining_time[i] = list[i].cpu_burst;
        terminated[i] = false;
    }

    while (terminated_count < size) {
        int index = -1;
        int shortest_remaining = 1000000;

        // 현재 시간까지 도착한 프로세스 중, 남은 실행 시간이 가장 짧은 것 선택
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {
                if (remaining_time[i] < shortest_remaining ||
                    (remaining_time[i] == shortest_remaining && list[i].arrival < list[index].arrival)) {
                    shortest_remaining = remaining_time[i];
                    index = i;
                }
            }
        }

        if (index == -1) {
            // 실행 가능한 프로세스 없음 -> idle
            printf("CPU Idle (Time: %d ~ %d)\n", current_time, current_time + 1);
            current_time++;
            continue;
        }

        // 1단위 실행
        printf("PID: %d 실행 (Time: %d ~ %d)\n", list[index].pid, current_time, current_time + 1);
        remaining_time[index]--;
        current_time++;

        // 프로세스 종료
        if (remaining_time[index] == 0) {
            int end_time = current_time;
            int turnaround_time = end_time - list[index].arrival;
            int waiting_time = turnaround_time - list[index].cpu_burst;

            total_turnaround_time += turnaround_time;
            total_waiting_time += waiting_time;

            terminated[index] = true;
            terminated_count++;
        }
    }

    waiting_time_array[4] = (float)total_waiting_time / size;
    turnaround_time_array[4] = (float)total_turnaround_time / size;

    printf("Average Waiting Time: %.2f\n", waiting_time_array[4]);
    printf("Average Turnaround Time: %.2f\n", turnaround_time_array[4]);

    free(remaining_time);
    free(terminated);
}


void P_Priority(process* list, int size) {
    int completed = 0;
    int current_time = 0;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int remaining_time[MAX_PROCESS_NUMBER];

    for (int i = 0; i < size; i++) {
        remaining_time[i] = list[i].cpu_burst;
    }

    while (completed < size) {
        int highest_priority = 9999;
        int idx = -1;

        // 현재 시간에 실행할 프로세스 선택
        for (int i = 0; i < size; i++) {
            if (list[i].arrival <= current_time && remaining_time[i] > 0) {
                if (list[i].priority < highest_priority) {
                    highest_priority = list[i].priority;
                    idx = i;
                }
                else if (list[i].priority == highest_priority) {
                    // 우선순위가 같으면 도착 시간이 더 빠른 프로세스를 선택
                    if (list[i].arrival < list[idx].arrival) {
                        idx = i;
                    }
                }
            }
        }

        if (idx != -1) {
            remaining_time[idx]--;
            current_time++;

            // 프로세스 완료 시 처리
            if (remaining_time[idx] == 0) {
                completed++;
                int end_time = current_time;
                int turnaround_time = end_time - list[idx].arrival;
                int waiting_time = turnaround_time - list[idx].cpu_burst;

                total_turnaround_time += turnaround_time;
                total_waiting_time += waiting_time;

                printf("PID: %d  Arrival: %d  CPU Burst: %d  Priority: %d\n",
                    list[idx].pid, list[idx].arrival, list[idx].cpu_burst, list[idx].priority);
            }
        }
        else {
            // 실행할 프로세스가 없으면 idle
            current_time++;
        }
    }

    waiting_time_array[5] = (float)total_waiting_time / size;
    turnaround_time_array[5] = (float)total_turnaround_time / size;

    printf("Average Waiting Time: %.2f\n", waiting_time_array[5]);
    printf("Average Turnaround Time: %.2f\n", turnaround_time_array[5]);
}

void Evaluate(float* arr1,float *arr2,int size) {
    /*printf("Algorithm     Average Waiting Time      Average Turnaround Time\n");
    printf("==================================================================================\n");
    printf("1.FCFS              %.2f                       %.2f\n", waiting_time_array[0], turnaround_time_array[0]);
    printf("2.Non-Preemptive SJF              %.2f                       %.2f\n", waiting_time_array[1], turnaround_time_array[1]);
    printf("3.Non-Preemptive Priority              %.2f                       %.2f\n", waiting_time_array[2], turnaround_time_array[2]);
    printf("4.Round-Robin              %.2f                       %.2f\n", waiting_time_array[3], turnaround_time_array[3]);
    printf("5.Preemptive SJF              %.2f                       %.2f\n", waiting_time_array[4], turnaround_time_array[4]);
    printf("6.Preemptive Priority              %.2f                       %.2f\n", waiting_time_array[5], turnaround_time_array[5]);*/
    int waiting_min_index = 0;
    int turnaround_min_index = 0;
    char min_waiting[30] = "";
    char min_turnaround[30] = "";
    for (int i = 0; i < size; i++) {
        if (arr1[i] < arr1[waiting_min_index])waiting_min_index= i;
    }
    for (int i = 0; i < size; i++) {
        if (arr2[i] < arr2[turnaround_min_index])turnaround_min_index = i;
    }
    switch (waiting_min_index) {
    case 0:
        strcpy(min_waiting, "FCFS");
        break;
    case 1:
        strcpy(min_waiting, "Non-Preemptive SJF" );
        break;
    case 2:
        strcpy(min_waiting, "Non-Preemptive Priority");
        break;
    case 3:
        strcpy(min_waiting, "Round-Robin");
        break;

    case 4:
        strcpy(min_waiting, "Preemptive SJF");
        break;

    case 5:
        strcpy(min_waiting, "Preemptive Priority");
        break;

    }
    switch (turnaround_min_index) {
    case 0:
        strcpy(min_turnaround, "FCFS");
        break;

    case 1:
        strcpy(min_turnaround, "Non-Preemptive SJF");
        break;

    case 2:
        strcpy(min_turnaround, "Non-Preemptive Priority");
        break;

    case 3:
        strcpy(min_turnaround, "Round-Robin");
        break;

    case 4:
        strcpy(min_turnaround, "Preemptive SJF");
        break;

    case 5:
        strcpy(min_turnaround, "Preemptive Priority");
        break;

    }
    printf("The algorithm that has the minimum Average Waiting Time is : %s\n",min_waiting);
    printf("The algorithm that has the minimum Average Turnaround Time is : %s\n", min_turnaround);
}

int main() {
    int num,alg;//프로세스 개수,알고리즘 종류
    srand((time(NULL)));

    //프로세스 개수 입력
    printf("input the number of the process less than 5: ");
    scanf_s("%d", &num);
    
    if (num > MAX_PROCESS_NUMBER || num<=0) {
        printf("Wrong range for the number of processes!");
        return 0;
    }//프로세스 개수를 잘못 지정했을 경우 종료
    // 프로세스 생성
    create_process(num);

    process_info(process_list,num);
   
    FCFS(process_list, num);
    Evaluate(waiting_time_array,turnaround_time_array,num);
}

