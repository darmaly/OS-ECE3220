/*
    Deeb Armaly
    Brandon White
    ECE 3220
    HW2
    Spring 2022
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task_type task_t;

struct task_type {
    int     task_id,
            arrival_time,
            service_time,
            remaining_time,
            completion_time,
            response_time,
            wait_time;
    task_t   *next;
};

typedef struct head_type {
    task_t   *head,
           *tail;
    int     task_counter;
} head_t;

typedef struct job_type {
    int     task_id,
            arrival_time,
            service_time;
} job_t;



void    linkedInsert    (head_t *list, job_t job, task_t *task, int time, int option);
task_t *linkedRemove    (head_t *list);
head_t *constructList   (void);
void    deconstructList (head_t *list);
void    writeToFile     (head_t *list, FILE *fp_write);
void    printList       (head_t *list);
job_t  *parseFile       (FILE *fp_read, int *total_jobs);
void    printHeader     (FILE *fp_write, int option);
void    printReadyQueue (task_t *readyQueue, FILE *fp_write);
task_t *checkCPU        (head_t *readyQueue);
void    printCompleted  (FILE *fp_write, head_t *list); 

task_t *checkCPU(head_t *readyQueue){
    if(readyQueue == NULL) return NULL;
    return readyQueue->head;
}

void printReadyQueue(task_t *readyQueue, FILE *fp_write){
    int count       = 0;
    if (readyQueue == NULL) fprintf(fp_write, "--");
    task_t *rover   = readyQueue;
    while(rover != NULL) {
        rover->wait_time++;
        if (count > 0) 
            fprintf(fp_write, ", ");
        fprintf(fp_write, "%c%d", rover->task_id, rover->remaining_time);
        count++;
        rover = rover->next;
    }
    
}



/* creates a list and returns it to the user */
head_t *constructList () {
    int i;
    head_t *list = (head_t *)malloc(sizeof(head_t));
    list->head  = list->tail = NULL;
    list->task_counter  = 0;
    return list;
}



/* deconstruct a list of type head_t. This cleans up the memory */
void deconstructList (head_t *list) {
    task_t *rover   = list->head;
    task_t *prev    = rover;

    while (rover != NULL) {
        prev        = rover;
        rover       = rover->next;
        list->head  = rover;
        free(prev);
        prev        = NULL;
    }
    free(list);
}


/* This function prints the entire remaining list */
void printList (head_t *list) {
    task_t *rover = list->head;
    while (rover != NULL) {
        printf("%3c\t%3d\t%3d\n", rover->task_id, rover->arrival_time, rover->service_time);
        rover = rover->next;
    }
    printf("\n");
}


/* 
    This function inserts the item into the list.

    2 MODES && 2 options:
        when task == NULL -- (normal)
            allocate memory for node
            insert node @ tail no matter what

        when task != NULL -- (fifo && round robin) || shortest job first
        if option ==  1
            no allocation
            insert node @ tail
        if option == -1
            insert node in ascending order based on service time
            this is when the TIME variable will be used
    *************************************************************************
    fifo mode should be used anytime we are NOT inserting into the readyQueue and also when inserting into the readyQueue with fifo option selected.
 */
