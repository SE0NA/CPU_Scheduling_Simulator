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
	int turnaroundTume = 0;
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
	Process* process_head;			// �迭�� ���� �޸� �Ҵ�
	GanttData* gantt_head;	// ���� ����Ʈ�� ���� �޸� �Ҵ�

public:
	Scheduler(){
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
		// ���μ��� ���� ����
		for (int i = 0; i < processCnt; i++) {
			fgets(buf, 256, fp);
			
			token = strtok(buf, "/");	
			process_head[i].PID = atoi(token);
			
			token = strtok(NULL, "/");
			process_head[i].arrivedTime = atoi(token);
			
			token = strtok(NULL, "/");
			process_head[i].burstTime = atoi(token);
			process_head[i].remainTime = process_head[i].burstTime;

			process_head[i].priority = atoi(buf);	// ������ ������
		}
		fclose(fp);
	}
	~Scheduler() {
		free(process_head);
	}

	void PrintData() {
		cout << " +-----+----------+----------+----------+" << std::endl;
		cout << " | PID | �����ð� | ����ð� | �켱���� |" << std::endl;
		cout << " +-----+----------+----------+----------+" << std::endl;

		for (int i = 0; i < processCnt; i++) {
			printf(" | %3d | %8d | %8d | %8d |\n", process_head[i].PID, process_head[i].arrivedTime, process_head[i].burstTime, process_head[i].priority);
		}
		cout << " +-----+----------+----------+----------+" << std::endl;
	}

	void AskTimeSlice() {
		PrintData();

		ChangeTextColor(6);
		cout << " Ÿ�� �����̽� ũ�� >> ";
		cin >> timeSlice;

		system("cls");
	}

	void PrintTable() {
		float sum_wait = 0;
		float sum_response = 0;
		float sum_turnaround = 0;

		// ���ð�, ����ð�, ��ȯ�ð�
		// �� ���μ���
		cout << " +-----+----------+----------+----------+" << std::endl;
		cout << " | PID | ���ð� | ����ð� | ��ȯ�ð� |" << std::endl;
		cout << " +-----+----------+----------+----------+" << std::endl;

		for (int i = 0; i < processCnt; i++) {
			printf(" | %3d | %8d | %8d | %8d |\n", process_head[i].PID, process_head[i].waitingTime, process_head[i].responseTime, process_head[i].turnaroundTume);
			sum_wait += process_head[i].waitingTime;
			sum_response += process_head[i].responseTime;
			sum_turnaround += process_head[i].turnaroundTume;
		}
		cout << " +-----+----------+----------+----------+\n" << std::endl;

		// ���
		cout << " ��� ���ð� AWT: " << sum_wait << " / " << processCnt << " = " << sum_wait / processCnt << std::endl;
		cout << " ��� ����ð� ART: " << sum_response << " / " << processCnt << " = " << sum_response / processCnt << std::endl;
		cout << " ��� ��ȯ�ð� ATT: " << sum_turnaround << " / " << processCnt << " = " << sum_turnaround / processCnt << std::endl;
	}
	void DrawGanttChart() {
		int currentTime = 0;
		int colorValue[] = { 10, 11, 12, 13, 14, 15 };
		int colorLength = 6;

		GanttData* p = gantt_head;
		// |[  P1  ]|[
		// 0        5
		
		cout << "|";
		while (p != NULL) {
			ChangeBackgroundColor(colorValue[p->PID%colorLength]);		// �������� ��Ʈ ǥ��
			for (int i = 0; i < p->runTime; i = i + 2)	cout << " ";
			printf("P%2d", p->PID);
			for (int i = 0; i < p->runTime; i = i + 2)	cout << " ";
			p = p->next;
			ChangeBackgroundColor(0);
			cout << "|";
		}
		
		// �ð� ǥ��
		p = gantt_head;
		ChangeTextColor(15);
		cout << "\n0";
		while (p != NULL) {
			for (int i = 0; i < p->runTime - (p->runTime % 2); i++)	cout << " ";
			currentTime += p->runTime;
			printf("  %2d", currentTime);
			p = p->next;
		}
		cout << std::endl;

		// ���� �Ҵ� �޸� ����
		GanttData* next;
		p = gantt_head;
		while (p != NULL) {
			next = p->next;
			free(p);
			p = next;
		}
	}

	GanttData* InsertGanttNode(GanttData* head,int pid, int time) {
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
		// ������ ������� CPU�� �Ҵ� - ������
		GanttData* p = NULL;

		int currentTime = 0;
		int minTime = 0;
		for (int i = 0; i < processCnt; i++) {
			// minTime ����
			for(int j=0;j<processCnt;j++)
				if (process_head[j].remainTime != 0) {
					minTime = j;
					break;
				}
			
			for (int j = 0; j < processCnt; j++) {
				if (process_head[j].remainTime != 0) {
					if (process_head[j].arrivedTime <= process_head[minTime].arrivedTime) {
						// ���� �ð��� ���� ���� ���μ��� ���ϱ�
						minTime = j;
					}
				}
			}
			process_head[minTime].waitingTime = currentTime - process_head[minTime].arrivedTime;
			process_head[minTime].responseTime = process_head[minTime].waitingTime;	// �� �𸣰ٴ� ����ð��� ��� �ð��� ����!
			currentTime += process_head[minTime].burstTime;
			process_head[minTime].turnaroundTume = currentTime;
			process_head[minTime].remainTime = 0;
			
			p = InsertGanttNode(p, process_head[minTime].PID, process_head[minTime].burstTime);
		}
	}
};

