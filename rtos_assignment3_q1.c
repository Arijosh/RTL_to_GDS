
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define MSG_SIZE 32
#define TM_SIZE (20 * 1024)
#define TM_COUNT (TM_SIZE / MSG_SIZE)
#define QUEUE_SIZE 64
#define RUN_NS 1000000000LL

typedef struct {
    const char *name;
    long long period_ns;
    long execution_us;
} Task;

static char queue[QUEUE_SIZE][MSG_SIZE];
static int qhead = 0, qtail = 0, qcount = 0;
static unsigned dropped = 0;

static char telemetry[TM_COUNT][MSG_SIZE];
static int tm_head = 0, tm_count = 0;
static unsigned total_stored = 0;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static long long start_ns;

static long long now_ns(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (long long)t.tv_sec * 1000000000LL + t.tv_nsec;
}

static void sleep_until(long long ns) {
    struct timespec t = {
        .tv_sec = ns / 1000000000LL,
        .tv_nsec = ns % 1000000000LL
    };
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}

static void simulate_work(long microseconds) {
    struct timespec t = {
        .tv_sec = microseconds / 1000000,
        .tv_nsec = (microseconds % 1000000) * 1000
    };
    nanosleep(&t, NULL);
}

// Insert a simulated message into the queue.
static void send_message(const char *name, int number) {
    char msg[MSG_SIZE] = {0};
    snprintf(msg, sizeof(msg), "%s Data %04d", name, number);

    pthread_mutex_lock(&lock);

    if (qcount < QUEUE_SIZE) {
        memcpy(queue[qtail], msg, MSG_SIZE);
        qtail = (qtail + 1) % QUEUE_SIZE;
        qcount++;
    } else {
        dropped++;
    }

    pthread_mutex_unlock(&lock);
}

// Save queued messages in a 20 KB circular buffer.
static void receive_telemetry(void) {
    pthread_mutex_lock(&lock);

    while (qcount > 0) {
        memcpy(telemetry[tm_head], queue[qhead], MSG_SIZE);

        qhead = (qhead + 1) % QUEUE_SIZE;
        qcount--;

        tm_head = (tm_head + 1) % TM_COUNT;
        if (tm_count < TM_COUNT)
            tm_count++;

        total_stored++;
    }

    pthread_mutex_unlock(&lock);
}

// Periodic tasks A, B and C.
static void *periodic_task(void *arg) {
    Task *task = (Task *)arg;
    int number = 0;

    for (long long elapsed = 0;
         elapsed < RUN_NS;
         elapsed += task->period_ns) {

        sleep_until(start_ns + elapsed);
        simulate_work(task->execution_us);
        send_message(task->name, ++number);
    }

    return NULL;
}

// Aperiodic event task D.
static void *event_task(void *arg) {
    (void)arg;

    // Simulate one external event at 500 ms.
    sleep_until(start_ns + 500000000LL);
    simulate_work(1000);

    printf("Task D: Simulated event handled\n");
    return NULL;
}

// Telemetry task E runs every 10 ms.
static void *telemetry_task(void *arg) {
    (void)arg;

    for (long long elapsed = 0;
         elapsed < RUN_NS;
         elapsed += 10000000LL) {

        sleep_until(start_ns + elapsed);
        receive_telemetry();
        simulate_work(1000);
    }

    return NULL;
}

int main(void) {
    pthread_t a, b, c, d, e;

    Task A = {"TaskA", 2500000LL, 250};
    Task B = {"TaskB", 2500000LL, 1000};
    Task C = {"TaskC", 10000000LL, 2000};

    start_ns = now_ns() + 100000000LL;

    pthread_create(&e, NULL, telemetry_task, NULL);
    pthread_create(&a, NULL, periodic_task, &A);
    pthread_create(&b, NULL, periodic_task, &B);
    pthread_create(&c, NULL, periodic_task, &C);
    pthread_create(&d, NULL, event_task, NULL);

    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);
    pthread_join(d, NULL);
    pthread_join(e, NULL);

    // Store any remaining messages.
    receive_telemetry();

    printf("\n=== TELEMETRY BUFFER ===\n");
    printf("Total messages stored: %u\n", total_stored);
    printf("Messages retained: %d\n", tm_count);
    printf("Dropped queue messages: %u\n", dropped);

    int oldest = (tm_head - tm_count + TM_COUNT) % TM_COUNT;

    for (int i = 0; i < tm_count; i++) {
        int index = (oldest + i) % TM_COUNT;
        printf("%03d: %s\n", i + 1, telemetry[index]);
    }

    pthread_mutex_destroy(&lock);
    return 0;
}
