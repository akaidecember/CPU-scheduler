/*
Assignment 2 (CSC 139 OS) - Resubmission
CPU scheduler for FCFS, RR and SRTF
@Anshul Kumar

This program simulates the CPU process scheduling algorithm for the given processes
*/

//Header Files
#include<stdio.h>
#include<stdlib.h>

//structure for storing the process information
//also represents a linked list for traversing and storing data
typedef struct Node{

    long pid;                                               //Process ID
    long arrival;                                           //arrival time for the process
    long burst;                                             //Burst time for the process
    struct Node *next;                                      //Next Node to point to the next process in the queue (using a linked list)

} Node;

//This function will return the number of processes
int processCount(char *fileName){

	FILE *fp;                                               //FILE pointer
    char c;
	int procNum = 0;
    fp = fopen(fileName, "r");                              //Open the file
    if(fp == NULL)
        exit(1);
    while (c != EOF){
    	c = getc(fp);
        if (c == '\n') 										// Increment count if this character is newline
            procNum++;
    }

    fclose(fp);
	return procNum;                                         //return the number of processes

}

//Function to insert if linked list empty
Node *empty(Node *head, Node *node){

	head = node;
	head -> next = head;
	return head;

}

//Function to insert the node (as a queue)
Node *insertProcess(Node *head, Node *node){

	node -> next = head -> next;
	head -> next = node;
	head = node;
	return head;

}

//Function to search a sutiable position for the node based on arrival time
Node *search(Node *head, Node *node){

	Node *temp = head -> next;
	while((temp -> next -> arrival < node -> arrival) && (temp -> next != head -> next))
		temp = temp -> next;
	node -> next = temp -> next;
	temp -> next = node;
	return head;
}

//Function to ge tthe node and insert them in a sorted manner
Node *sortNode(Node *head, Node *node){

	if(head == NULL)                                                //If the list is empty
		return empty(head, node);
	else if(node -> arrival >= head -> arrival)                     //Sorting is done based on the arrival times of the process
		return insertProcess(head, node);
	else
		return search(head, node);                                  //Search for a sutiable spot

}

//Function to get the data from the file in proper manner
Node *getDataFromFile(char *fileName, Node *head){

	FILE *fp;
	Node *returnNode = NULL;
	int procNum = processCount(fileName), i = 0;                    //Number of processes
	long arr = 0, bur = 0, pid = 0;
	char buffer[100];                                               //buffer to read the lines from the file

	fp = fopen(fileName, "r");
	if(fp == NULL)                                                  //error in file opening operation
			exit(1);

	//For each process available
	for(i = 0; i < procNum; i++){

		Node* node = (Node *)malloc(sizeof(Node));
		if(node == NULL)
			return NULL;

		fgets(buffer, sizeof(buffer), fp);
		sscanf(buffer, "%ld\t%ld\t%ld\n", &pid, &arr, &bur);        //scan file for pid, arrival and burst times

		//Populating the node
		node -> pid = pid;
		node -> arrival = arr;
		node -> burst = bur;
		node -> next = (Node*)malloc(sizeof(Node));

		returnNode = sortNode(returnNode, node);                    //return the populated node

	}

	printf("\nTotal %d tasks are read from \"%s\"", procNum, fileName);
	return returnNode;

}

//Function to remove the given process from the queue once completed
Node *removeCompletedProcess(Node *head){

	Node *temp = head -> next;
	head -> next = head -> next -> next;
	free(temp);

}

//Function to simulate FCFS algorithm
void FCFS(Node *head, char *file){

	int sysTime = 0, procNum = processCount(file), i = 0;
	Node *temp = head -> next;
	double turn = 0, cpu = 0, wait = 0, resp = 0, turnAvg = 0, cpuAvg = 0, waitAvg = 0, respAvg = 0;

	do{

        //execute each process till exhaustion
		while(temp -> burst != 0){

			printf("<System time\t%d> process %ld is running\n", sysTime++, temp -> pid);
			temp -> burst--;

		}

        //Calculating the turn around time
        turn = sysTime - (double)(temp -> arrival);

        //Calculate the waiting time
        wait = turn - (double)(temp -> burst);

        //Calculate the response time
        resp = (sysTime - (double)(temp -> burst)) - (double)(temp -> arrival);

        //Calculate the average times
        turnAvg += turn;
        waitAvg += wait;
        respAvg += resp;

		printf("<System time\t%d> process %ld is finished..........\n", sysTime, (temp -> pid));

		//Go to the next process in the queue
		temp = temp -> next;

	}while(temp != head -> next);                                       //repeat till reach the head again

	printf("<System time\t%d> All process finished............................\n", sysTime);
	printf("=======================================================================================================\n");
	printf("CPU Usage: \n");
	printf("Average Wait Time: %f\n", waitAvg /(double)procNum);
	printf("Average Response Time: %f\n", respAvg / (double)procNum);
	printf("Average Turnaround time: %f\n", turnAvg / (double)procNum);

}

