

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_PROCESS 15

typedef struct data_manager
{
    int pid;
    char name[256];
    long memory_status;
}data_manager;

// Ver os status da CPU
void get_cpu_status(){
    FILE *cpu_file;
    char BUFFER[1024];
    long user, nice, system,idle;
    cpu_file = fopen("/proc/stat", "r");
    fgets(BUFFER, sizeof(BUFFER),cpu_file);
    sscanf(BUFFER, " cpu %ld %ld %ld %ld", &user, &nice, &system, &idle);
    long total_cpu = user + nice + system + idle;
    double cpu_usage = 100.0 * (total_cpu - idle ) / total_cpu;
    printf("CPU USAGE: %.2f%%\n", cpu_usage);
    fclose(cpu_file);

}

void get_memory_usage(){
    FILE *memory_file;
    long total_memory = 0;
    long free_memory  = 0;

    char label[256];
    long value;
    char unit[32];

    memory_file = fopen("/proc/meminfo", "r");

    while (fscanf(memory_file, "%s %ld %s", label, &value, unit) != EOF){
        if(strcmp(label,"MemTotal:") == 0){
            total_memory = value;
        }
            if(strcmp(label,"MemAvailable:") == 0){
            free_memory = value;
        }
    }
    long memory_used = total_memory - free_memory;
    printf("USO RAM: %.2f%%\n",(100.0 * memory_used)/total_memory);
    printf("RAM USADA: %ld MB / %ld MB\n", memory_used / 1024, total_memory/1024);
    fclose(memory_file);

}

int is_number(const char *str){
    while(*str) {
        if(!isdigit(*str)){
            return 0;
        }
        str++;
    }
    return 1;
}

// Processos do sistema
void list_process(){
    DIR *process_dir;
    struct  dirent *entry;
    data_manager processes[MAX_PROCESS];
    int count = 0;

    process_dir = opendir("/proc");

    while((entry = readdir(process_dir)) != NULL && count < MAX_PROCESS){
        if(is_number(entry->d_name)){
            int pid = atoi(entry->d_name);
            char path_process[256];
            sprintf(path_process, "/proc/%d/status", pid);
            FILE *process_file = fopen(path_process, "r");
            if(process_dir){
                char line[256];
                char name[256] = "Unknown";
                long memory = 0;

                while (fgets(line, sizeof(line), process_file)){
                    if(strncmp(line, "Name:", 5)==0){
                        sscanf(line, "Name:\t%s", name);
                    }
                    if(strncmp(line,"VmRSS:",6)==0){
                        sscanf(line,"VmRSS:\t%ld",&memory);
                    }
                }
                processes[count].pid = pid;

                strcpy(processes[count].name, name);
                processes[count].memory_status = memory;

                count++;

                fclose(process_file);
            }
        }
    }

    closedir(process_dir);

    printf("\n%-10s %-30s %-15s\n", "PID", "Process", "Memory(MB)");

    for (int i = 0; i < count; i++){

        printf("%-10d %-30s %-15ld\n",
            processes[i].pid,
            processes[i].name,
            processes[i].memory_status/1024);
    }

}
int main(){
    
    while(1){
        system("clear");

        printf("========================================\n");
        printf("      TERMINAL TASK MANAGER (C)\n");
        printf("========================================\n\n");

        get_cpu_status();
        printf("\n");

        get_memory_usage();
        printf("\n");

        list_process();

        sleep(2);
    }

    return 0;
}



