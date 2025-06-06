#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#define MAX_PROCESS_NUMBER 5
typedef struct process {
    int pid; // 프로세스 번호
    int arrival; // 도착시간
    int cpu_burst; // 실행시간
    int priority; // 우선순위
}process;//io 구현은 본 스케줄러 구현에서 제외했으므로, 관련된 필드는 정의하지 않았다.

process process_list[MAX_PROCESS_NUMBER];//프로세스를 담은 구조체 배열
float waiting_time_array[MAX_PROCESS_NUMBER] = { 0, };
float turnaround_time_array[MAX_PROCESS_NUMBER] = { 0, };//각 알고리즘의 성능 비교를 위해 평균 waitingtime과 turnaroundtime 저장
void process_info(process* list, int size) {
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

void Gantt(int count, int* pid, int* end) {
    printf("|");
    for (int i = 0; i < count; i++) {
        if (pid[i] == 0)//idle 프로세스일 경우
            printf(" idle |");
        else
            printf("  P%d  |", pid[i]);//실행한 프로세스 출력
    }

    printf("\n%d", 0);
    for (int i = 0; i < count; i++) {
        printf("%7d", end[i]);
    }
}

void create_process(int num) {
    int process_count = 0;
    for (int i = 0; i < MAX_PROCESS_NUMBER; i++) {
        process p = { 0, };
        int arrival_time;
        do {
            arrival_time = rand() % 10;//0 ~ 9
        } while (time_dup_check(arrival_time));//arrival 중복 확인
        p.arrival = arrival_time;
        p.pid = process_count + 1;
        p.cpu_burst = (rand() % 11) + 5;//5 ~ 15
        p.priority = (rand() % 5) + 1;//1 ~ 5

        process_list[process_count++] = p;
    }
}

//CPU utilization 측정
float util(int total_time, int idle_time) {
    return ((float)(total_time - idle_time) / total_time) * 100;
}

void FCFS(process* list, int size) {
    int current_time = 0;

    //간트차트 출력을 위한 정보를 저장할 배열 동적할당
    int* pid = (int*)malloc(sizeof(int) * size * 100);
    int* end = (int*)malloc(sizeof(int) * size * 100);

    //프로세스 실행 기록
    int count = 0;

    //총 idle시간 합
    int total_idle = 0;

    //종료된 프로세스의 인덱스는 true로 저장하는 boolean 타입 배열
    bool* terminated = (bool*)malloc(size);
    for (int i = 0; i < size; i++) {
        terminated[i] = false;
    }
    //종료된 프로세스 개수. 아래 while 원툴
    int terminated_count = 0;

    //모든 프로세스가 실행되도록 함
    while (terminated_count < size) {

        int index = -1;//실행할 프로세스의 인덱스. -1일 경우는 idle
        int earliest_arrival = 100;//가장 arrival이 짧은 프로세스의 arrival 저장

        //현재 시간까지 도착한 프로세스 중 아직 실행되지 않은 프로세스 중에서 arrival이 가장 빠른 프로세스 선택
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {
                if (list[i].arrival < earliest_arrival) {
                    earliest_arrival = list[i].arrival;
                    index = i;
                }
            }
        }

        //현재 시간까지 도착한 프로세스가 없다면 idle로 처리
        if (index == -1) {
            int next_arrival = 100;//idle 이후에 실행할 프로세스의 arrival

            //idle 이후 실행할 프로세스의 arrival을 next_arrival에 저장
            for (int i = 0; i < size; i++) {
                if (!terminated[i] && list[i].arrival > current_time) {
                    if (list[i].arrival < next_arrival)
                        next_arrival = list[i].arrival;
                }
            }
            //idle 기간 정보를 간트차트 출력 위한 배열에 저장
            pid[count] = 0;
            end[count] = next_arrival;
            total_idle += next_arrival - current_time;
            current_time = next_arrival;
            count++;
        }

        // 선택된 프로세스 실행 후 시작,종료시간 기록
        else {
            process p = list[index];
            int start_time = current_time;
            int end_time = start_time + p.cpu_burst;

            //실행정보를 간트차트 출력 위한 배열에 저장
            pid[count] = p.pid;
            end[count] = end_time;
            count++;

            //평균 turnaround time과 waiting time 저장 위해 turnaround time과 waiting time을 각 배열에 저장
            turnaround_time_array[0] += (float)(end_time - p.arrival);
            waiting_time_array[0] += (float)(start_time - p.arrival);

            //프로세스 종료 시 현재시간 업데이트 및 종료된 프로세스정보(terminated[],terminated_count) 업데이트
            current_time = end_time;
            terminated[index] = true;
            terminated_count++;
        }
    }

    //평균 waiting time,turnaround time 계산 후 각 배열에 저장
    waiting_time_array[0] /= size;
    turnaround_time_array[0] /= size;

    //간트차트 및 CPU utilization 출력
    printf("Gantt Chart for FCFS\n");
    printf("=================================================================\n");

    Gantt(count, pid, end);

    printf("\n=================================================================\n");
    printf("Average Waiting Time: %.2f//", waiting_time_array[0]);
    printf("Average Turnaround Time: %.2f//", turnaround_time_array[0]);
    printf("CPU Utilization=%.2f%%\n", util(end[count - 1], total_idle));

    free(pid);
    free(end);
    free(terminated);
}

