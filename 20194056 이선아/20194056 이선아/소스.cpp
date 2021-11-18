#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>

using namespace std;

void PrintMenu(int);
void ChangeTextColor(int);
void ChangeBackgroundColor(int);

struct Process {
	int PID = 0;
	int priority = 0;
	int arrivedTime = 0;
	int waitingTime = 0;
	int responseTime = 0;
	int burstTime = 0;
	int turnaroundTime = 0;
	int remainTime = 0;
};

struct GanttData {
	int PID;
	int runTime;
	GanttData* next;
};

class Scheduler {
protected:
	int timeSlice;
	int processCnt;
	Process* process_head;			// 배열로 동적 메모리 할당
	GanttData* gantt_head;	// 연결 리스트로 동적 메모리 할당

public:
	Scheduler() {
		FILE *fp = fopen("data.txt", "r");
		if (fp == NULL) {
			ChangeTextColor(12);
			cout << "\n\nFile Not Found!\n" << std::endl;
			ChangeTextColor(15);
			exit(0);
		}

		processCnt = 0;
		char buf[256];
		while (fgets(buf, 256, fp))	processCnt++;
		process_head = (Process*)malloc(processCnt * sizeof(Process));

		rewind(fp);
		char* token;
		// 프로세스 정보 저장
		for (int i = 0; i < processCnt; i++) {
			fgets(buf, 256, fp);

			token = strtok(buf, "/");
			process_head[i].PID = atoi(token);

			token = strtok(NULL, "/");
			process_head[i].arrivedTime = atoi(token);

			token = strtok(NULL, "/");
			process_head[i].burstTime = atoi(token);
			process_head[i].remainTime = process_head[i].burstTime;

			token = strtok(NULL, "/");
			process_head[i].priority = atoi(token);	// 마지막 데이터
		}
		fclose(fp);
	}
	~Scheduler() {
		free(process_head);
	}

	void PrintData() {
		cout << " +-----+----------+----------+----------+" << std::endl;
		cout << " | PID | 도착시간 | 실행시간 | 우선순위 |" << std::endl;
		cout << " +-----+----------+----------+----------+" << std::endl;

		for (int i = 0; i < processCnt; i++) {
			printf(" | %3d | %8d | %8d | %8d |\n", process_head[i].PID, process_head[i].arrivedTime, process_head[i].burstTime, process_head[i].priority);
		}
		cout << " +-----+----------+----------+----------+" << std::endl;
	}

	void AskTimeSlice() {
		PrintData();

		ChangeTextColor(6);
		cout << " 타임 슬라이스 크기 >> ";
		cin >> timeSlice;

		system("cls");
	}

	void PrintTable() {
		float sum_wait = 0;
		float sum_response = 0;
		float sum_turnaround = 0;

		// 대기시간, 응답시간, 반환시간

		cout << " +----------+";
		for (int i = 0; i < processCnt; i++) {
			cout << "-----+";
		}
		cout << "------+" << std::endl;

		cout << " |          |";
		for (int i = 0; i < processCnt; i++) {
			printf(" P%2d |", process_head[i].PID);
		}
		cout << " 평균 |" << std::endl;

		cout << " +----------+";
		for (int i = 0; i < processCnt; i++) {
			cout << "-----+";
		}
		cout << "------+" << std::endl;

		cout << " | 대기시간 |";
		for (int i = 0; i < processCnt; i++) {
			printf(" %3d |", process_head[i].waitingTime);
			sum_wait += process_head[i].waitingTime;
		}
		printf(" %4.1f |\n", sum_wait / processCnt);

		cout << " | 응답시간 |";
		for (int i = 0; i < processCnt; i++) {
			printf(" %3d |", process_head[i].responseTime);
			sum_response += process_head[i].responseTime;
		}
		printf(" %4.1f |\n", sum_response / processCnt);

		cout << " | 반환시간 |";
		for (int i = 0; i < processCnt; i++) {
			printf(" %3d |", process_head[i].turnaroundTime);
			sum_turnaround += process_head[i].turnaroundTime;
		}
		printf(" %4.1f |\n", sum_turnaround / processCnt);

		cout << " +----------+";
		for (int i = 0; i < processCnt; i++) {
			cout << "-----+";
		}
		cout << "------+" << std::endl;
	}

