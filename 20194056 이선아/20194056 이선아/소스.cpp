#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <Windows.h>

using namespace std;

void PrintMenu(int);
void ChangeTextColor(int);

struct Process {
	int PID;
	int priority;
	int arrivedTime;
	int waitingTime;
	int reponseTime;
	int burstTime;
	int turnaroundTume;
	int remainTime;
};

class Scheduler {
protected:
	int timeSlice;
	int processCnt;
	int sum_wait;
	int sum_reponse;
	int sum_turnaround;
	Process* head;
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
		head = (Process*)malloc(processCnt * sizeof(Process));

		rewind(fp);
		char* token;
		// 프로세스 정보 저장
		for (int i = 0; i < processCnt; i++) {
			fgets(buf, 256, fp);
			
			token = strtok(buf, "/");	
			head[i].PID = atoi(token);
			
			token = strtok(NULL, "/");
			head[i].arrivedTime = atoi(token);
			
			token = strtok(NULL, "/");
			head[i].burstTime = atoi(token);

			head[i].priority = atoi(buf);	// 마지막 데이터
		}
		fclose(fp);

		sum_wait = 0;
		sum_reponse = 0;
		sum_turnaround = 0;
	}
	~Scheduler() {
		free(head);
	}

	int GetTimeSlice() {
		return timeSlice;
	}
	void SetTimeSlice(int value) {
		timeSlice = value;
	}

	void PrintData() {
		cout << " +-----+----------+----------+----------+" << std::endl;
		cout << " | PID | 도착시간 | 실행시간 | 우선순위 |" << std::endl;
		cout << " +-----+----------+----------+----------+" << std::endl;

		for (int i = 0; i < processCnt; i++) {
			printf(" | %3d | %8d | %8d | %8d |\n", head[i].PID, head[i].arrivedTime, head[i].burstTime, head[i].priority);
		}
		cout << " +-----+----------+----------+----------+" << std::endl;
	}

	virtual void PrintResult() = 0;
};

class FCFS : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ FCFS ]" << std::endl;
		ChangeTextColor(15);

		
	}
};
class SJF : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ SJF ]" << std::endl;
		ChangeTextColor(15);

	}
};
class NP_Priority : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ 비선점 Priority ]" << std::endl;
		ChangeTextColor(15);

	}
};
class P_Priority : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ 선점 Priority ]" << std::endl;
		ChangeTextColor(15);

	}
};
class RR : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ RR ]" << std::endl;
		ChangeTextColor(15);

	}
};
class SRT : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ SRT ]" << std::endl;
		ChangeTextColor(15);

	}
};
class HRN : public Scheduler {
public:
	void PrintResult() {
		ChangeTextColor(11);
		cout << " [ HRN ]" << std::endl;
		ChangeTextColor(15);

	}
};

void PrintMenu(int num) {
	int basicColor = 15;	

	char schedulerName[7][20] = { "FCFS      ", "SJF       ", "비선점 Priority  ",
							      "선점 Priority    " , "RR     ", "SRT     ", "HRN" };

	cout << "\n\t\t  20194056 이선아 " << std::endl;
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
	cout << "\n\n\t\t Press ← → Enter key" << std::endl;
	ChangeTextColor(basicColor);
}
void ChangeTextColor(int fg) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, fg);
}

int main() {
	int selectMenu = 0;
	int input;

	while (true) {
		while (true) {	// 스케줄러 선택
			system("cls");
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

		system("cls");
		scheduler->PrintData();
		cout << "\n" << std::endl;
		cout << "타임 슬라이스 크기 >>";
		cin >> input;
		scheduler->SetTimeSlice(input);
		
		system("cls");

		ChangeTextColor(8);
		scheduler->PrintData();
		cout << "  time slice: " << scheduler->GetTimeSlice() << std::endl;
		ChangeTextColor(15);

		scheduler->PrintResult();

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