void NP_SJF(process* list, int size)
{

    int current_time = 0;

    //실행기록 저장용 배열 동적할당
    int* pid = (int*)malloc(sizeof(int) * size * 100);
    int* end = (int*)malloc(sizeof(int) * size * 100);
    int count = 0;
    int total_idle = 0;

    bool* terminated = (bool*)malloc(size);
    for (int i = 0; i < size; i++) {
        terminated[i] = false;
    }
    int terminated_count = 0;

    //모든 프로세스가 종료될 때 까지 반복
    while (terminated_count < size) {
        int index = -1;
        int shortest_burst = 100;

        /*현재 시간까지 도착한 프로세스 중 아직 실행되지 않은 프로세스 중에서 cpu burst가 가장 짧은 프로세스 선택
        혹은 cpu_busrt가 최소인 다른 프로세스를 만나면 arrival이 더 빠른 것으로 선택해 cpu_burst와 index에 값 대입(동일한 cpu burst에 대해서는 FCFS)*/
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {
                if (list[i].cpu_burst < shortest_burst || (list[i].cpu_burst == shortest_burst && list[i].arrival < list[index].arrival)) {
                    shortest_burst = list[i].cpu_burst;
                    index = i;
                }
            }
        }

        //현재 시간까지 도착한 프로세스가 없다면 idle로 처리
        if (index == -1) {
            int next_arrival = 100;
            for (int i = 0; i < size; i++) {
                if (!terminated[i] && list[i].arrival > current_time) {
                    if (list[i].arrival < next_arrival)
                        next_arrival = list[i].arrival;
                }
            }
            //현재 도착한 프로세스가 없을 경우 cpu idle,이후 end[] 업데이트(간트차트 표기용)
            pid[count] = 0;
            end[count] = next_arrival;
            count++;
            total_idle += next_arrival - current_time;
            current_time = next_arrival;

        }

        else {//선택된 프로세스 실행 후 시작,종료시간 기록
            process p = list[index];
            int start_time = current_time;
            int end_time = start_time + p.cpu_burst;

            //실행한 프로세스와 실행기간 기록
            pid[count] = p.pid;
            end[count] = end_time;
            count++;

            //평균 turnaround time과 waiting time 저장 위해 turnaround time과 waiting time을 각 배열에 저장
            turnaround_time_array[1] += (float)(end_time - p.arrival);
            waiting_time_array[1] += (float)(start_time - p.arrival);

            //프로세스 종료 시 현재시간 업데이트 및 종료된 프로세스정보(terminated[],terminated_count) 업데이트
            current_time = end_time;
            terminated[index] = true;
            terminated_count++;
        }
    }

    //평균 waiting time,turnaround time 계산 후 각 배열에 저장
    waiting_time_array[1] /= size;
    turnaround_time_array[1] /= size;

    //간트차트 및 CPU utilization 출력
    printf("Gantt Chart for Non Preemptive SJF\n");
    printf("=================================================================\n");

    Gantt(count, pid, end);

    printf("\n=================================================================\n");
    printf("Average Waiting Time: %.2f//", waiting_time_array[1]);
    printf("Average Turnaround Time: %.2f//", turnaround_time_array[1]);
    printf("CPU Utilization=%.2f%%\n", util(end[count - 1], total_idle));

    free(pid);
    free(end);
    free(terminated);
}