	void DrawGanttChart() {
		int currentTime = 0;
		int colorValue[] = { 10, 11, 12, 13, 14, 15, 9, 6, 7, 8 };
		int colorLength = 10;

		GanttData* p = gantt_head;
		//     P1
		// |[      ]|[
		// 0        5

		cout << "  ";
		while (p != NULL) {
			for (int i = 0; i < p->runTime - (p->runTime % 2); i = i + 2)	cout << " ";
			printf("P%2d", p->PID);
			for (int i = 0; i < p->runTime - (p->runTime % 2); i = i + 2)	cout << " ";
			p = p->next;
			cout << " ";
		}
		cout << std::endl;

		p = gantt_head;
		cout << " |";
		while (p != NULL) {
			ChangeBackgroundColor(colorValue[p->PID%colorLength]);		// 배경색으로 차트 표현
			for (int i = 0; i < p->runTime - (p->runTime % 2); i = i + 2)	cout << " ";
			printf("   ", p->PID);
			for (int i = 0; i < p->runTime - (p->runTime % 2); i = i + 2)	cout << " ";
			p = p->next;
			ChangeBackgroundColor(0);
			cout << "|";
		}
		cout << "─→" << std::endl;

		// 시간 표시
		p = gantt_head;
		ChangeTextColor(15);
		cout << " 0";
		while (p != NULL) {
			for (int i = 0; i < p->runTime - (p->runTime % 2); i++)	cout << " ";
			currentTime += p->runTime;
			printf("  %2d", currentTime);
			p = p->next;
		}
		cout << std::endl;

		// 동적 할당 메모리 해제
		GanttData* next;
		p = gantt_head;
		while (p != NULL) {
			next = p->next;
			free(p);
			p = next;
		}
	}

	GanttData* InsertGanttNode(GanttData* head, int pid, int time) {
		if (head == NULL) {
			gantt_head = new GanttData{ pid, time };
			return gantt_head;
		}
		GanttData* newNode = new GanttData{ pid, time };
		head->next = newNode;
		return head->next;
	}

	virtual void SchedulerName() = 0;
	virtual void Sceduling() = 0;
};

