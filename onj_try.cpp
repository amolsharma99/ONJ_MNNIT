/*
Secure online programming judge.
Author: T3rmianl
running this program requires root privilages.
"jail" folder will contain a folder named "problems" having subfolders 1,2,3...
...each having only sample input file named "in". The sample output file will be outside the jail folder for security reasons.
Currenty the main folder(onj) have subfolder "problems" containing subfolder 1,2,3 with sample outputfile.
Output-file generated by the program is named main_out and stored in same subfolded 1,2,3.....
*/



/*					ONJ
            	__________________________|_____________________________________
		|						|
	   Problems					       	_________jail____	
	_______|_______                                         |		|	
	|    |    |    |				     Problems	      compiled a.out 
	1    2    3    4				________|________
	|    |    |    |				|    |     |     |
	out  out  out  out 				
* output file generated by program "main_out" will be in same folder as "in".
*/		
#include <iostream>
#include <cstdio>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/wait.h>
#include <signal.h>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/reg.h>
#include <sys/time.h>
using namespace std;
#define NAME_LEN 50
#define NO_OF_ARGS 4
#define MAIN_FILE 1 //command line argument no for input file
#define PROB_NO 2
#define TIME_LIMIT 3
#define USERNAME 4
#define RIGHT 0
#define WRONG 2
#define TIME_EXCEEDED 3
#define COMPILE_ERROR 1
#define ILLEGAL_FILE 4
#define RUN_TIME_ERROR 5
#define PRESENTATION_ERROR 6
#define PRESENTATION	//for enabling the detection of presentation error
#define EXECUTION_ERROR 7
/*
configuration settings
A seperate file is a clean idea but tracing at runtime is already slow, so 
settings are hard-coded in main file only
*/
/*
comment this line for faster response at cost of security i.e no runtime checking of functions
*/
#define SECURE
void check_arguments(int);
void check_file();
void parse_file();
void compile();
void set_environment();
void execute();
void compare();
void register_timeout(double, void (*)(int));
void time_out(int );
void print_total_time();
void log_init(char *username);
string getpath(string );
string getext(string );
string exe="exe"; //name of a.out file generated
string err[]={"right","compile err","wrong","time excd","illegal file","run time error","presentation error","execution error"};
char restricted_owner[]="piyush";
char main_file[NAME_LEN],problem[3],username[NAME_LEN];
double time_limit;
pid_t pid;
int main(int argc,char *argv[])
{
	/*
	sample invokation:
	sudo ./a.out main_file 2(problem no) 4(time limit)
	*/	

	/*
	copy the input n outputfile name
	which are global to file.
	To Do:Check for buffer overflow here
	*/
	cout<<ORIG_EAX;
	strcpy(main_file,argv[MAIN_FILE]);
	strcpy(problem,argv[PROB_NO]);
	strcpy(username,argv[USERNAME]);
	time_limit=atof(argv[TIME_LIMIT]);
	log_init(username);
	check_arguments(argc);
	check_file();
	parse_file();
	compile();
	execute();
	compare();
}

void time_out(int i)
{
	if(kill(pid,SIGKILL)==-1)
	{
		perror("Error kill:");
		exit(EXECUTION_ERROR);
	}
	cerr<<err[TIME_EXCEEDED]<<endl;
	exit(TIME_EXCEEDED);
}
void check_arguments(int argc)
{
	/*
	Check if proper no of arguments are supplied..blah blah..
	*/
	/*
	EXTENSION:
	check for debug option at command line.
	*/ 

}


string getpath(string s)
{
	int i = s.find_last_of('/');
	if(i != string::npos)
		return s.substr(0, i);
	return "";
}

string getext(string s)
{
	int i = s.find_last_of('.');
	if(i != string::npos)
		return s.substr(i+1);
	return "";
}
void check_file()
{
	/*
	Check extension of file and store path
	*/
	string ext = getext(main_file);
	string path = getpath(main_file);
	if(ext=="" || path=="" || (ext!="cpp" && ext!="c" && ext!="CPP" && ext!="C"))
	{
		cerr<<err[ILLEGAL_FILE]<<endl;
		exit(ILLEGAL_FILE);
	}
	else
	{
	/*
	if u wish to check for c or c++
	*/	
	}
	/*
	EXTENSION:
	max len of file name can be cheked
	*/ 

}

