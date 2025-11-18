#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <queue>
#include <chrono>
#include <list>
#include <algorithm>

#ifdef __linux__
#include <unistd.h>
#include <signal.h>
#endif

int MaxJobs, MaxProcs;
bool should_close = 0;

struct task_t
{
    int number;
    int submit_time;
    int run_time;
    int procs;
};

int process_datafile(const char *datafile_name, int tasks_number, std::vector<task_t> &tasks)
{
    std::ifstream datafile_stream;
    datafile_stream.open(datafile_name, std::ios_base::in);
    if(datafile_stream.fail())
    {
        std::cerr << "Nie znaleziono pliku " << datafile_name << ".\n";
        return 0;
    }
    int n = 0;
    std::string current_line;
    while(std::getline(datafile_stream, current_line))
    {
        if(n >= tasks_number)
            break;
        int temp_number = -1, temp_rj = -1, temp_pj = -1, temp_sizej = -1, test = -1;
        std::stringstream current_line_stream(current_line);
        if(current_line[0] == ';')
        {
            auto MaxJobsPos = current_line.find("MaxJobs: ");
            auto MaxProcsPos = current_line.find("MaxProcs: ");
            if(MaxJobsPos != std::string::npos)
            {
                current_line_stream.seekg(MaxJobsPos + 9);
                current_line_stream >> MaxJobs;
            }
            if(MaxProcsPos != std::string::npos)
            {
                current_line_stream.seekg(MaxProcsPos + 10);
                current_line_stream >> MaxProcs;
            }
            continue;
        }
        n++;
        current_line_stream >> temp_number >> temp_rj >> test >> temp_pj >> temp_sizej;
        if(temp_pj <= 0 || temp_sizej <= 0)
            continue;

        task_t current_job {temp_number, temp_rj, temp_pj, temp_sizej};
        tasks.push_back(current_job);
    }
    return 1;
}



int schedule_tasks(const std::vector<task_t> &tasks, std::vector<std::vector<int>> &schedule)
{
    std::vector<int> procs_workload(MaxProcs, -1);      // Current workload of processors. Value assigned to each proc id is time of release
    std::vector<bool> proc_isUsed(MaxProcs, false);

    std::vector<task_t> tasks_list(tasks.begin(), tasks.end());
    std::sort(tasks_list.begin(), tasks_list.end(), [](task_t& t1, task_t& t2){return t1.run_time < t2.run_time;});

    int time = 0;
    while(!tasks_list.empty())
    {
        for(int i = 0; i < tasks_list.size(); ++i)
        {
            if(should_close)
                return 2;
            if(tasks_list[i].submit_time > time)
                continue;

            std::vector<int> assigned_procs_ids;
            int assigned_procs_count = 0;
            for(int id = 0; id < MaxProcs; ++id)
            {
                if(assigned_procs_count >= tasks_list[i].procs)
                    break; 
                if(procs_workload[id] <= time)
                {
                    assigned_procs_count++;
                    assigned_procs_ids.push_back(id);
                }
            }
            if(assigned_procs_count < tasks_list[i].procs)
                continue;
            for(int id : assigned_procs_ids)
            {
                procs_workload[id] = time + tasks_list[i].run_time; 
            }
            std::vector<int> current_task;
            current_task.push_back(tasks_list[i].number);
            current_task.push_back(time);
            current_task.push_back(time + tasks_list[i].run_time);
            for(int i : assigned_procs_ids)
            {
                current_task.push_back(i);
            }
            schedule.push_back(current_task);
            tasks_list.erase(tasks_list.begin() + i);
            i--;
        }
        
        int min_release_time = INT_MAX;
        for(int i = 0; i < tasks_list.size(); ++i)
        {
            if(tasks_list[i].submit_time > time)
                min_release_time = std::min(tasks_list[i].submit_time, min_release_time);
        }
        for(int i = 0; i < MaxProcs; ++i)
        {
            if(procs_workload[i] > time)min_release_time = std::min(procs_workload[i], min_release_time);
        }
        if (min_release_time == INT_MAX)
            break;
        time = min_release_time;
    }

    
    return 1;
}

void export_to_file(const std::vector<std::vector<int>> &schedule)
{
    std::ofstream of("./schedule");
    for(const std::vector<int> &i : schedule)
    { 
        for(int j : i)
        {
            of << j << ' ';
        }
        of << '\n';
    }
}

void alarm_handling(int a)
{
    should_close = 1;
}

int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);
    if(argc != 3 || !atoi(argv[2]))
    {
        std::cerr << "Poprawne uzycie: " << argv[0] << " [nazwa pliku] [liczba zadan]\n";
        return 1;
    }

    std::vector<task_t> tasks;
    std::vector<std::vector<int>> schedule;

    if(!process_datafile(argv[1], atoi(argv[2]), tasks))
    {
        std::cerr << "Blad podczas wczytywania z pliku.\n";
        return 1;
    }

    #ifdef __linux__
    signal(SIGALRM, alarm_handling);
    alarm(300);
    #endif

    auto begin = std::chrono::high_resolution_clock::now();
    schedule_tasks(tasks, schedule);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Execution time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n";

    export_to_file(schedule);
}