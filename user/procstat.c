/*
Copyright (C) 2016-2019 The University of Notre Dame
This software is distributed under the GNU General Public License.
See the file LICENSE for details.
*/

#include "library/syscalls.h"
#include "kernel/syscall.h"
#include "kernel/stats.h"
#include "library/string.h"

int main(int argc, const char *argv[])
{
	if (argc <= 1) {
		printf("No program to run\n");
		return 1;
	}
	unsigned int startTime;
	syscall_system_time(&startTime);
	int pid = syscall_process_fork();
	if (pid == 0) { // child
		syscall_process_exec(argv[1], argc-1, &argv[1]);
		printf("exec failed\n");
		return 1;
	} else if (pid < 0) {
		printf("fork failed\n");
		return -1;
	}
	/* parent */
	struct process_info info;
	syscall_process_wait(&info, -1);
	unsigned int timeElapsed;
	syscall_system_time(&timeElapsed);
	timeElapsed -= startTime;
	struct kernel_io_stats stat;
	syscall_process_stats(pid,&stat);
	printf("Process %u exited with status %d\n", info.pid, info.exitcode);
	printf("Time elapsed: %d:%d:%d\n", timeElapsed/3600, (timeElapsed%3600)/60, timeElapsed % 60);
	printf("%d blocks read, %d blocks written\n", stat.read_ops, stat.write_ops);
	printf("%d bytes read, %d bytes written\n", stat.read_bytes, stat.write_bytes);

	printf("System calls used: %u\n",stat.syscall_count);

	return 0;
}
