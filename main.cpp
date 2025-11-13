#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>

int MaxJobs, MaxProcs;

struct task_t
{
    int number;
    int submit_time;
    int run_time;
    int procs;
};

int process_datafile(const char *datafile_name, std::vector<task_t> &tasks)
{
    std::ifstream datafile_stream;
    datafile_stream.open(datafile_name, std::ios_base::in);

    std::string current_line;
    while(std::getline(datafile_stream, current_line))
    {
        int temp_number = -1, temp_rj = -1, temp_pj = -1, temp_sizej = -1, test = -1;
        std::stringstream current_line_stream(current_line);
        if(current_line[0] == ';')
        {
            if(current_line.find("; MaxJobs: ") != std::string_view::npos)
            {
                current_line_stream.seekg(11);
                current_line_stream >> MaxJobs;
            }
            if(current_line.find("; MaxProcs: ") != std::string_view::npos)
            {
                current_line_stream.seekg(12);
                current_line_stream >> MaxProcs;
            }
            continue;
        }

        current_line_stream >> temp_number >> temp_rj >> test >> temp_pj >> temp_sizej;
        if(temp_pj <= 0 || temp_sizej <= 0)
            continue;

        task_t current_job {temp_number, temp_rj, temp_pj, temp_sizej};
        tasks.push_back(current_job);
    }
    return 0;
}



std::vector<std::vector<int>>& schedule_tasks(const std::vector<task_t> &tasks, std::vector<std::vector<int>> &schedule)
{
    std::vector<int> procs_workload(MaxProcs, 0);      // Current workload of processors. Value assigned to each proc id is time of release
    std::vector<bool> is_proc_used(MaxProcs, false);
    int free_procs = MaxProcs;

    int time = 0;
    
    for(task_t task : tasks)
    {
        // while(free_procs < task.procs)
        {
            
        }
        std::cout << "freeprocs: " << free_procs << '\n';
        std::vector<int> assigned_procsid;
        int assigned_procs = 0;
        for(int i = 0; i < MaxProcs; ++i)
        {
            if(assigned_procs >= task.procs) break;
            if(procs_workload[i] <= time)
            {
                procs_workload[i] = time + task.run_time;
                is_proc_used[i] = true;
                assigned_procsid.push_back(i);
                assigned_procs++;
                free_procs--;
            }
        }
        
        std::vector<int> current{task.number, time, time + task.run_time};
        for(int id : assigned_procsid)
        {
            current.push_back(id);
        }
        schedule.push_back(current);
    }
    std::cout << "number\t" << "start\t" << "end\t" << "procs\n";
    int temp = 0;
    for(std::vector<int> i : schedule)
    {
        for(int j : i)
        {
            std::cout << j << "\t";
        }
        std::cout << "(needed " << tasks[temp].procs << ") free_proc: " << free_procs << " taskproc: " << tasks[temp].procs << "\n";
        temp++;
    }
    return schedule;
}

void export_to_file(const std::vector<std::vector<int>> &schedule)
{
    std::ofstream of("schedule");
    for(const std::vector<int> &i : schedule)
    { 
        for(int j : i)
        {
            of << j << ' ';
        }
        of << '\n';
    }
}

int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);
    std::vector<task_t> tasks;
    std::vector<std::vector<int>> schedule;
    int test = process_datafile(argv[1], tasks);
    std::cout << "number\t" << "r_j\t" << "p_j\t" << "size_j\n";
    for(task_t s : tasks)
    {
        std::cout << s.number << "\t" << s.submit_time << "\t" << s.run_time << "\t" << s.procs << '\n';
    }
    std::cout << MaxProcs << '\n';
    schedule_tasks(tasks, schedule);
    export_to_file(schedule);
}