void NP_Priority(process* list, int size) {
    int current_time = 0;

    // 간트차트 출력을 위한 실행 기록 저장
    int* pid = (int*)malloc(sizeof(int) * 100 * size);
    int* end = (int*)malloc(sizeof(int) * 100 * size);
    int count = 0;
    int total_idle = 0;
    bool* terminated = (bool*)malloc(size * sizeof(bool));
    for (int i = 0; i < size; i++) {
        terminated[i] = false;
    }

    int terminated_count = 0;

    //모든 프로세스가 실행되도록 함
    while (terminated_count < size) {
        int index = -1;
        int highest_priority = 100;

        /*현재 시간까지 도착한 프로세스 중 아직 실행되지 않은 프로세스 중에서 priority가 가장 작은 프로세스 선택
        혹은 priority가 최소인 다른 프로세스를 만나면 arrival이 더 빠른 것으로 선택해 cpu_burst와 index에 값 대입(동일한 priority에 대해서는 FCFS)*/
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {
                if (list[i].priority < highest_priority || (list[i].priority == highest_priority && list[i].arrival < list[index].arrival)) {
                    highest_priority = list[i].priority;
                    index = i;
                }
            }
        }

        //실행 가능한 프로세스가 없을 경우, idle로 처리
        //이후 가장 빨리 도착하는 프로세스 찾음
        if (index == -1) {
            int next_arrival = 100;
            for (int i = 0; i < size; i++) {
                if (!terminated[i] && list[i].arrival > current_time) {
                    if (list[i].arrival < next_arrival)
                        next_arrival = list[i].arrival;
                }
            }

            //현재 도착한 프로세스가 없을 경우 cpu idle,이후 start[]와 end[] 업데이트(간트차트 표기용)
            pid[count] = 0;
            end[count] = next_arrival;
            count++;
            total_idle += next_arrival - current_time;
            current_time = next_arrival;
        }

        else {// 선택된 프로세스 실행 후 시작,종료시간 기록
            process p = list[index];
            int start_time = current_time;
            int end_time = start_time + p.cpu_burst;

            //실행한 프로세스와 실행기간 기록
            pid[count] = p.pid;
            end[count] = end_time;
            count++;

            //평균 turnaround time과 waiting time 저장 위해 turnaround time과 waiting time을 각 배열에 저장
            turnaround_time_array[2] += (float)(end_time - p.arrival);
            waiting_time_array[2] += (float)(start_time - p.arrival);

            //프로세스 종료 시 현재시간 업데이트 및 종료된 프로세스정보(terminated[],terminated_count) 업데이트
            current_time = end_time;
            terminated[index] = true;
            terminated_count++;
        }
    }

    //평균 waiting time,turnaround time 계산 후 각 배열에 저장
    waiting_time_array[2] /= size;
    turnaround_time_array[2] /= size;

    //간트차트 및 CPU utilization 출력
    printf("Gantt Chart for Non-Preemptive Priority\n");
    printf("=================================================================\n");

    Gantt(count, pid, end);

    printf("\n=================================================================\n");
    printf("Average Waiting Time: %.2f//", waiting_time_array[2]);
    printf("Average Turnaround Time: %.2f//", turnaround_time_array[2]);
    printf("CPU Utilization=%.2f%%\n", util(end[count - 1], total_idle));

    free(pid);
    free(end);
    free(terminated);
}

