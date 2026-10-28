#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <getopt.h> 

typedef struct {
    double x, y, z;
} Pt;

typedef struct {
    Pt *arr;     
    int n;       
    int id;      
    int thr;     
    double res;  
    int i1, i2, i3; 
} Task;

double area(Pt a, Pt b, Pt c) {
    double abx = b.x - a.x, aby = b.y - a.y, abz = b.z - a.z;
    double acx = c.x - a.x, acy = c.y - a.y, acz = c.z - a.z;
    double cx = aby * acz - abz * acy;
    double cy = abz * acx - abx * acz;
    double cz = abx * acy - aby * acx;
    return 0.5 * sqrt(cx * cx + cy * cy + cz * cz);
}

void* work(void *arg) {
    Task *t = (Task *)arg;
    t->res = -1.0;
    
    for (int i = t->id; i < t->n - 2; i += t->thr) {
        for (int j = i + 1; j < t->n - 1; j++) {
            for (int k = j + 1; k < t->n; k++) {
                double s = area(t->arr[i], t->arr[j], t->arr[k]);
                if (s > t->res) {
                    t->res = s;
                    t->i1 = i;
                    t->i2 = j;
                    t->i3 = k;
                }
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    int thr = 1; 
    int n = 0;   
    int key; 

    while ((key = getopt(argc, argv, "t:n:")) != -1) {
        if (key == 't') thr = atoi(optarg);
        if (key == 'n') n = atoi(optarg);
    }

    if (thr < 1 || n < 3) {
        printf("Error: need thr >= 1 and n >= 3\n");
        return 1;
    }
    if (thr > n - 2) thr = n - 2;

    srand(42);
    Pt *pts = malloc(n * sizeof(Pt));
    for (int i = 0; i < n; i++) {
        pts[i].x = (double)rand() / RAND_MAX * 100.0;
        pts[i].y = (double)rand() / RAND_MAX * 100.0;
        pts[i].z = (double)rand() / RAND_MAX * 100.0;
    }

    pthread_t *tids = malloc(thr * sizeof(pthread_t));
    Task *tasks = malloc(thr * sizeof(Task));
    struct timespec st, en;
    clock_gettime(CLOCK_MONOTONIC, &st);
    for (int i = 0; i < thr; i++) {
        tasks[i].arr = pts;
        tasks[i].n = n;
        tasks[i].id = i;
        tasks[i].thr = thr;
        pthread_create(&tids[i], NULL, work, &tasks[i]);
    }
    for (int i = 0; i < thr; i++) {
        pthread_join(tids[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &en);
    double dt = (en.tv_sec - st.tv_sec) + 1e-9 * (en.tv_nsec - st.tv_nsec);
    double best = -1.0;
    int a = -1, b = -1, idx3 = -1;
    for (int i = 0; i < thr; i++) {
        if (tasks[i].res > best) {
            best = tasks[i].res;
            a = tasks[i].i1;
            b = tasks[i].i2;
            idx3 = tasks[i].i3;
        }
    }
    printf("Thr: %d, N: %d\n", thr, n);
    printf("Max area: %.6f\n", best);
    printf("Points: %d %d %d\n", a, b, idx3);
    printf("Time: %.6f sec\n", dt);
    free(pts);
    free(tids);
    free(tasks);
    return 0;
}