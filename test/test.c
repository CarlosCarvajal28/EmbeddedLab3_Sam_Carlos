#include <stdio.h>
#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/printk.h>
#include "loop.h"

K_THREAD_STACK_DEFINE(coop_stack, STACKSIZE);
K_THREAD_STACK_DEFINE(coop_stack2, STACKSIZE);

void test_loop_block(){
    struct k_sem semaphore;
    struct k_timer timer;
    int result;
    int counter = 0;

    k_timer_init(&timer, NULL, NULL);
    k_sem_init(&semaphore, 1, 1);

    k_sem_take(&semaphore, K_FOREVER);

    result = do_loop(&timer, &semaphore, &counter, "test", K_MSEC(100));
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(0, counter);
}

void test_loop_run(){
    struct k_sem semaphore;
    struct k_timer timer;
    int result;
    int counter = 0;

    k_timer_init(&timer, NULL, NULL);
    k_sem_init(&semaphore, 1, 1);

    k_sem_take(&semaphore, K_FOREVER);

    result = do_loop(&timer, &semaphore, &counter, "test", K_MSEC(100));
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(1, counter);
}

void test_deadlock(){
    struct k_sem sem1, sem2;
    struct k_timer timer;
    struct k_thread coop_thread, coop_thread2;
    
    
    k_sem_init(&sem1, 1, 1);
    k_sem_init(&sem2, 1, 1);
    k_timer_init(&timer, NULL, NULL);
    k_timer_start(&timer, K_MSEC(1), K_NO_WAIT);
    k_timer_status_sync(&timer);

    k_thread_create(&coop_thread,
                    coop_stack,
                    STACKSIZE,
                    (k_thread_entry_t) deadlock,
                    &sem1,
                    &sem2,
                    &timer,
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);

    k_thread_create(&coop_thread2,
                    coop_stack2,
                    STACKSIZE,
                    (k_thread_entry_t) deadlock,
                    &sem2,
                    &sem1,
                    &timer,
                    K_PRIO_COOP(7),
                    0,
                    K_NO_WAIT);

    TEST_ASSERT_EQUAL(0, k_sem_count_get(&sem1));
    TEST_ASSERT_EQUAL(0, k_sem_count_get(&sem2));

    k_thread_abort(&coop_thread);
    k_thread_abort(&coop_thread2);
}



int main(){
    UNITY_BEGIN();
    RUN_TEST(test_loop_block);
    RUN_TEST(test_loop_run);
    RUN_TEST(test_deadlock);
    return UNITY_END();
}