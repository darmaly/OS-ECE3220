/*

    IMPORTANT:
        *we are printing from the ready queue.

        *when an item is ready, it is removed from the current list
            and added to the ready queue.

        *there are tmp break statements in "while (list->task_counter != 0)"
            this is just because i haven't added the list->task_counter-- yet.
        
        *list->task_counter-- will only happen when an item is in the readyQueue
        
        *I have added a few lines where the code really needs to be inserted


        *my idea is to task_counter-- and rQueue->head->remaining_time--
            if(rQueue->head->remaining_time == 0) then remove it from the
            ready queue. 
            if nothing is waiting behind it in the queue (&& list->task_counter != 0), then wait for another item to appear in the queue. 

*/



#include "functions.h"


int main (int argc, char **argv) {
    
    /* Usage Error */
    if (argc != 4) {
        printf("Error: a.out [option] [file_read] [file_write]\n");
        exit(0);
    }

    int     totalJobs, i, time, option;
    FILE   *fp_read, *fp_write;
    head_t *list, *rQueue, *completedList;
    job_t   emptyJob;
    task_t *rover, *taskDone, *temp;

    
    option  = (strcmp(argv[1], "-fifo") == 0) ? 0
            : (strcmp(argv[1], "-sjf")  == 0) ? 1
            : (strcmp(argv[1], "-rr")   == 0) ? 2
            : -1; // invalid option

    fp_read = fopen(argv[2], "r");
    // fp_read = fopen("in1", "r");

    // parse the file
    job_t * buffer = parseFile(fp_read, &totalJobs);
    fclose(fp_read);

    // create our list and ready queue
    list            = constructList();
    rQueue          = constructList();
    completedList   = constructList();

    // insert buffer into list, then free buffer
    for (i = 0; i < totalJobs; i++)
        linkedInsert(list, buffer[i], NULL, 0, 0);
    free(buffer);
    // printList(list);

    time = 0;
    fp_write = fopen(argv[3], "w");

    /* FIFO */
    if (option == 0) 
    {
        printHeader(fp_write, option);
        while (list->task_counter != 0) {
            // fill ReadyQueue
            rover = list->head;
            while (rover != NULL && rover->arrival_time == time) {
                linkedRemove(list);
                linkedInsert(rQueue, emptyJob, rover, time, 1); 
                rover = list->head;
            }
            // check ReadyQueue
            /* INSERT CODE HERE */
            rover = checkCPU(rQueue);
            fprintf(fp_write, "%3d  ", time);
            if (rover != NULL) {
                fprintf(fp_write, "\t\t%c%d\t\t", rover->task_id, rover->remaining_time--);
                printReadyQueue(rQueue->head->next, fp_write);
                list->task_counter--;
                if (rover->remaining_time == 0) {
                    taskDone = linkedRemove(rQueue);
                    linkedInsert(completedList, emptyJob, taskDone, 0, -2);
                } 
            } else {
                fprintf(fp_write, "\t\t\t\t\t--");
            }
            fprintf(fp_write, "\n");

            time++;
            if (time > 1000) break; // just to be safe
        }
    } 
    /* SHORTEST JOB FIRST */
    else if (option == 1) 
    {
        printHeader(fp_write, option);
        while (list->task_counter != 0) {
            // fill ReadyQueue
            rover = list->head;
            while (rover != NULL && rover->arrival_time == time) {
                linkedRemove(list);
                linkedInsert(rQueue, emptyJob, rover, time, -1); 
                rover = list->head;
            }
            // check ReadyQueue
            /* INSERT CODE HERE */
            rover = checkCPU(rQueue);
            fprintf(fp_write, "%3d  ", time);
            if (rover != NULL) {
                fprintf(fp_write, "\t\t%c%d\t\t", rover->task_id, rover->remaining_time--);
                printReadyQueue(rQueue->head->next, fp_write);
                list->task_counter--;
                if (rover->remaining_time == 0) {
                    taskDone = linkedRemove(rQueue);
                    linkedInsert(completedList, emptyJob, taskDone, 0, -2);
                } 
            } else {
                fprintf(fp_write, "\t\t\t\t\t--");
            }
            fprintf(fp_write, "\n");

            time++;
            if (time > 1000) break; // just to be safe
        }
    }
    /* ROUND ROBIN */
    else if (option == 2) {
        printHeader(fp_write, option);
        while (list->task_counter != 0) {
            // fill ReadyQueue
            rover = list->head;
            while (rover != NULL && rover->arrival_time == time) {
                linkedRemove(list);
                linkedInsert(rQueue, emptyJob, rover, time, -2); 
                rover = list->head;
            }
            // check ReadyQueue();
            /* INSERT CODE HERE */
            rover = checkCPU(rQueue);
            fprintf(fp_write, "%3d  ", time);
            if (rover != NULL) {
                fprintf(fp_write, "\t\t%c%d\t\t", rover->task_id, rover->remaining_time--);
                printReadyQueue(rQueue->head->next, fp_write);
                // rover = rQueue->head;
                temp = linkedRemove(rQueue);
                rover = rQueue->head;
                list->task_counter--;
                if (temp->remaining_time == 0) {
                    linkedInsert(completedList, emptyJob, temp, 0, -2);
                    rover = checkCPU(rQueue);
                } else
                    linkedInsert(rQueue, emptyJob, temp, time, 1);
            } else {
                fprintf(fp_write, "\t\t\t\t\t--");
            }
            fprintf(fp_write, "\n");

            time++;
            if (time > 1000) break; // just to be safe
        } 
    }
    // invalid option
    else {
        printf("Option error!\nvalid options range from 0-2\ncurrent option value = %d\n", option);
        exit(0);
    }

    // these statements just are here for validation that the items are being removed from our current list and inserted into the ready list


    head_t *junk = constructList();
    printCompleted(fp_write, completedList);
    
    rover = completedList->head;
    while (rover != NULL) {
        temp = linkedRemove(completedList);
        linkedInsert(junk, emptyJob, temp, 0, -3); 
        rover = completedList->head;
    }
    rover = junk->head;
    while (rover != NULL) {
        fprintf(fp_write,"%d\t\t\t\t%d\n", rover->service_time, rover->wait_time);
        rover = rover->next;
    }

    deconstructList(list);
    deconstructList(rQueue);
    deconstructList(completedList);
    deconstructList(junk);
    return 0;
}