void parse_file()
{
	/*
	Parse file to check for any illegal words except system calls.
	System calls will be checked at run-time.
	*/
}

void compile()
{
	/*
	Compile file and save "exe" in jail (restricted folder).
	*/
	string flag=" ";
	string path_out="./jail/";
	string cmd = "g++ -lm -static"+flag+string(main_file)+" -o "+path_out+exe;
	if(system(cmd.c_str()))	//system return 0 on success
	{
		cerr<<err[COMPILE_ERROR];
		exit(COMPILE_ERROR);
	}

}
void set_environment()
{
	
	/*
	change owner of the a.out file from root to normal user
	*/
	string path_out=" ./jail/"+exe;
	string command="chown "+string(restricted_owner)+":"+string(restricted_owner)+ path_out;
	if(system(command.c_str())==-1)
	{
		cerr<<"Error changing owner of file "<<exe;
		exit(EXECUTION_ERROR);
	}
	
	/*
	redirecting input from input-file in /jail/in to /jail/out
	*/
	int infile_fd,outfile_fd;
	string path_in="./problems/"+string(problem)+"/in";
	path_out="./code/"+string(username)+"/"+string(problem)+"/main_out";	//name of outfile file going to be generated
	if((infile_fd=open(path_in.c_str(),O_RDONLY))==-1)
	{
		perror("Error open input file");
		exit(EXECUTION_ERROR);
	}
	if((outfile_fd=open(path_out.c_str(),O_RDWR|O_CREAT|O_TRUNC))==-1)
	{
		perror("Error open output file");
		exit(EXECUTION_ERROR);
	}
	if(dup2(infile_fd,0)==-1)
	{
		perror("Error dup, input file");
		exit(EXECUTION_ERROR);
	}
	if(dup2(outfile_fd,1)==-1)
	{
		perror("Error dup, outfile file");
		exit(EXECUTION_ERROR);
	}

	/*
	Time limit will be set in parent process as signal handler
	won't be valid after exec. 
	*/
	
	/*
	TO DO:set memory limits
	*/
	
	/*
	TO DO:set cpu limits
	*/
	
	/*	
	change root dir to jail (restricted environment)
	system() function uses sh shell to execute command.so sh shell must be present in jailed folder
	*/
	
	if(chroot("./jail"))
	{
		perror("Error Jail");
		exit(EXECUTION_ERROR);
	}
	if(chdir("/"))
	{
		perror("Error chdir");
		exit(EXECUTION_ERROR);
	}
}
void execute()
{
	if((pid=fork())==-1)
	{
		perror("Error Fork");
		exit(EXECUTION_ERROR);
	}
	/*
	child process:
	set environment and execute the code
	*/
	if(!pid)
	{
		set_environment();
		/*
		Allow parent process to trace child 
		*/
		#ifdef SECURE
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		#endif
		execv(exe.c_str(),NULL);
		//exec never return if no error
		perror("Error exec");
		exit(EXECUTION_ERROR);	
	}
	/*
	parent process:
	wait for child
	*/
	else
	{
		int status,orig_eax;
		int insyscall=0;
		register_timeout(time_limit,time_out);
		while(1)
		{
			
			wait(&status);
			if(WIFSIGNALED(status))
			{
				cerr<<err[RUN_TIME_ERROR];
				exit(RUN_TIME_ERROR);
			
			}
			if(WIFEXITED(status))
			{
			
				if(WEXITSTATUS(status)==EXECUTION_ERROR)
				{
					cerr<<err[EXECUTION_ERROR]<<endl;	
					exit(EXECUTION_ERROR);
				}
				else
				{
					print_total_time();
                        		break;
				}
	               	}
	               	#ifdef SECURE
	               	if (insyscall==0)
                        {
                        	/*
                        	Exiting from system call
                        	this part become active when exiting from a system call
                        	it may seem contraty to many examples on net but there is slight
                        	difference. Bottom-line is : It works
                        	*/
				insyscall = 1;
                         }
                        else
              		{
              			/*
              			Before entering to system call
              			*/
				insyscall = 0;
				/*
				Clone system call is also checked as "fork" system call is not 
				calling fork but clone system call.
				Ptrace here peek into the child process registers 
				and return the system call number.
				*/
			
				if((orig_eax=ptrace(PTRACE_PEEKUSER,pid, 4 * ORIG_EAX, NULL))==__NR_fork || 
				orig_eax==__NR_vfork ||
				orig_eax==__NR_clone ||
				orig_eax==__NR_socketcall ||
				orig_eax==__NR_execve)
                             	{
                             		cerr<<"Illegal system call\n";
	   	                        ptrace(PTRACE_KILL,pid,0,0);
                             		exit(WRONG);
                             	}
			}
                        ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
			#endif
		}
	}
}