//Function to simulate the RR algorithm
void RR(Node *head, int quantum, char *file){

	int procNum = processCount(file), sysTime = 0, i = 0;
	Node *temp1, *temp2;
	double turn = 0, cpu = 0, wait = 0, resp = 0, turnAvg = 0, cpuAvg = 0, waitAvg = 0, respAvg = 0;

	do{

		temp1 = head -> next;

		//If the burst of the current process is less than the given time quantum, execute till exhaustion
		if(temp1 -> burst <= quantum){

			for(i = 0; i < (temp1 -> burst); i++)
				printf("<System time\t%d> process %ld is running\n", sysTime++, temp1 -> pid);

			temp1 -> burst = 0;
			printf("<System time\t%d> process %ld is finished..........\n", sysTime, (temp1 -> pid));

            //Calculate the turn around time
            turn = sysTime - (double)(temp1 -> arrival);

            //Calculate the wait time
            wait = turn - (double)(temp1 -> burst);

            //Calculate average times
             turnAvg += turn;
             waitAvg += wait;

			//if process completed, remove it from the queue
			removeCompletedProcess(head);
			procNum--;

		}
		else{		//If the burst is more than quantum, run till quantum amount

			for(i = 0; i < procNum; i++){

				printf("<System time\t%d> process %ld is running\n", sysTime++, temp1 -> pid);
				temp1 -> burst--;

			}

            //Switch to next process in ready queue provided it's arrival time is less than the system time
			while((temp1 -> next -> arrival) <= sysTime){

				temp1 = temp1 -> next;
				if(temp1 == head){

					head = temp1 -> next;                               //set the current element to the next possible process and get out of loop
					break;

				}

			}

			if(head == temp1 -> next)
				continue;

			temp2 = temp1 -> next;
			temp1 -> next = head -> next;
			head -> next = head -> next -> next;
			temp1 -> next -> next = temp2;

		}
	}while(procNum != 0);

	printf("<System time\t%d> All process finished............................\n", sysTime);
	printf("=======================================================================================================\n");
    printf("CPU Usage: \n");
	printf("Average Wait Time: %f\n", waitAvg /(double)procNum);
	printf("Average Response Time: \n");
	printf("Average Turnaround time: %f\n", turnAvg / (double)procNum);

}

//Function to simulate the SRTF algorithm
void SRTF(Node *head, char *file){

	int sysTime = 0, procNum = processCount(file);
    double turn = 0, cpu = 0, wait = 0, resp = 0, turnAvg = 0, cpuAvg = 0, waitAvg = 0, respAvg = 0;
	Node *temp1, *temp2, *temp3;

	do{

		temp1 = head -> next;

		while((temp1 -> burst) != 0){                                   //While the current process  is not exhausted

			temp2 = temp1 -> next;
			while(temp2 != temp1){

                //compare two consecutive processes in the queue
				if((temp2 -> burst < temp1 -> burst
					&& temp2 -> arrival <= sysTime)
					|| (temp2 -> burst == temp1 -> burst
					&& temp2 -> pid < temp1 -> pid
					&& temp2 -> arrival < sysTime)){

						if(head -> arrival <= sysTime
							&& head -> burst < temp1 -> burst)
							head = temp1;
						else if(temp2 -> burst == temp1 -> burst
							&& temp2 -> pid < temp1 -> pid
							&& temp2 -> arrival < sysTime){

								temp3 = temp1;

								while(temp3 -> next != temp2)
									temp3 = temp3 -> next;

								temp3 -> next = temp2 -> next;
								temp2 -> next = temp1;
								temp1  = temp2;
								head -> next = temp2;

						}
						else{

							temp1 -> next = head;
							head -> next = temp2;
							temp3 = temp2;

							while(temp3 -> next != head)
								temp3 = temp3 -> next;

							temp3 -> next = temp1;

						}

						temp1 = temp2;
						temp2 = temp1 -> next;

				}
				else
					temp2 = temp2 -> next;

			}

			temp1 -> burst--;
			printf("<System time\t%d> process %ld is running\n", sysTime++, temp1 -> pid);

		}

		printf("<System time\t%d> process %ld is finished..........\n", sysTime, (temp1 -> pid));

        //Calculate the turn around time
        turn = sysTime - (double)(temp1 -> arrival);

        //Calculate the wait time
        wait = turn - (double)(temp1 -> burst);

        //Calculate average times
        turnAvg += turn;
        waitAvg += wait;

		removeCompletedProcess(head);

	}while(temp1 != (head -> next));

	printf("<System time\t%d> All process finished............................\n", sysTime);
	printf("=======================================================================================================\n");
    printf("CPU Usage: \n");
	printf("Average Wait Time: %f\n", waitAvg /(double)procNum);
	printf("Average Response Time: \n");
	printf("Average Turnaround time: %f\n", turnAvg / (double)procNum);

}

//Main function of the program
int main(int argc, char *argv[]){

    setbuf (stdout, NULL);
	char *fileName;
	int type = 0, quantum = 0;					//1 FCFS, 2 RR, 3 SRTF
	Node *head = NULL;

	if (argc < 2){

        printf("Error. FILE name missing !!!");
		exit(0);

	}
    else if(argc == 2){

        printf("Error. Scheduling type missing !!!");
		exit(0);

	}
    else{

		fileName = argv[1];
		if(!(strcmp(argv[2],"FCFS"))){

			type = 1;
			printf("Scheduling Algorithm: FCFS");

		}
        else if(!(strcmp(argv[2], "RR"))){

            if(argc != 4){

                printf("Error. Time quantum not given !!!");
				exit(0);

			}
            else{

				quantum = atoi(argv[3]);
				printf("Scheduling Algorithm: RR");
				type = 2;

			}

        }
        else if(!(strcmp(argv[2], "SRTF"))){

            printf("Scheduling Algorithm: SRTF");
			type = 3;

        }

	}

	head = getDataFromFile(fileName, head);				//Populate the queue with the processes and return the head of the queue

	printf("\n=======================================================================================================\n");

	switch(type){

		case 1: FCFS(head, fileName);                   //FCFS algo
				break;
		case 2: RR(head, quantum, fileName);            //RR algo
				break;
		case 3: SRTF(head, fileName);                   //SRTF algo
				break;
		default: exit(1);
				break;

	}

}