class FCFS : public Scheduler {
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ FCFS ]" << std::endl;
		ChangeTextColor(15);
	}
	void Sceduling() {
		// 도착한 순서대로 CPU를 할당 - 비선점형
		GanttData* p = NULL;

		int currentTime = 0;
		int next = 0;
		for (int i = 0; i < processCnt; i++) {
			// minTime 설정
			for (int j = 0; j < processCnt; j++)
				if (process_head[j].remainTime != 0) {
					next = j;
					break;
				}

			for (int j = 0; j < processCnt; j++) {
				if (process_head[j].remainTime != 0) {
					if (process_head[j].arrivedTime <= process_head[next].arrivedTime) {
						// 도착 시간이 가장 빠른 프로세스 구하기
						next = j;
					}
				}
			}
			process_head[next].waitingTime = currentTime - process_head[next].arrivedTime;
			process_head[next].responseTime = process_head[next].waitingTime;
			currentTime += process_head[next].burstTime;
			process_head[next].turnaroundTime = currentTime - process_head[next].arrivedTime;
			process_head[next].remainTime = 0;

			p = InsertGanttNode(p, process_head[next].PID, process_head[next].burstTime);
		}
	}
};
class SJF : public Scheduler {
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ SJF ]" << std::endl;
		ChangeTextColor(15);
	}
	void Sceduling() {
		// 실행 시간이 가장 짧은 작업부터 CPU 할당 - 비선점형
		GanttData* p = NULL;

		int currentTime = 0;
		int next = 0;

		for (int i = 0; i < processCnt; i++) {
			// minTime 구하기
			for (int i = 0; i < processCnt; i++) {
				// 끝나지 않은 프로세스 중, 현재시점에서 이미 도착한 프로세스
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					next = i;
					break;
				}
			}

			for (int i = 0; i < processCnt; i++) {
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					// 해당 프로세스의 실행시간은 next의 실행시간보다 작은가?
					if (process_head[i].burstTime <= process_head[next].burstTime)
						next = i;
				}
			}

			process_head[next].waitingTime = currentTime - process_head[next].arrivedTime;
			process_head[next].responseTime = process_head[next].waitingTime;
			currentTime += process_head[next].burstTime;
			process_head[next].turnaroundTime = currentTime - process_head[next].arrivedTime;
			process_head[next].remainTime = 0;

			p = InsertGanttNode(p, process_head[next].PID, process_head[next].burstTime);
		}
	}
};
class NP_Priority : public Scheduler {
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ 비선점 Priority ]" << std::endl;
		ChangeTextColor(15);
	}
	void Sceduling() {
		// 우선순위 값이 낮은 순서대로 할당 - 비선점형
		GanttData* p = NULL;

		int currentTime = 0;
		int next = 0;

		for (int i = 0; i < processCnt; i++) {
			// minTime 구하기
			for (int i = 0; i < processCnt; i++) {
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					next = i;
					break;
				}
			}

			for (int i = 0; i < processCnt; i++) {
				// 끝나지 않은 프로세스 중, 현재시점에서 이미 도착한 프로세스
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					// 해당 프로세스의 우선순위는 next의 우선순위보다 높은가?
					if (process_head[i].priority <= process_head[next].priority)
						next = i;
				}
			}

			process_head[next].waitingTime = currentTime - process_head[next].arrivedTime;
			process_head[next].responseTime = process_head[next].waitingTime;
			currentTime += process_head[next].burstTime;
			process_head[next].turnaroundTime = currentTime - process_head[next].arrivedTime;
			process_head[next].remainTime = 0;

			p = InsertGanttNode(p, process_head[next].PID, process_head[next].burstTime);
		}
	}
};
class P_Priority : public Scheduler {
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ 선점 Priority ] ";
		ChangeTextColor(6);
		cout << "timeSlice: " << timeSlice << std::endl;
		ChangeTextColor(15);
	}
	void Sceduling() {
		// RR에서 실행할 프로세스를 선택할 때 우선순위가 높은 순서로 실행
		// 선점형
		AskTimeSlice();
		system("cls");

		GanttData* p = NULL;

		int currentTime = 0;
		short next = 0;
		int remainProcess = processCnt;

		// 모든 프로세스 remainTime가 0이 되어야 종료
		while (remainProcess > 0) {
			for (int i = 0; i < processCnt; i++) {
				// next 시작 선정
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					next = i;
					break;
				}
			}

			for (int i = 0; i < processCnt; i++) {
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					if (process_head[i].priority < process_head[next].priority)
						next = i;
				}
			}

			// 처음 실행된 상태
			if (process_head[next].remainTime == process_head[next].burstTime) {
				process_head[next].responseTime = currentTime - process_head[next].arrivedTime;
			}
			// 남은 시간과 타임슬라이스 크기 비교
			if (timeSlice >= process_head[next].remainTime) {
				currentTime += process_head[next].remainTime;
				p = InsertGanttNode(p, process_head[next].PID, process_head[next].remainTime);
				// 프로세스 종료
				process_head[next].waitingTime = currentTime - process_head[next].arrivedTime - process_head[next].burstTime;
				process_head[next].remainTime = 0;
				process_head[next].turnaroundTime = currentTime - process_head[next].arrivedTime;
				remainProcess--;
			}
			else {
				currentTime += timeSlice;
				p = InsertGanttNode(p, process_head[next].PID, timeSlice);
				process_head[next].remainTime -= timeSlice;
			}
		}
	}
};
class RR : public Scheduler {
private:
	short* queue;
	short* lastProcessTime;
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ RR ]";
		ChangeTextColor(6);
		cout << "timeSlice: " << timeSlice << std::endl;
		ChangeTextColor(15);
	}

	RR() {
		lastProcessTime = (short*)malloc(processCnt * sizeof(processCnt));
		for (int i = 0; i < processCnt; i++)
			lastProcessTime[i] = 0;
	}

	~RR() {
		free(queue);
		free(lastProcessTime);
	}

	void SortByArrivedTime() {
		queue = (short*)malloc(processCnt * sizeof(short));
		short tmp;

		// 기본값 저장
		for (int i = 0; i < processCnt; i++) {
			queue[i] = i;
		}

		for (int i = 0; i < processCnt; i++) {
			for (int j = 0; j < processCnt - i - 1; j++) {
				// [j]와 [j+1]중 [j+1]이 더 먼저 도착했을 때 서로 스왑
				if (process_head[queue[j]].arrivedTime > process_head[queue[j + 1]].arrivedTime) {
					queue[j] = tmp;
					queue[j] = queue[j + 1];
					queue[j + 1] = tmp;
				}
			}
		}
	}

	void Sceduling() {
		// 타임슬라이스만큼 작업하고 완료하지 못하면 준비 큐의 맨 뒤로 가서 기다리는
		// 선점형
		AskTimeSlice();
		system("cls");

		GanttData* p = NULL;

		int currentTime = 0;
		short now = 0;
		int remainProcess = processCnt;

		SortByArrivedTime();

		// 모든 프로세스 remainTime가 0이 되어야 종료
		while (remainProcess > 0) {
			// 종료되지 않은 프로세스들을 queue 순서대로 실행
			if (process_head[queue[now]].remainTime != 0) {
				if (lastProcessTime[queue[now]] == 0) {	// 처음 실행
					process_head[queue[now]].waitingTime = currentTime - process_head[queue[now]].arrivedTime;
					process_head[queue[now]].responseTime = currentTime - process_head[queue[now]].arrivedTime;
				}
				else {
					// 처음 실행 X → 마지막 실행 ~ 이번 실행 = 대기 시간에 추가
					process_head[queue[now]].waitingTime += currentTime - lastProcessTime[queue[now]];
				}

				// 남은 시간과 타임슬라이스 크기 비교
				if (timeSlice >= process_head[queue[now]].remainTime) {
					currentTime += process_head[queue[now]].remainTime;
					p = InsertGanttNode(p, process_head[queue[now]].PID, process_head[queue[now]].remainTime);
					// 프로세스 종료
					process_head[queue[now]].remainTime = 0;
					process_head[queue[now]].turnaroundTime = currentTime - process_head[queue[now]].arrivedTime;
					remainProcess--;
				}
				else {
					currentTime += timeSlice;
					p = InsertGanttNode(p, process_head[queue[now]].PID, timeSlice);
					process_head[queue[now]].remainTime -= timeSlice;
				}
				lastProcessTime[queue[now]] = currentTime;
			}

			now = (now + processCnt + 1) % processCnt;
		}
	}
};
class SRT : public Scheduler {
private:
	short* lastProcessTime;
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ SRT ]";
		ChangeTextColor(6);
		cout << "timeSlice: " << timeSlice << std::endl;
		ChangeTextColor(15);
	}
	SRT() {
		lastProcessTime = (short*)malloc(processCnt * sizeof(short));
		for (int i = 0; i < processCnt; i++)
			lastProcessTime[i] = 0;
	}

	~SRT() {
		free(lastProcessTime);
	}

	void Sceduling() {
		// 기본적으로 RR 사용, 선택시 남아있는 작업시간이 가장 적은 프로세스 선택
		// 선점형
		AskTimeSlice();
		system("cls");

		GanttData* p = NULL;

		int currentTime = 0;
		short next = 0;
		int remainProcess = processCnt;

		// 모든 프로세스 remainTime가 0이 되어야 종료
		while (remainProcess > 0) {
			for (int i = 0; i < processCnt; i++) {
				// next 시작 선정
				if (process_head[i].remainTime != 0) {
					next = i;
					break;
				}
			}

			for (int i = 0; i < processCnt; i++) {
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					if (process_head[i].remainTime < process_head[next].remainTime)
						next = i;
				}
			}
			
			if (lastProcessTime[next] == 0)
				process_head[next].waitingTime = currentTime - process_head[next].arrivedTime;
			else
				process_head[next].waitingTime += currentTime - lastProcessTime[next];

			// 처음 실행된 상태
			if (process_head[next].remainTime == process_head[next].burstTime) {
				process_head[next].responseTime = process_head[next].waitingTime;
			}
			// 남은 시간과 타임슬라이스 크기 비교
			if (timeSlice >= process_head[next].remainTime) {
				currentTime += process_head[next].remainTime;
				p = InsertGanttNode(p, process_head[next].PID, process_head[next].remainTime);
				// 프로세스 종료
				process_head[next].remainTime = 0;
				process_head[next].turnaroundTime = currentTime - process_head[next].arrivedTime;
				remainProcess--;
			}
			else {
				currentTime += timeSlice;
				p = InsertGanttNode(p, process_head[next].PID, timeSlice);
				process_head[next].remainTime -= timeSlice;
			}
			lastProcessTime[next] = currentTime;
		}
	}
};
class HRN : public Scheduler {
public:
	void SchedulerName() {
		ChangeTextColor(11);
		cout << " [ HRN ]" << std::endl;
		ChangeTextColor(15);
	}

