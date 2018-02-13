#include "shell.h"



void RunShell()
{
	int running = 1;
	char* str;
	char** argv;
	Init();
	while (running)
	{
		KillZombies();

		checkQueue();
		PrintPrompt();
		str = ReadInput();
		argv = ParseInput(str);
		
		if ((argv[0] == NULL) || CheckForIOandPipeErrors(argv) || CheckForBackgroundErrors(argv))
		{
			// do nothing if empty arguments
		}
		else if (strcmp(argv[0], "exit") == 0)
		{
			MemFunc(argv);
			printf("Exiting Shell...\n");
			exitQueue();
			exit(0);
		}
		else if (strcmp(argv[0], "cd") == 0)
		{


			if (GetSize(argv) <= 2)
			{
				if (GetSize(argv) == 2){
					ChangeDirectory(argv[1]);
				}
				else
					ChangeDirectory(getenv("HOME"));

			}

			else
			{
				printf("Too many arguments...\n");
			}



		}


		else if (strcmp(argv[0], "io") == 0)
		{
			if (GetSize(argv) > 1)
			{
				argv = RemoveArr(argv, 0);
				Limits(argv);
			}

		}
		else if (strcmp(argv[0], "etime") == 0)
		{
			printf("HHHH\n");
			if (GetSize(argv) > 1)
			{
				argv = RemoveArr(argv, 0);
				ETime(argv);
			}
		}
		else if (ExecCheck(argv[0]))
		{
			int background = StringCheck(argv, "&");
			int I_loc = StringCheck(argv, "<");
			int O_loc = StringCheck(argv, ">");
			int pipe_count = CountStr(argv, "|");
			if (I_loc != -1)
			{
				argv = ExecuteExternalWithInput(argv, I_loc, background);
			}
			else if (O_loc != -1)
			{
				argv = ExecuteExternalWithOutput(argv, O_loc, background);
			}
			else if (pipe_count > 0)
			{
				argv = ExecuteExternalWithPipe(argv, pipe_count, background);
			}
			else
			{
				char* cmd = Convert(argv);
				if (background != -1)
				{
					argv = RemoveArr(argv, background);
				}
				ExecuteExternal(argv, background, cmd);
				free(cmd);
			}
		}


		
		MemFunc(argv);
	}
}

void ChangeDirectory(const char* dir)
{

	int ret = chdir(dir);
	switch(ret){
	case 0:
	{
		setenv("PWD", dir, 1);
		break;
	}
	default:
	{
		printf("No such directory\n");
		break;
	}
	}

}



void ExecuteExternal(char** argv, int background, char* cmd)
{
	int status;
	pid_t pid = fork();
	switch(pid){
	case -1:
	{
		printf("Error on fork\n");
		exit(1);
		break;
	}
	case 0:
	{
		execv(argv[0], argv);
		printf("Trouble executing: \n");
		DisplayArgs(argv);
		exit(1);
		break;
	}
	default:
	{
		switch(background){
		default:
		{
			waitpid(pid, &status, WNOHANG);
			handleQueue(newPro(pid, -1, cmd));
			//kill(pid, SIGSTOP);
			//Need to implement a pause here for things like text editors
			break;
		}
		case -1:
		{
			waitpid(pid, &status, 0);
			break;
		}
		}
		break;
	}
	}
}

char** ExecuteExternalWithInput(char** argv, int I_loc, int background)
{
	char* filename = (char*)calloc(strlen(argv[I_loc+1])+1, sizeof(char));
	strcpy(filename, argv[I_loc+1]);

	argv = RemoveArr(argv, I_loc);
	argv = RemoveArr(argv, I_loc);
				
	// update background iterator
	background = StringCheck(argv, "&");
	char* cmd = Convert(argv);
	if (background != -1)
	{
		argv = RemoveArr(argv, background);
	}
	IORedirect(argv, 1, filename, background, cmd);
	free(filename);
	free(cmd);
	
	return argv;
}

char** ExecuteExternalWithOutput(char** argv, int O_loc, int background)
{
	char* filename = (char*)calloc(strlen(argv[O_loc+1])+1, sizeof(char));
	strcpy(filename, argv[O_loc+1]);

	argv = RemoveArr(argv, O_loc);
	argv = RemoveArr(argv, O_loc);
			
	// update background iterator
	background = StringCheck(argv, "&");
	char* cmd = Convert(argv);
	if (background != -1)
	{
		argv = RemoveArr(argv, background);
	}
	IORedirect(argv, 0, filename, background, cmd);
	free(filename);
	free(cmd);
	
	return argv;
}