void RR(process* list, int size) {
    int quantum = rand() % 5 + 4;//4~8 내의 무작위 Time Quantum
    int current_time = 0;

    //실행될 프로세스의 순서를 담는 배열
    int* order = (int*)malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) order[i] = i;

    //arrival 기준 버블 정렬
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            if (list[order[j]].arrival > list[order[j + 1]].arrival) {
                int temp = order[j];
                order[j] = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }

    int* pid = (int*)malloc(sizeof(int) * size * 100);
    int* end = (int*)malloc(sizeof(int) * size * 100);
    int count = 0;
    int total_idle = 0;
    int terminated_count = 0;

    int remaining_cpu_burst[MAX_PROCESS_NUMBER] = { 0, };
    for (int i = 0; i < size; i++) {
        remaining_cpu_burst[i] = list[i].cpu_burst;
    }

    //실행할 프로세스의 인덱스
    int index = 0;


    while (terminated_count < size) {
        int i = order[index];

        //실행할 프로세스가 없을 경우(idle)
        if (list[i].arrival > current_time) {
            pid[count] = 0;
            end[count] = list[i].arrival;
            total_idle += (list[i].arrival - current_time);
            current_time = list[i].arrival;
            count++;
            continue;
        }

        //프로세스 실행
        if (remaining_cpu_burst[i] > 0) {

            //잔여 cpu burst가 time quantum보다 작을 경우 잔여 cpu burst만큼 실행
            int exec_time = (remaining_cpu_burst[i] > quantum) ? quantum : remaining_cpu_burst[i];
            pid[count] = list[i].pid;
            end[count] = current_time + exec_time;
            count++;

            current_time += exec_time;
            remaining_cpu_burst[i] -= exec_time;

            //프로세스 종료시 기록
            if (remaining_cpu_burst[i] == 0) {
                int turnaround = current_time - list[i].arrival;
                int waiting = turnaround - list[i].cpu_burst;
                turnaround_time_array[3] += turnaround;
                waiting_time_array[3] += waiting;
                terminated_count++;
            }
        }
        //다음 프로세스 실행
        index = (index + 1) % size;
    }

    waiting_time_array[3] /= size;
    turnaround_time_array[3] /= size;

    printf("Gantt Chart for Round Robin Scheduling (Time Quantum = %d)\n", quantum);
    printf("=================================================================\n");

    Gantt(count, pid, end);

    printf("\n=================================================================\n");
    printf("Average Waiting Time: %.2f//", waiting_time_array[3]);
    printf("Average Turnaround Time: %.2f//", turnaround_time_array[3]);
    printf("CPU Utilization = %.2f%%\n", util(end[count - 1], total_idle));

    free(order);
    free(pid);
    free(end);
}

void P_SJF(process* list, int size) {
    int current_time = 0;
    int terminated_count = 0;

    int remaining_cpu_burst[MAX_PROCESS_NUMBER] = { 0, };
    for (int i = 0; i < size; i++) {
        remaining_cpu_burst[i] = list[i].cpu_burst;
    }
    bool* terminated = (bool*)malloc(size * sizeof(bool));

    //실행기록 저장용 배열 동적할당
    int* pid = (int*)malloc(sizeof(int) * size * 100);
    int* end = (int*)malloc(sizeof(int) * size * 100);
    int count = 0;
    int total_idle = 0;

    for (int i = 0; i < size; i++) {
        remaining_cpu_burst[i] = list[i].cpu_burst;
        terminated[i] = false;
    }

    //이전 실행되던 프로세스정보와(간트차트 기록용) 현재 프로세스의 실행 시작시간 저장
    int prev_index = -1;
    int exec_start = 0;


    //모든 프로세스가 종료될 때 까지 반복
    while (terminated_count < size) {
        int index = -1;
        int shortest_burst = 100;


        /*현재 시간까지 도착한 프로세스 중 아직 실행되지 않은 프로세스 중에서 cpu burst가 가장 짧은 프로세스 선택
        혹은 cpu_busrt가 최소인 다른 프로세스를 만나면 arrival이 더 빠른 것으로 선택해 cpu_burst와 index에 값 대입(동일한 cpu burst에 대해서는 FCFS)*/
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {
                if (remaining_cpu_burst[i] < shortest_burst || (remaining_cpu_burst[i] == shortest_burst && list[i].arrival < list[index].arrival)) {
                    shortest_burst = remaining_cpu_burst[i];
                    index = i;
                }
            }
        }

        //현재 시간까지 도착한 프로세스가 없다면 idle로 처리
        if (index == -1) {
            //이전에 실행하던 프로세스가 있었을 경우 해당 프로세스의 실행정보 기록
            if (prev_index != -1) {
                pid[count] = list[prev_index].pid;
                end[count] = current_time;
                count++;
                //이제부터 idle상태임을 나타냄+이후에 프로세스 실행 시 이전 프로세스와 다를 경우 idle이었다.
                prev_index = -1;
            }
            //idle의 시작시점 기록
            exec_start = current_time;

            //실행 가능한 프로세스 도착할 때까지 시간 증가
            int next_arrival = 100;
            for (int i = 0; i < size; i++) {
                if (!terminated[i] && list[i].arrival > current_time) {
                    if (list[i].arrival < next_arrival) {
                        next_arrival = list[i].arrival;
                        index = i;
                    }
                }
            }
            current_time = list[index].arrival;
            
            //현재 도착한 프로세스가 없을 경우 cpu idle,이후 start[]와 end[] 업데이트(간트차트 표기용)
            pid[count] = 0;
            end[count] = current_time;
            count++;
            total_idle += current_time - exec_start;
            //다음 실행 프로세스 실행시간
            exec_start = current_time;
        }

        //프로세스 실행정보 기록 및 index 프로세스의 실행 시작시간 초기화
        else {
            //preemption 발생(idle이 아니면서 현재 실행할 프로세스가 이전에 실행하는 프로세스와 다를 경우)
            if (prev_index != -1 && prev_index != index) {
                pid[count] = list[prev_index].pid;
                end[count] = current_time;
                count++;
                exec_start = current_time;
            }

            //현재 실행중인 프로세스 인덱스를 prev_index에 저장 후 index 프로세스의 잔여 cpu burst time 1 감소 및 현재시간 1 증가
            prev_index = index;
            remaining_cpu_burst[index]--;
            current_time++;

            //현재 프로세스가 종료될 경우 turnaround time과 waiting time 저장
            if (remaining_cpu_burst[index] == 0) {
                int turnaround_time = current_time - list[index].arrival;
                int waiting_time = turnaround_time - list[index].cpu_burst;
                turnaround_time_array[4] += turnaround_time;
                waiting_time_array[4] += waiting_time;

                //프로세스 종료 시 종료된 프로세스정보(terminated[],terminated_count) 업데이트
                terminated[index] = true;
                terminated_count++;
            }
        }
    }
        //마지막 실행 프로세스 기록
        pid[count] = list[prev_index].pid;
        end[count] = current_time;
        count++;

    waiting_time_array[4] /= size;
    turnaround_time_array[4] /= size;


    printf("Gantt Chart for Preemptive SJF Scheduling\n");
    printf("=================================================================\n");

    //간트차트 출력
    Gantt(count, pid, end);

    printf("\n=================================================================\n");
    printf("Average Waiting Time: %.2f//", waiting_time_array[4]);
    printf("Average Turnaround Time: %.2f//", turnaround_time_array[4]);
    printf("CPU Utilization=%.2f%%\n", util(end[count - 1], total_idle));

    free(terminated);
    free(pid);
    free(end);
}