void linkedInsert (head_t *list, job_t job, task_t *task, int time, int option) {
    task_t *rover = list->head;
    task_t *prev  = rover;

    // linked list insertion
    if (task == NULL) {
        task_t *myTask          = (task_t *)calloc(sizeof(task_t), 1);
        myTask->arrival_time    = job.arrival_time;
        myTask->service_time    = job.service_time;
        myTask->remaining_time  = job.service_time;
        myTask->task_id         = job.task_id;
        myTask->next            = NULL;
        
        // this it he total service time for all jobs
        list->task_counter += myTask->service_time;

        // list empty
        if (list->head == NULL) {
            list->head = myTask; 
            list->tail = myTask;
        } 
        // else insert @ tail
        else {
            list->tail->next    = myTask;
            list->tail          = myTask;
        }
    } 
    // readyQueue insertion
    else if (task != NULL) 
    {
        task->next = NULL;
        if (option == -2) {
            task->response_time    = task->service_time + task->wait_time;
            task->completion_time  = task->arrival_time + task->response_time;
        }
        if (list->head == NULL) {
            list->head = task; 
            list->tail = task;
        }
        // fifo / round robin insertion
        else if (option == 1) {
            list->tail->next    = task;
            list->tail          = task;
        } 
        // shortest job first insertion
        else if (option == -1) {
            while (rover != NULL && (task->remaining_time >= rover->remaining_time)) {
                prev  = rover;
                rover = rover->next;
            }
            // when rover == head or tail
            if (rover == list->head){
                task->next = list->head;
                list->head  = task;
            } else if (rover == NULL) {
                prev->next  = task;
                list->tail  = task;
            } else {
                prev->next  = task;
                task->next  = rover;
            }
        }
        else if (option == -2) {
            task->response_time    = task->service_time + task->wait_time;
            task->completion_time  = task->arrival_time + task->response_time;
            while (rover != NULL && (task->task_id >= rover->task_id)) {
                prev  = rover;
                rover = rover->next;
            }
            // when rover == head or tail
            if (rover == list->head){
                task->next = list->head;
                list->head  = task;
            } else if (rover == NULL) {
                prev->next  = task;
                list->tail  = task;
            } else {
                prev->next  = task;
                task->next  = rover;
            }
            
        }
        // last insertion
        else if (option == -3) {
            while (rover != NULL && (task->service_time >= rover->service_time)) {
                if (task->service_time == rover->service_time) {
                    if (task->wait_time < rover->wait_time) break;
                }
                prev  = rover;
                rover = rover->next;
            }
            // when rover == head or tail
            if (rover == list->head){
                task->next = list->head;
                list->head  = task;
            } else if (rover == NULL) {
                prev->next  = task;
                list->tail  = task;
            } else {
                prev->next  = task;
                task->next  = rover;
            }
        } else if (option == -4) {
            task->next = list->head;
            list->head  = task;
        } else if (option == -5) {
            task->next = list->head->next;
            list->head->next = task;
        }
    } 
}


/* This parses the file and stores & returns a dynamically allocated buffer */
job_t *parseFile (FILE *fp_read, int *total_jobs) {
    job_t  *buffer = NULL;
    int     arrive_time, job_time, count;

    count = 0;
    // since buffer == NULL, realloc is useful to clean up the code
    fseek(fp_read, 0, 0);
    while (fscanf(fp_read, "%d %d", &arrive_time, &job_time) != EOF) {
        buffer = (job_t *)realloc(buffer, sizeof(job_t) * (count + 1));
        buffer[count].arrival_time  = arrive_time;
        buffer[count].service_time  = job_time;
        buffer[count].task_id       = 65 + count;

        count++;
    } 
    *total_jobs = count;
    return buffer;
}


/* Since we only remove the head in both lists, this is a simple function */
task_t *linkedRemove (head_t *list) {
    task_t *rover = list->head;
    
    // empty list
    if (list->head == NULL) return NULL;
    
    // 1 item in list
    if (list->head == list->tail)
        list->head = list->tail = NULL;
    else 
        list->head = rover->next;
    return rover;


}


/* This just prints the header to our output file */
void printHeader (FILE *fp_write, int option) {
    char *type;

    type    = (option == 0) ? "FIFO"
            : (option == 1) ? "SJF(preemptive)"
            : (option == 2) ? "RR"
            : "ERROR!!";

    fprintf(fp_write, "%s scheduling results ", type);
    if(option == 2) fprintf(fp_write, "(time slice is 1)");
    fprintf(fp_write, "\n");
    fprintf(fp_write, "time   cpu ready queue (tid/rst)\n");
    fprintf(fp_write, "----------------------------------\n");
}

void printCompleted (FILE *fp_write, head_t *list) {

    fprintf(fp_write,"\n        arrival service completion response wait\n");
    fprintf(fp_write,"tid     time    time    time       time     time\n");
    fprintf(fp_write,"---------------------------------------------\n");
    task_t *rover = list->head;
    task_t *temp  = rover;
    job_t emptyJob;
    while (rover != NULL) {
        fprintf(fp_write, "%2c\t\t\t%2d\t\t\t %2d\t\t\t\t %2d\t\t\t   %2d\t\t %2d\n",rover->task_id, rover->arrival_time, rover->service_time, rover->completion_time, rover->response_time, rover->wait_time);
        rover = rover->next;
    }
    fprintf(fp_write,"\nservice wait\n");
    fprintf(fp_write," time   time\n");
    fprintf(fp_write,"------- ----\n");

}