char** ExecuteExternalWithPipe(char** argv, int pipe_count, int background)
{
	char* cmd = Convert(argv);
	if (background != -1)
	{
		argv = RemoveArr(argv, background);
	}

	if (pipe_count == 1 || pipe_count == 2 || pipe_count == 3)
{
    char** argv1 = (char**)calloc(1, sizeof(char*));
    char** argv2 = (char**)calloc(1, sizeof(char*));
    char** argv3 = (char**)calloc(1, sizeof(char*));
    char** argv4 = (char**)calloc(1, sizeof(char*));
    argv1[0] = NULL;
    argv2[0] = NULL;
    argv3[0] = NULL;
    argv4[0] = NULL;
    int it = 0;
    while(strcmp(argv[it], "|") != 0)
    {
        argv1 = PBackArr(argv1, argv[it]);
        ++it;
    }
    ++it;
        if (pipe_count == 1)
        {
            while(argv[it] != NULL)
            {
                argv2 = PBackArr(argv2, argv[it]);
                ++it;
            }
            OnePipe(argv1, argv2, background, cmd);
            MemFunc(argv1);
            MemFunc(argv2);
        }
        else if (pipe_count == 2 )
        {
            while(strcmp(argv[it], "|") != 0)
            {
                argv2 = PBackArr(argv2, argv[it]);
                ++it;
            }
            ++it;
            while(argv[it] != NULL)
            {
                argv3 = PBackArr(argv3, argv[it]);
                ++it;
            }
            TwoPipe(argv1, argv2, argv3, background, cmd);
            MemFunc(argv1);
            MemFunc(argv2);
            MemFunc(argv3);
        }
        else if (pipe_count == 3)
        {
            while(strcmp(argv[it], "|") != 0)
            {
                argv2 = PBackArr(argv2, argv[it]);
                ++it;
            }
            ++it;
            while(strcmp(argv[it], "|") != 0)
            {
                argv3 = PBackArr(argv3, argv[it]);
                ++it;
            }
            ++it;
            while(argv[it] != NULL)
            {
                argv4 = PBackArr(argv4, argv[it]);
                ++it;
            }
            ThreePipe(argv1, argv2, argv3, argv4, background, cmd);
            MemFunc(argv1);
            MemFunc(argv2);
            MemFunc(argv3);
            MemFunc(argv4);
        }
}	
	free(cmd);
	return argv;





}

void IORedirect(char** argv, int dir, char* filename, int background, char* cmd)
{
	// output redirection
	switch(dir){
	case 0:
	{
		int status;
		int fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, 0777);
		if (fd == -1)
		{
			printf("Error. Cannot open filename: %s\n", filename);
			exit(1);
		}
		pid_t pid = fork();

		switch(checkZero(pid)){
		case 2:
		{
			close(1);
			dup(fd);
			close(fd);
			execv(argv[0], argv);
			printf("Unknown command in IORedirect\n");
			exit(1);
			break;
		}
		case 1:
		{
			switch(background){
			default:
			{
				waitpid(pid, &status, WNOHANG);
				handleQueue(newPro(pid, -1, cmd));
				break;
			}
			case -1:
			{
				waitpid(pid, &status, 0);
				break;
			}
			}
			close(fd);
			break;
		}
		case 0:
		{
			printf("Fork failed in IORedirect\n");
			exit(1);
			break;
		}
		}
		break;
	}
	case 1:
	{
		int status;
		int fd = open(filename, O_RDONLY);
		switch(fd){
              	  case -1:
		  {
			printf("Error, cannot access file in IORedirect!\n");
			exit(1);
			break;
		  }
		}
		pid_t pid = fork();
		
		switch(checkZero(pid)){
		case 2:
		{
			close(0);
			dup(fd);
			close(fd);
			execv(argv[0], argv);

			printf("Unknown command in IORedirect\n");
			exit(1);
			break;
		}
		case 1:
		{
		
			switch(background){
			default:
			{
				waitpid(pid, &status, WNOHANG);
				handleQueue(newPro(pid, -1, cmd));
				break;
			}
			case -1:
			{
				waitpid(pid, &status, 0);
				break;
			}
			}
			close(fd);
			break;
		}
		case 0:
		{
			printf("Fork() failed in IORedirect\n");
			exit(1);
			break;
		}
		}
		break;
	}
	}
}

int CheckForIOandPipeErrors(char** argv)
{
	
	switch(GetSize(argv)){
	case 1:
	{
		if ((StringCheck(argv, "<") != -1) ||
			(StringCheck(argv, ">") != -1) ||
			(StringCheck(argv, "|") != -1))
		{
			printf("Requires more than 1 argument\n");
			return 1;
		}
		break;
	}
	default:
	{
		if ((StringCheck(argv, "<") == 0) ||
			(StringCheck(argv, ">") == 0) ||
			(StringCheck(argv, "|") == 0))
		{
			printf("Error, IO redirection or pipe cannot appear at beginning of statement\n");
			return 1;
		}
		else if ((StringCheck(argv, "<") == GetSize(argv)-1) ||
				 (StringCheck(argv, ">") == GetSize(argv)-1) ||
				 (StringCheck(argv, "|") == GetSize(argv)-1))
		{
			printf("Error, IO redirection or pipe cannot appear at end of statement\n");
			return 1;
		}
		break;
	}
	}
	return 0;
}