void P_Priority(process* list, int size) {
    int current_time = 0;
    int terminated_count = 0;

    int* remaining_time = (int*)malloc(size * sizeof(int));
    bool* terminated = (bool*)malloc(size * sizeof(bool));

    //실행기록 저장용 배열 동적할당
    int* pid = (int*)malloc(sizeof(int) * size * 100);
    int* end = (int*)malloc(sizeof(int) * size * 100);
    int count = 0;
    int total_idle = 0;

    for (int i = 0; i < size; i++) {
        remaining_time[i] = list[i].cpu_burst;
        terminated[i] = false;
    }

    printf("Gantt Chart for Preemptive Priority Scheduling\n");
    printf("=================================================================\n");

    //이전 실행되던 프로세스정보와(간트차트 기록용) 현재 프로세스의 실행 시작시간 저장
    int prev_index = -1;
    int exec_start = current_time;

    while (terminated_count < size) {
        int index = -1;
        int highest_priority = 100;

        /*현재 시간까지 도착한 프로세스 중 아직 실행되지 않은 프로세스 중에서 priority가 가장 작은 프로세스 선택
        혹은 priority가 최소인 다른 프로세스를 만나면 arrival이 더 빠른 것으로 선택해 cpu_burst와 index에 값 대입(동일한 priority에 대해서는 FCFS)*/
        for (int i = 0; i < size; i++) {
            if (!terminated[i] && list[i].arrival <= current_time) {
                if (list[i].priority < highest_priority || (list[i].priority == highest_priority && list[i].arrival < list[index].arrival)) {
                    highest_priority = list[i].priority;
                    index = i;
                }
            }
        }

        //현재 시간까지 도착한 프로세스가 없다면 idle로 처리
        if (index == -1) {
            //이전에 실행하던 프로세스가 있었을 경우 해당 프로세스의 실행정보 기록
            if (prev_index != -1) {
                pid[count] = list[prev_index].pid;
                end[count] = current_time;
                count++;
                prev_index = -1;
            }

            //idle의 시작시점 기록
            exec_start = current_time;

            //실행 가능한 프로세스 도착할 때까지 시간 증가
            int next_arrival = 100;
            for (int i = 0; i < size; i++) {
                if (!terminated[i] && list[i].arrival > current_time) {
                    if (list[i].arrival < next_arrival) {
                        next_arrival = list[i].arrival;
                        index = i;
                    }
                }
            }
            current_time = list[index].arrival;

            //idle 구간 기록
            pid[count] = 0;
            end[count] = current_time;
            count++;
            total_idle += current_time - exec_start;

            exec_start = current_time;
        }
        else {
            //preemption 발생(idle이 아니면서 현재 실행할 프로세스가 이전에 실행하는 프로세스와 다를 경우)
            //프로세스 실행정보 기록 및 index 프로세스의 실행 시작시간 초기화        
            if (prev_index != -1 && prev_index != index) {
                pid[count] = list[prev_index].pid;
                end[count] = current_time;
                count++;
                exec_start = current_time;
            }

            //현재 실행중인 프로세스 인덱스를 prev_index에 저장 후 index 프로세스의 잔여 cpu burst time 1 감소 및 현재시간 1 증가
            prev_index = index;
            remaining_time[index]--;
            current_time++;

            //현재 프로세스가 종료될 경우 turnaround time과 waiting time 저장
            if (remaining_time[index] == 0) {
                int turnaround_time = current_time - list[index].arrival;
                int waiting_time = turnaround_time - list[index].cpu_burst;
                turnaround_time_array[5] += turnaround_time;
                waiting_time_array[5] += waiting_time;

                //프로세스 종료 시 종료된 프로세스정보(terminated[],terminated_count) 업데이트
                terminated[index] = true;
                terminated_count++;
            }
        }
    }
        //마지막 실행 프로세스 기록
        pid[count] = list[prev_index].pid;
        end[count] = current_time;
        count++;

    waiting_time_array[5] /= size;
    turnaround_time_array[5] /= size;

    Gantt(count, pid, end);

    printf("\n=================================================================\n");
    printf("Average Waiting Time: %.2f//", waiting_time_array[5]);
    printf("Average Turnaround Time: %.2f//", turnaround_time_array[5]);
    printf("CPU Utilization=%.2f%%\n", util(end[count - 1], total_idle));

    free(remaining_time);
    free(terminated);
    free(pid);
    free(end);
}
void Evaluate(float* arr1, float* arr2, int size) {
    int waiting_min_index = 0;
    int turnaround_min_index = 0;
    char min_waiting[30] = "";
    char min_turnaround[30] = "";
    for (int i = 0; i < size; i++) {
        if (arr1[i] < arr1[waiting_min_index])waiting_min_index = i;
    }
    for (int i = 0; i < size; i++) {
        if (arr2[i] < arr2[turnaround_min_index])turnaround_min_index = i;
    }
    switch (waiting_min_index) {
    case 0:
        strcpy(min_waiting, "FCFS");
        break;
    case 1:
        strcpy(min_waiting, "Non-Preemptive SJF");
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
    default:
        printf("Error occured while calculating Minimum waiting time!");
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
    default:
        printf("Error occured while calculating Minimum turnaround time!");
    }

    //가장 효율적인 알고리즘 출력
    printf("The algorithm that has the minimum Average Waiting Time is : %s\n", min_waiting);
    printf("The algorithm that has the minimum Average Turnaround Time is : %s\n", min_turnaround);
}

int main() {
    int num;//프로세스 개수
    srand((time(NULL)));

    //프로세스 개수 입력
    printf("input the number of the process less than 5: ");
    scanf("%d", &num);

    if (num > MAX_PROCESS_NUMBER || num <= 0) {
        printf("Wrong range for the number of processes!");
        return 0;
    }//프로세스 개수를 잘못 지정했을 경우 종료
    //프로세스 생성
    create_process(num);

    process_info(process_list, num);

    FCFS(process_list, num);
    NP_SJF(process_list, num);
    NP_Priority(process_list, num);
    RR(process_list, num);
    P_SJF(process_list, num);
    P_Priority(process_list, num);

    Evaluate(waiting_time_array, turnaround_time_array, num);
}



