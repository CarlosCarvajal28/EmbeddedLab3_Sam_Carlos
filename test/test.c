#include <stdio.h>
#include <zephyr.h>
#include <arch/cpu.h>
#include <sys/printk.h>
#include "loop.h"



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


int main(){
    UNITY_BEGIN();
    RUN_TEST(test_loop_block);
    RUN_TEST(test_loop_run);
    return UNITY_END();
}