int CheckForBackgroundErrors(char** argv)
{
	int I_loc = StringCheck(argv, "<");
	int O_loc = StringCheck(argv, ">");
	int P_loc = StringCheck(argv, "|");
	int B_loc = StringCheck(argv, "&");
	
	if (B_loc == -1)
	{
		return 0;
	}
	
	// if not < > or |, no error
	if ((I_loc == -1) &&
		(O_loc == -1) &&
		(P_loc == -1))
	{
		return 0;
	}
	else
	{
		if (I_loc != -1)
		{
			// & neighbors <
			if (I_loc - 1 == B_loc || I_loc + 1 == B_loc)
			{
				printf("Invalid background processing format\n");
				return 1;
			}
		}
		if (O_loc != -1)
		{
			// & neighbors >
			if (O_loc - 1 == B_loc || O_loc + 1 == B_loc)
			{
				printf("Invalid background processing format\n");
				return 1;
			}
		}
		if (P_loc != -1)
		{
			// & neighbors |
			if (P_loc - 1 == B_loc || P_loc + 1 == B_loc)
			{
				printf("Invalid background processing format\n");
				return 1;
			}
		}
	}
	return 0;
}

void PrintPrompt()
{
	const char* directory = "PWD";
	const char* machine = "MACHINE";
	const char* user = "USER";    
	char *machine_out;
	char *user_out;
	char *directory_out;
				  
	machine_out = getenv(machine);
	user_out = getenv(user);
	directory_out = getenv(directory);			 

	printf("%s@%s: %s => ", user_out, machine_out, directory_out);

}


void Limits(char** argv)
{
	int childID;
	int status;

	int line_count = 0;

	char file[256] = "/proc/";
	char PID[256];
	char location[256] = "/io";
	char limit_string[256];

	pid_t childPID; 
	childPID = fork();

	switch(checkZero(childPID)){
	case 2:
	{
		execv(argv[0], argv);
		printf("Trouble executing: \n");
		DisplayArgs(argv);
		exit(1);
		break;
	}
	case 1:
	{
		waitpid(childPID, &status, 0);
		childID = getpid();
		sprintf(PID, "%i", childID);
		strcat(file, PID);
		strcat(file, location);
		FILE* limit_file;
		limit_file = fopen(file, "r");

		while(fgets(limit_string, sizeof(limit_string), limit_file))
		{
			if ((line_count == 3) || (line_count == 7) ||
			    (line_count == 8) || (line_count == 12))
			    {
				printf("%s", limit_string);
			    }
			line_count++;
		}
		fclose(limit_file);
		break;
	}
	case 0:
	{
		printf("Fork failed in Limits()\n");
		exit(1);
		break;
	}
	}
}


/*
void ioacct( pid_t child ) {

    char filename[100];
    char test[100];

    int tempBytes = -1;
    int readBytes = -1;
    int writeBytes = -1;
    pid_t child_finished = -999;            // pid is never going to be this

    sprintf(filename, "/proc/%d/io", (int)child);
    FILE* proc_file = fopen(filename, "r"); // open the proc file for eading

    while ((child != child_finished)) {

        //read from the file
        int count = 0;
        while (fscanf(proc_file, "%s %d", test, &tempBytes) > 0) {
            if (count == 4) {
                readBytes = tempBytes;
            }
            if (count == 5) {
                writeBytes = tempBytes;
            }
            ++count;
        }
        child_finished = waitpid(-1, (int *)NULL, 0);
    }
    printf("\nBytes Read: %d\nBytes Written: %d\n", readBytes, writeBytes);
}

*/


void ETime(char** argv)
{
	int status;
	struct timeval timeofday;
	gettimeofday(&timeofday, NULL);
	double beginning_time=timeofday.tv_sec+(timeofday.tv_usec/1000000.0);

	pid_t childPID = fork();

	switch(checkZero(childPID)){
	  case 2:
	  {
		execv(argv[0], argv);
                printf("Trouble executing: \n");
                DisplayArgs(argv);
                exit(1);
		break;
	  }
	  case 1:
		waitpid(childPID, &status, 0);
		break;
	  case 0:
		printf("Fork failed in ETime\n");
		break;
	}


	gettimeofday(&timeofday, NULL);
	double finishing_time=timeofday.tv_sec+(timeofday.tv_usec/1000000.0);
	printf("Elapsed Time: %f\n", finishing_time-beginning_time);
}

int checkZero(int tocheck)
{
	if (tocheck == 0)
		return 2;
	else if (tocheck > 0)
		return 1;
	else
		return 0;
}

void KillZombies() {

    pid_t pid;

    // Kill processes as long as we keep finding them
    while ( ( pid = waitpid( -1, 0, WNOHANG ) )  )
        
        // No zombie processes are found
        if ( pid == -1 || pid == 0 )
            break;

}