	float GetHRNPriority(Process p, int currentTime) {
		return(((float)(currentTime - p.arrivedTime) + p.burstTime) / p.burstTime);
	}

	void Sceduling() {
		// 기다린 시간과 CPU 사용시간을 고려하여 선택 - 비선점형
		// 우선순위 = (대기시간+CPU사용시간)/CPU사용시간
		// 우선순위 값이 큰 편이 먼저 실행
		GanttData* p = NULL;

		int currentTime = 0;
		int next = 0;

		for (int i = 0; i < processCnt; i++) {
			// minTime 구하기
			for (int i = 0; i < processCnt; i++) {
				if (process_head[i].remainTime != 0) {
					next = i;
					break;
				}
			}

			for (int i = 0; i < processCnt; i++) {
				// 끝나지 않은 프로세스 중, 현재시점에서 이미 도착한 프로세스
				if (process_head[i].remainTime != 0 && process_head[i].arrivedTime <= currentTime) {
					// 해당 프로세스의 우선순위는 next의 우선순위보다 높은가?
					if (GetHRNPriority(process_head[i], currentTime) >= GetHRNPriority(process_head[next], currentTime))
						next = i;
				}
			}

			process_head[next].waitingTime = currentTime - process_head[next].arrivedTime;
			process_head[next].responseTime = process_head[next].waitingTime;	// 응답시간
			currentTime += process_head[next].burstTime;
			process_head[next].turnaroundTime = currentTime;
			process_head[next].remainTime = 0;

			p = InsertGanttNode(p, process_head[next].PID, process_head[next].burstTime);
		}
	}
};

