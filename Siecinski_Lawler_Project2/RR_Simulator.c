#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// structure to hold job information
struct Job {
    int runtime;
    int time_left;
    bool complete;
    int memory;
    bool allocated;
    int start_time;
    int time_complete;
};


//return a random number between 0 and limit inclusive
//used to ensure there is no skew in the random numbers generated
int rand_lim(int limit) {

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do {
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

int main(int argc, char *argv[]){
    system("export RANDOM_SEED");   //make RANDOM_SEED an environmental variable from shell variable
    if(argc != 8){
        printf("Usage: ./RR_Simulator {Memory Size} {Page Size} {Number of Jobs} {Minimum Runtime} {Maximum Runtime} {Minimum Job Memory} {Maximum Job Memory}\n"); //print usage for users
    }
    else{

        //set variables from parameters
        char *ptr;
        long mem_size = strtol(argv[1], &ptr, 10);
        long page_size = strtol(argv[2], &ptr, 10);
        long num_jobs = strtol(argv[3], &ptr, 10);
        long min_run = strtol(argv[4], &ptr, 10);
        long max_run = strtol(argv[5], &ptr, 10);
        long min_mem = strtol(argv[6], &ptr, 10);
        long max_mem = strtol(argv[7], &ptr, 10);
        long seed = strtol(getenv("RANDOM_SEED"), &ptr, 10);
        srand(seed);

        //print simulator parameters
        printf("Simulator Parameters:\n");
        printf("Memory Size: %ld\nPage Size: %ld\nNumber of Jobs: %ld\nRuntime (min-max) timesteps: %ld-%ld\nMemory (min-max): %ld-%ld\nRandom Seed: %ld\n\n", mem_size,page_size,num_jobs,min_run,max_run,min_mem,max_mem,seed);

        //set other needed variables
        int num_complete = 0;
        int unallocated = mem_size/page_size;
        int page_table[unallocated];
        int i;
        int time_step = 1;
        int curr_job = 0;
        int k;
        for(i = 0;i < unallocated;i++){
            page_table[i] = 0;
        }

        //make queue of jobs in form of array and fill in structure data
        struct Job job_queue[num_jobs];
        for(i = 0;i < num_jobs;i++){
            job_queue[i].runtime = rand_lim(max_run-min_run)+min_run;
            job_queue[i].memory = (rand_lim(max_mem-min_mem)+min_mem);
            job_queue[i].time_left = job_queue[i].runtime;
            job_queue[i].complete = false;
            job_queue[i].allocated = false;
        }

        //print the job queue information
        printf("Job Queue:\n\tJob #\tRuntime\tMemory\n");
        for(i = 0;i < num_jobs;i++){
            printf("\t%d\t%d\t%d\n",i+1,job_queue[i].runtime,job_queue[i].memory);
        }

        //start of timesteps for round robin scheduling
        while(num_complete < num_jobs){
            printf("Time Step %d:\n", time_step);

            //allocate unallocated jobs
            for(i = 0;i < num_jobs;i++){

                if(job_queue[i].allocated == false && job_queue[i].complete == false && unallocated*page_size > job_queue[i].memory){
                    int curr_mem = job_queue[i].memory;
                    for(k = 0; k < mem_size/page_size; k++){
                        if(page_table[k]==0){
                            page_table[k]=i+1;
                            unallocated--;
                            curr_mem = curr_mem-page_size;
                            if(curr_mem<=0){
                                break;
                            }
                        }

                    }
                    job_queue[i].allocated = true;
                    job_queue[i].start_time = time_step;
                    printf("\tJob %d Starting\n", i+1);
                }
            }

            //progress current job
            job_queue[curr_job].time_left--;
            printf("\tContinuing Job %d\n", curr_job+1);
            if(job_queue[curr_job].time_left==0){
                job_queue[curr_job].complete = true;
                job_queue[curr_job].allocated = false;
                for(k = 0; k < mem_size/page_size; k++){
                    if(page_table[k]==curr_job+1){
                        page_table[k] = 0;
                        unallocated++;
                    }
                }
                job_queue[curr_job].time_complete = time_step;
                num_complete++;
                printf("\tJob %d Complete\n", curr_job+1);
            }

            //find next job
            for(i = (curr_job+1)%num_jobs; i < num_jobs; i=(i+1)%num_jobs){
                if(job_queue[i].allocated == true && job_queue[i].complete == false){
                    curr_job = i;
                    break;
                }
                if(num_complete==num_jobs){break;}
            }

            //print page table
            printf("\tPage Table:\n\t\t");
            for(i = 0; i < mem_size/page_size; i++){
                if(i%24 == 0){printf("\n\t\t");}
                else if(i%4 == 0){printf(" ");}
                printf("%d ", page_table[i]);
            }
            printf("\n");

            time_step++;

        }
        //print completion stats
        printf("\nJob Information:\n");
        printf("\tJob #\tStart Time\tEnd Time\n");
        for(i=0; i<num_jobs; i++){
            printf("\t%d\t%d\t\t%d\n", i+1, job_queue[i].start_time, job_queue[i].time_complete);
        }
    }
    return 0;
}
