#include "cutter.h"

#define MAX_C_INFOS_HELD 512 //arbitrary
static computed_info_t c_info_aggregate[MAX_C_INFOS_HELD];
static int num_c_infos_held = 0;

void printer_print_formatted(computed_info_t* c_info)
{
    logger_log(LOG_INFO, "pid %d\n", getpid());
    printf("total\t%.2f%%\n", c_info->cpu.value);

    for (int i = 0; i < get_nprocs(); i++)
    {
        printf("cpu%d\t%.2f%%\n", i, c_info->cores[i].value);
    }
}

static void compute_average_and_print(void)
{
    computed_info_t sum;
    memset(&sum, 0, sizeof(computed_info_t));

    int num_cores = get_nprocs();

    for (int i = 0; i < num_c_infos_held; i++)
    {
        sum.cpu.value += c_info_aggregate[i].cpu.value;

        for (int j = 0; j < num_cores; j++)
        {
            sum.cores[j].value += c_info_aggregate[i].cores[j].value;
        }
    }

    sum.cpu.value /= (float)num_c_infos_held;

    for (int i = 0; i < num_cores; i++)
    {
        sum.cores[i].value /= (float)num_c_infos_held;
    }

    printer_print_formatted(&sum);
}

static void* printer_job(void* data)
{
    UNPACK_JOB_ARGS;

    long prev_time = 0;
    long curr_time = time(NULL);

    while (true)
    {
        prev_time = curr_time;
        curr_time = time(NULL);

        sem_wait(&syncs->c_info_taken_semaphore);
        pthread_mutex_lock(&syncs->c_info_mutex);
        {
            if (prev_time == curr_time && num_c_infos_held < MAX_C_INFOS_HELD)
            {
                c_info_aggregate[num_c_infos_held++] = buffers->c_infos[buffers->num_c_infos];
            }
            else if (prev_time != curr_time)
            {
                compute_average_and_print();

                memset(c_info_aggregate, 0, sizeof(computed_info_t) * MAX_C_INFOS_HELD);
                num_c_infos_held = 0;
            }
            buffers->num_c_infos--;
        }
        pthread_mutex_unlock(&syncs->c_info_mutex);
        sem_post(&syncs->c_info_empty_semaphore);
    }

    return NULL;
}

void printer_job_init(job_t* printer, sync_primitives_t* sync_prims, exchange_buffers_t* buffers)
{
    memset(printer, 0, sizeof(job_t));

    printer->args = (job_arguments_t){
        .prims = sync_prims,
        .buffers = buffers
    };

    pthread_create(&printer->thread, NULL, &printer_job, &printer->args);
}
