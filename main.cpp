#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

int MaxJobs, MaxProcs;

struct task_t
{
    int number, r_j, p_j, size_j;
};

int process_datafile(const char *datafile_name, std::vector<task_t> &tasks)
{
    std::ifstream datafile_stream;
    datafile_stream.open(datafile_name, std::ios_base::in);

    std::string current_line;
    while(std::getline(datafile_stream, current_line))
    {
        int temp_number, temp_rj, temp_pj, temp_sizej, test;
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


int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio(false);
    std::vector<task_t> tasks;
    int test = process_datafile(argv[1], tasks);
    std::cout << "number\t" << "r_j\t" << "p_j\t" << "size_j\n";
    for(task_t s : tasks)
    {
        std::cout << s.number << "\t" << s.r_j << "\t" << s.p_j << "\t" << s.size_j << '\n';
    }
    std::cout << MaxJobs << ' ' << MaxProcs;

}