class SJF : public Scheduler {
public:
	void SchedulerName() {
	}
	void Sceduling() {
	}
};
class NP_Priority : public Scheduler {
public:
	void SchedulerName() {
	}
	void Sceduling() {
	}
};
class P_Priority : public Scheduler {
public:
	void SchedulerName() {
	}
	void Sceduling() {

	}
};
class RR : public Scheduler {
public:
	void SchedulerName() {
	}
	void Sceduling() {

	}
};
class SRT : public Scheduler {
public:
	void SchedulerName() {
	}
	void Sceduling() {

	}
};
class HRN : public Scheduler {
public:
	void SchedulerName() {
	}
	void Sceduling() {

	}
};

void PrintMenu(int num) {
	int basicColor = 15;	

	char schedulerName[7][20] = { "FCFS      ", "SJF       ", "���� Priority  ",
							      "���� Priority    " , "RR     ", "SRT     ", "HRN" };

	cout << "\n\t\t  20194056 �̼��� " << std::endl;
	cout << "\t\t+===============+" << std::endl;
	ChangeTextColor(11);
	cout << "\t\t  CPU Scheduler" << std::endl;
	ChangeTextColor(basicColor);
	cout << "\t\t+===============+\n\n" << std::endl;

	for (int i = 0; i < 7; i++) {
		if (i == 0 || i == 3)	cout << std::endl << std::endl << "       ";
		if (i == num) {
			ChangeTextColor(6);
		}
		cout << schedulerName[i];
		if (i == num) {
			ChangeTextColor(basicColor);
		}	
	}
	ChangeTextColor(8);
	cout << "\n\n\t\t Press �� �� Enter key" << std::endl;
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

	while (true) {
		while (true) {	// �����ٷ� ����
			system("cls");
			PrintMenu(selectMenu);
			input = _getch();

			if (input == 75) {		// ����
				selectMenu = (selectMenu + 6) % 7;
			}
			else if (input == 77) {	// ������
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

		system("cls");
		ChangeTextColor(8);
		scheduler->PrintData();
		cout << std::endl;

		scheduler->Sceduling();

		scheduler->SchedulerName();

		scheduler->PrintTable();
		cout << std::endl;

		scheduler->DrawGanttChart();
		

		ChangeTextColor(13);
		cout << "\n\n\t\tMenu: 'z', Quit: esc" << std::endl;
		ChangeTextColor(15);

		delete scheduler;

		input = _getch();
		if (input == 'z')
			continue;
		else if (input == 27)
			break;
	}	
}