void PrintMenu(int num) {
	int basicColor = 15;

	char schedulerName[7][20] = { "      FCFS     ", "      SJF      ", "비선점 Priority"
								, " 선점 Priority ", "       RR      ", "      SRT      "
								, "      HRN      "};

	cout << "\t\t 20194056 이선아 " << std::endl;
	cout << "\t\t+===============+" << std::endl;
	ChangeTextColor(6);
	cout << "\t\t  CPU Scheduler" << std::endl;
	ChangeTextColor(basicColor);
	cout << "\t\t+===============+" << std::endl << std::endl;

	cout << "\n\n              ";
	ChangeTextColor(11);
	cout << "◀ " << schedulerName[num] << " ▶";

	ChangeTextColor(8);
	cout << std::endl << std::endl << std::endl << "              Press ← → Enter key" << std::endl;
	ChangeTextColor(basicColor);
}
void ChangeTextColor(int fg) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, fg);
}
void ChangeBackgroundColor(int bg) {
	int fg;
	if (bg <= 9)	fg = 15;
	else			fg = 0;
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, (bg << 4) | fg);
}


int main() {
	int selectMenu = 0;
	int input;

	system("Title 20194056 이선아 - CPU Scheduling Simulator");

	while (true) {
		system("mode con cols=50 lines=20");
		while (true) {	// 스케줄러 선택
			system("cls");
			cout << std::endl << std::endl << std::endl;
			PrintMenu(selectMenu);
			input = _getch();

			if (input == 75) {		// 왼쪽
				selectMenu = (selectMenu + 6) % 7;
			}
			else if (input == 77) {	// 오른쪽
				selectMenu = (selectMenu + 8) % 7;
			}
			else if (input == 13) {	// Enter
				break;
			}
		}

		Scheduler* scheduler;
		if (selectMenu == 0)	scheduler = new FCFS();
		else if (selectMenu == 1)	scheduler = new SJF();
		else if (selectMenu == 2)	scheduler = new NP_Priority();
		else if (selectMenu == 3)	scheduler = new P_Priority();
		else if (selectMenu == 4)	scheduler = new RR();
		else if (selectMenu == 5)	scheduler = new SRT();
		else if (selectMenu == 6)	scheduler = new HRN();

		system("mode con cols=100 lines=30");
		system("cls");
		cout << std::endl;

		scheduler->Sceduling();

		ChangeTextColor(8);
		scheduler->PrintData();
		ChangeTextColor(15);
		cout << std::endl << std::endl;

		scheduler->SchedulerName();
		cout << std::endl;
		scheduler->DrawGanttChart();
		cout << std::endl;
		scheduler->PrintTable();

		ChangeTextColor(13);
		cout << "\n\n\t\tPress Anykey | Quit: esc" << std::endl;
		ChangeTextColor(15);

		delete scheduler;

		input = _getch();
		
		if (input == 27)	// ESC
			break;
	}
	return 0;
}