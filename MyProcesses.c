#include "MyProcesses.h"

extern struct beginPro* process_queue[];

void Init()
{
	int i = 0;
	while(i < PQUEUE_SIZE)
	{
		process_queue[i] = NULL;
		i++;
	}
	

}

void handleQueue(struct beginPro* p)
{
	int i = 0;
	// find first available slot
	while(i < PQUEUE_SIZE)
	{
		
		if(checkNull(process_queue[i]))
                {
			
                        process_queue[i] = p;
                        displayProStart(process_queue[i], i);
			
                        break;
			
                }

		i++;
	}
}

void checkQueue()
{
	int i = 0;
	while(i < PQUEUE_SIZE)
	{
		if(!checkNull(process_queue[i]))
                {
                        if (checkPro(process_queue[i]))
                        {
                                remPro(i);
                        }
                }
		i++;
	}
}

void exitQueue()
{
	int processes_running = 1;
	while(processes_running)
	{
		checkQueue();
		int i = 0;
		while(i < PQUEUE_SIZE)
		{
			if (!checkNull(process_queue[i]))
                        {
                                processes_running = 1;
                                break;
                        }
			i++;
		}

		switch(i){
		  case PQUEUE_SIZE:
			processes_running = 0;
			break;
		}


	}
}

void remPro(int index)
{
	
	if (index >= 0)
	{
	  if( index < PQUEUE_SIZE)
		if (!checkNull(process_queue[index]))
		{
			displayProDone(process_queue[index], index);
			spacePro(process_queue[index]);
			process_queue[index] = NULL;
			
			
		}
	}
}

struct beginPro* newPro(int pid_1, int pid_2, char* cmd)
{
	struct beginPro* ans = (struct beginPro*)calloc(1, sizeof(struct beginPro));
	ans->pid_1 = pid_1;
	ans->pid_2 = pid_2;
	ans->cmd = (char*)calloc(strlen(cmd)+1,sizeof(char));
	strcpy(ans->cmd, cmd);
	return ans;
}

void spacePro(struct beginPro* p)
{
	free(p->cmd);
	free(p);
}

int checkPro(struct beginPro* p)
{
	int status;
	int waitpidAns;
	if (p->pid_2 != -1)
	{
		waitpidAns = waitpid(p->pid_2, &status, WNOHANG);
		
		switch(waitpidAns){
		  case 0:
			return 0;
			break;
		  case -1:
			printf("Error in child process: %s\n", p->cmd);
			printf("1\n");
			break;
		}

	}
	waitpidAns = waitpid(p->pid_1, &status, WNOHANG);

	switch(waitpidAns){
	  case 0:
	  	return 0;
		break;
	  case -1:
		printf("Error in child process: %s\n", p->cmd);
		printf("2\n");
		break;
	  default:
		return 1;
		break;
	}


	return 1;	// fix compiler warning
}

void displayProStart(const struct beginPro* p, int pos)
{
	if (!checkNull(p))
	{
		
		if (p->pid_2 == -1)
			printf("[%i]\t[%i]\n", pos, p->pid_1);
		else
			printf("[%i]\t[%i][%i]\n", pos, p->pid_1, p->pid_2);
	}
}

void displayProDone(const struct beginPro* p, int pos)
{
	if (!checkNull(p))
	{
		printf("[%i]+\t[%s]\n", pos, p->cmd);
	}
}

bool checkNull(const struct beginPro* tocheck)
{

	if(tocheck == NULL)
		return true;
	else
		return false;
}