void compare()
{
	string file1 ="./problems/"+string(problem)+"/out";	//sample output file
	string file2 = "./code/"+string(username)+"/"+string(problem)+"/main_out"; //file generated by program
	if(open(file2.c_str(),O_RDONLY)==-1)
	{
		cerr<<err[WRONG];
		exit(WRONG);
	}
	string diff="diff " + file1 + " " + file2;
	int FLAG,FLAG2;
	if((FLAG=system(diff.c_str()))==-1)
	{
		perror("Error System:diff->1");
		exit(EXECUTION_ERROR);
	}
	if(WEXITSTATUS(FLAG)==0)
	{
		cerr<<err[RIGHT];
		exit(RIGHT);
	}
	else
	{
		#ifdef PRESENTATION
		 string diff1="diff -bBiw " + file1 + " " + file2;
		 if((FLAG2=system(diff1.c_str()))==-1)
		 {
			perror("Error System:diff->2");
	 		exit(EXECUTION_ERROR);
		 }
		 if(WEXITSTATUS(FLAG2)==0)
		 {
		 	cerr<<err[PRESENTATION_ERROR]<<endl;
		 	exit(PRESENTATION_ERROR);
		 }
		 else
		#endif
		{
		 	cerr<<err[WRONG]<<endl;
		  	exit(WRONG);
		}
	}
}

void register_timeout(double time, void handler(int))
{
	 itimerval time_limit;
	/*
	conveerting time to seconds and microseconds
	*/
	int sec,micro_sec;
	sec=(int)time;
	micro_sec=(int)((time-(int)time)*1e6);	//getting the fractional part of time
	/*
	filling time structure
	*/
//	cout<<"sec"<<sec<<" "<<micro_sec<<endl;
	time_limit.it_value.tv_sec=sec;
	time_limit.it_value.tv_usec=micro_sec;
	/*alarm to be set only once...
	so set periodic-interval value to 0
	*/
	time_limit.it_interval.tv_sec=0;
	time_limit.it_interval.tv_usec=0;
	if(signal(SIGALRM,handler)==SIG_ERR)
	perror("Signal Error:");

	if(setitimer(ITIMER_REAL, &time_limit,0)!=0)
	perror("Settimer Error:");
}

void print_total_time()
{
       	itimerval current;
	getitimer(ITIMER_REAL,&current);
	cerr<<"Execution Time:"<<time_limit-(current.it_value.tv_sec+current.it_value.tv_usec/1e6)<<endl;
}
void log_init(char *username)
{
	int error_fd;
	if((error_fd=open("logfile",O_WRONLY | O_CREAT | O_APPEND,0644))==-1) //644
	{
		perror("open errro file");
		exit(EXECUTION_ERROR);
	}
	dup2(error_fd,2);
	close(error_fd);
	write(2,username,strlen(username));
	char marker[]="   ____________________________________________________________\n";	
	write(2,marker,sizeof(marker)-1);

}