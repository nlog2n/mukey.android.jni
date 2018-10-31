#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <dirent.h>

// Android/Linux pid <-> program name


// 通过读/proc/<pid>/cmdline文件, 从process ID获取相应的程序名字
// input: pid - process pid
// output:   appname - 限定256字节长数组
// return:  0- OK, else -fail
int get_name_by_pid_cmdline(int pid, char* appname)
{
	char fname[256];
	snprintf(fname, sizeof(fname), "/proc/%d/cmdline", pid);
	int fp = open(fname, O_RDONLY);
	if (fp < 0)
	{
		return -1; // fail read
	}
	read(fp, appname, 256); // sizeof(fname));
	close(fp);

	// now we got app name
	printf("pid %d, appname %s\n", pid, appname);
	return 0;
}


// program name to pid, 可以用来查询恶意进程名字是否存在.
// 反向查询某appname的pid, 还是通过查询cmdline文件每行.
// 遍历 /proc/<pid>/cmdline 每个 pid, 如果  cmdline 文件内容(1行)等于program name, 则返回该pid.
// return: 0 - not found, >0 found
int get_pid_by_name_cmdline(const char* appname)
{
	int pid = 0;  // not found

	DIR* dir = opendir("/proc");
	if (dir == NULL)
	{
		return 0; // error, should not happen
	}

	struct dirent* de;
	while( (de=readdir(dir)) != NULL)
	{
		// skip those file names which are not numbers (pid)
		pid = atoi(de->d_name);
		if (!pid)
			continue;

        // open file
        char filename[64];
        sprintf(filename, "/proc/%d/cmdline", pid);
        FILE* fp = fopen(filename, "r");
        if (fp == NULL)
        	continue;

        // read line
        int found = 0;
        char line[256];
        while (fgets(line, sizeof(line), fp) != NULL)
        {
        	if (strstr(line, appname) == line) // line starts with this appname
        	{
        		found = 1;
        		break;
        	}
        }

        fclose(fp);

        if (found) break;
	}

	closedir(dir);
	return pid;
}


// 参考java code
// https://android.googlesource.com/platform/cts/+/android-4.4.4_r1/tests/tests/security/src/android/security/cts/Proc.java
/*
int findPidFor(String executable)
{
	File f = new File("/proc");
    for (File d : f.listFiles()) 
    {
       String cmdLineString = d.getAbsolutePath() + "/cmdline";
       File cmdLine = new File(cmdLineString);
       if (cmdLine.exists()) 
       {
          BufferedReader in = null;
          try 
          {
             in = new BufferedReader(new FileReader(cmdLine));
             String line = in.readLine();
             if ((line != null) && line.startsWith(executable)) 
             {
                return Integer.decode(d.getName());
             }
          } 
          finally 
          {
             if (in != null) 
             {
                in.close();
             }
          }
       }
    }
}
*/