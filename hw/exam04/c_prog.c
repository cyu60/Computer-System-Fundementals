#include <unistd.h> // for read and write
#include <ctype.h>
#include "csapp.h"
#include <string.h>
#include <assert.h>

int str_send(int fd, const char *str);
int str_recv(int fd, char **pstr);

int main(void) {
    // char c;
    // // While not eof
    // // keep read and write
    // while(read(STDIN_FILENO, &c, 1) != 0){
    //     c = toupper(c);
    //     write(STDOUT_FILENO, &c, 1);
    // }
    int client_fd = 15;
    // sender
    int rc = str_send(client_fd, "Hello, world!");
    // if (!rc) {
    // // error sending
    // }
    // receiver
    char *s = NULL;
    rc = str_recv(client_fd, &s);
    printf("hi\n");
    // if (!rc) {
    // // error receiving
    // } else {
    printf("%s\n", s); // prints "Hello, world!"
    free(s);
    // }

    return 0;
}

//  The “wire format” of the data is up to you; consider sending the length of the string before
// the string data???
int str_send(int fd, const char *str) {
    // // what is the len of the str???
    // ssize_t status = rio_readn(fd, str, strlen(str)); // what should the line buff be???
    // if (status == -1) {
    //     return 0;
    // }
    // return 1;
    int str_len = strlen(str);
    assert(str_len <= 65535); // assert that the length is within range
    char buf[str_len + 60]; // need a length
    int len = sprintf(buf, "%59d\n", str_len); // first send the len
    assert(len == 60);
    if (rio_writen(fd, buf, len) != len) {return 0;}
    len = sprintf(buf, "%s", str);
    // assert(len == str_len); // 
    if (rio_writen(fd, buf, len + 1) != len + 1) {return 0;} // need to include null char

    return 1;
    
    // ssize_t status = rio_writen(fd, str, strlen(str));
    // if (status == -1) {
    //     return 0;
    // }
    // return 1;
}

int str_recv(int fd, char **pstr) {
    char len_buf[256];
    if (rio_readn(fd, len_buf, 60) != 60) {return 0;}
    int str_len = atoi(len_buf);// get the str len
    
    char *str_buf = malloc(str_len + 1); // need additional space for null char
    if (rio_readn(fd, str_buf, str_len + 1) != str_len + 1) { // account for null char
        free(str_buf); // make sure to free if failed!
        return 0;
    }

    // str_buf[str_len] = '\0'; need to allocate space for null char? or already accounted?
    *pstr = str_buf; // only change the pstr value if str received successfully 
    return 1;

    // // char buf[65535];
    // char *pstr = malloc(65535);
    // // ensure that the length is correct
    // char * str = *pstr;
    // // if (strlen(str) > 65535) {
    // //     return 0;
    // // }
    
    // // what is the len of the str???
    // // pastr doesn't change unless sucessful??
    // ssize_t status = rio_readn(fd, str, strlen(str)); // what should the line buff be???
    // if (status == -1) {
    //     return 0;
    // }
    // return 1;


    // ssize_t status = rio_writen(fd, );
    // if (status == -1) {
    //     return 0;
    // }
    // return 1;

}


// IS THE SLOT AND ITEMS STRUCTURE CORRECT??
struct Mailbox {
    void* message; // what is the length??
    pthread_mutex_t lock;
    sem_t slots, items;
};
void mb_init(struct Mailbox *mb);
void mb_destroy(struct Mailbox *mb);
void mb_send(struct Mailbox *mb, void *msg);
void *mb_recv(struct Mailbox *mb);

void mb_init(struct Mailbox *mb){
    // mb = malloc(sizeof(Mailbox));
    pthread_mutex_init(mb->lock, NULL);
    sem_init(&mb->slots, 1); // only 1 message per mailbox
    sem_init(&mb->items, 0); // 0 at the start
}

// NEED TO CHECK!!
void mb_destroy(struct Mailbox *mb){
    pthread_mutex_destroy(&mb->lock);
    sem_destroy(&mb->slots);
    sem_destroy(&mb->items);
}

void mb_send(struct Mailbox *mb, void *msg) {
    // wait for slot to be free
    sem_wait(&mb->slots);

    // send the message
    pthread_mutex_lock(&mb->lock);
    mb->message = msg;
    pthread_mutex_unlock(&mb->lock);

    // item now available
    sem_post(&mb->items);
}

void *mb_recv(struct Mailbox *mb) {
    // wait for item to be available
    sem_wait(&mb->items);

    // get the message
    pthread_mutex_lock(&mb->lock);
    void* msg = mb->message;
    pthread_mutex_unlock(&mb->lock);

    // slot now available
    sem_post(&mb->slots);

    return msg;
}


void mod4_count(unsigned *arr, unsigned len, unsigned *result);
void mod4_count(unsigned *arr, unsigned len, unsigned *result) {
for (unsigned i = 0; i < len; i++) {
result[arr[i] & 3]++;
}
}

struct Work {
    // TODO: add fields
    unsigned result[4]; // initialized to 0 by default
    unsigned len;
    unsigned *arr;

};

void *worker(void *arg) {
    struct Work *work = arg;
    // TODO: do this worker’s part of the computation
    mod4_count(work->arr, work->len, work->result);
    return NULL;
}

void mod4_count_par(unsigned *arr, unsigned len, unsigned *result) {
    struct Work *work1 = malloc(sizeof(struct Work)),
    *work2 = malloc(sizeof(struct Work));

    // TODO: initialize fields of Work objects for each worker thread
    // Need to set the result to 0??
    // first worker handles the first len/2 arrays
    work1->arr = arr;
    work1->len = len/2; // odd number???
    // work1->result = result;
    // Start from the middle
    work2->arr = arr[len/2];
    work2->len = len/2 + len % 2; // odd number???
    // work2->result = result;


    // Assume that both pthread_create calls will succeed
    pthread_t thr1, thr2;
    pthread_create(&thr1, NULL, worker, work1);
    pthread_create(&thr2, NULL, worker, work2);
    // TODO: wait for worker threads to complete
    // ptherad join
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

    // TODO: ensure that results are in result array
    // Add the count together
    result[0] = work1->result[0] + work2->result[0]; 
    result[1] = work1->result[1] + work2->result[1]; 
    result[2] = work1->result[2] + work2->result[2]; 
    result[3] = work1->result[3] + work2->result[3]; 

    free(work1);
    free(work2);
}
