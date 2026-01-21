/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include "kfifo.h"
using namespace std;

#define MAX_CAPACITY (1024)

#define BLACK "\033[1;30;30m"
#define RED "\033[1;30;31m"
#define END "\033[0m\n"
#define PRINT printf
#define LOGE(fmt, ...) PRINT(RED "<%s>.%d: " fmt END, __func__, __LINE__, ##__VA_ARGS__)
#define LOGI(fmt, ...) PRINT(BLACK "<%s>.%d: " fmt END, __func__, __LINE__, ##__VA_ARGS__)

struct student {
    long id;
    unsigned int age;
    unsigned int score;
};

static int ut_0001(void);
static int ut_0002(void);

static void msleep(unsigned int ms) {
    this_thread::sleep_for(chrono::milliseconds(ms));
}

static void get_student(struct student *s) {
    static long id = 0;
    s->id = ++id;
    s->age = rand() % 30;
    s->score = rand() % 101;
}

static void consumer(void *arg) {
    kfifo *fifo = (kfifo *)arg;
    struct student s;

    while (1) {
        msleep(100);

        unsigned int len = kfifo_pop(fifo, &s, sizeof(struct student));
        if (len > 0) {
            LOGI("--- id = %8ld, age = %3u, score = %3u", s.id, s.age, s.score);
        }
    }
}

static void producer(void *arg) {
    kfifo *fifo = (kfifo *)arg;
    struct student s;

    while (1) {
        get_student(&s);
        unsigned int len = kfifo_put(fifo, &s, sizeof(struct student));
        if (len > 0) {
            LOGI("+++ id = %8ld, age = %3u, score = %3u", s.id, s.age, s.score);
        }

        msleep(200);
    }
}

int main(int argc, char const *argv[]) {
    time_t tm;
    time(&tm);
    srand(tm);

    if (ut_0001() || ut_0002()) {
        return 0;
    }

    kfifo fifo = {0};
    if (0 != kfifo_alloc(&fifo, MAX_CAPACITY * sizeof(struct student))) {
        LOGE("kfifo alloc fail");
        return 0;
    }

    thread t1 = thread(producer, &fifo);
    thread t2 = thread(consumer, &fifo);

    t1.join();
    t2.join();

    kfifo_free(&fifo);
    return 0;
}

static int ut_0001(void) {
    kfifo fifo = {0};
    if (0 != kfifo_alloc(&fifo, MAX_CAPACITY * sizeof(struct student))) {
        LOGE("fifo alloc fail");
        return 1;
    }

    unsigned int capacity;
    unsigned int len;
    unsigned int ret = 1;
    struct student ss = {99, 10, 100};

    /* check empty after alloc */
    if (!kfifo_is_empty(&fifo)) {
        LOGE("fifo is not empty after alloc");
        goto __END__;
    }

    /* check capacity */
    capacity = MAX_CAPACITY * sizeof(struct student);
    if (capacity != kfifo_size(&fifo)) {
        LOGE("fifo capacity is %u incorrect, expected: %u", kfifo_size(&fifo), capacity);
        goto __END__;
    }

    /* check empty after reset */
    len = kfifo_put(&fifo, &ss, sizeof(struct student));
    if (len != sizeof(struct student)) {
        LOGE("put 99 fail, putted len %u", len);
        goto __END__;
    }

    kfifo_reset(&fifo);

    if (!kfifo_is_empty(&fifo)) {
        LOGE("fifo is not empty after reset");
        goto __END__;
    }

    /* push elements */
    for (unsigned int i = 0; i < MAX_CAPACITY; ++i) {
        struct student s {
            i, 20 + i, 80 + i
        };

        len = kfifo_put(&fifo, &s, sizeof(struct student));
        if (len != sizeof(struct student)) {
            LOGE("put %d fail, putted len %u", i, len);
            goto __END__;
        }

        if (kfifo_is_empty(&fifo)) {
            LOGE("fifo is empty after putted %d", i);
            goto __END__;
        }

        if (i < MAX_CAPACITY - 1) {
            if (kfifo_is_full(&fifo)) {
                LOGE("fifo is full after putted %d", i);
                goto __END__;
            }
        }

        /* check used memory */
        len = kfifo_len(&fifo);
        if (len != (i + 1) * sizeof(struct student)) {
            LOGE("fifo used len %u is unexpected after putted %d", len, i);
            goto __END__;
        }

        /* check available memory */
        if ((capacity - len) != kfifo_avail(&fifo)) {
            LOGE("fifo available len %u is unexpected after putted %d", kfifo_avail(&fifo), i);
            goto __END__;
        }
    }

    if (!kfifo_is_full(&fifo)) {
        LOGE("fifo is not full after all putted");
        goto __END__;
    }

    /* pop elements */
    for (unsigned int i = 0; i < MAX_CAPACITY; ++i) {
        struct student s1 {
            i, 20 + i, 80 + i
        };

        struct student s2;
        len = kfifo_pop(&fifo, &s2, sizeof(struct student));
        if (len != sizeof(struct student)) {
            LOGE("pop %d fail, putted len %u", i, len);
            goto __END__;
        }

        /* check element */
        if (s1.id != s2.id || s1.age != s2.age || s1.score != s2.score) {
            LOGE("pop %d unexpected element, id %u, age %u, score %u", i, s2.id, s2.age, s2.score);
            goto __END__;
        }

        if (kfifo_is_full(&fifo)) {
            LOGE("fifo is full after popped %d", i);
            goto __END__;
        }

        /* check used memory */
        len = kfifo_len(&fifo);
        if (len != (MAX_CAPACITY - i - 1) * sizeof(struct student)) {
            LOGE("fifo used len %u is unexpected after popped %d", len, i);
            goto __END__;
        }

        /* check available memory */
        if ((capacity - len) != kfifo_avail(&fifo)) {
            LOGE("fifo available len %u is unexpected after popped %d", kfifo_avail(&fifo), i);
            goto __END__;
        }
    }

    if (!kfifo_is_empty(&fifo)) {
        LOGE("fifo is not empty after all popped");
        goto __END__;
    }

    ret = 0;
    LOGI("pass");

__END__:
    kfifo_free(&fifo);
    return ret;
}

static int ut_0002(void) {
    kfifo fifo = {0};
    int ret = 1;

    constexpr unsigned int capacity = 64; /* must be pow of 2 */
    if (0 != kfifo_alloc(&fifo, capacity)) {
        LOGE("fifo alloc fail");
        return 1;
    } else {
        memset(fifo.data, 0x00, capacity);
    }

    kfifo_ele ele;
    unsigned int len;

    constexpr unsigned int sz1 = capacity - 10;
    constexpr unsigned int sz2 = 30;
    constexpr unsigned int sz3 = 25;
    constexpr unsigned int sz4 = sz1 + sz3;
    unsigned char src[sz4] = {0};
    unsigned char dst[sz4] = {0};
    constexpr unsigned int sz5 = sz1 - sz2 + sz3;

    /*
        |               sz4                |
        |         sz1          |
        |    sz2   |           |    sz3    |
        01       30 31       54 55       74
        A1 AA .. A2 B1 BB .. B2 C1 CC .. C2
    */
    memset(&src[0x0], 0xAA, sz2);
    src[0] = 0xA1;
    src[sz2 - 1] = 0xA2;
    memset(&src[sz2], 0xBB, sz1 - sz2);
    src[sz2] = 0xB1;
    src[sz1 - 1] = 0xB2;
    memset(&src[sz1], 0xCC, sz3);
    src[sz1] = 0xC1;
    src[sz4 - 1] = 0xC2;

    auto hexdump = [](void *buf, unsigned int len, const char *s) {
        PRINT("dump %s, -->: %u\n", s, len);
        unsigned char *p = (unsigned char *)buf;
        for (unsigned int i = 1; i <= len; ++i) {
            PRINT("%02X ", *p++);
            if (i % 16 == 0) {
                PRINT("\n");
            }
        }
        if (len > 0) {
            PRINT("\n");
        }
    };

    /* push sz1
        01       30 31       54 55       64
        A1 AA .. A2 B1 BB .. B2 XX XX .. XX
    */
    len = kfifo_put(&fifo, &src[0x0], sz1);
    if (len != sz1) {
        LOGE("put %u fail, putted len %u", sz1, len);
        goto __END__;
    }

    hexdump((unsigned char *)fifo.data, capacity, "fifo-mem-1");

    /* pop sz2
        01       30 31 32    54 55       64
        XX XX .. XX B1 BB .. B2 XX .. .. XX
    */
    len = kfifo_pop(&fifo, dst, sz2);
    if (len != sz2) {
        LOGE("pop %u fail, popped len %u", sz2, len);
        goto __END__;
    } else {
        if (0 != memcmp(src, dst, sz2)) {
            LOGE("check pop %u fail", sz2);
            goto __END__;
        }
    }

    /* pop sz2
       01       15 16       30 31       54 55       64
       CC CC .. C2 XX XX .. XX B1 BB .. B2 C1 CC .. CC
   */
    len = kfifo_put(&fifo, &src[sz1], sz3);
    if (len != sz3) {
        LOGE("put %u fail, putted len %u", sz3, len);
        goto __END__;
    }

    hexdump((unsigned char *)fifo.data, capacity, "fifo-mem-2");

    len = kfifo_peek_n(&fifo, &ele, sz5);
    if (len != sz5) {
        LOGE("peek no copy %u fail, len %u", sz5, len);
        goto __END__;
    }

    hexdump(ele.buf[0].buf, ele.buf[0].len, "peek-1");
    hexdump(ele.buf[1].buf, ele.buf[1].len, "peek-2");

    if (ele.buf[0].len + ele.buf[1].len != kfifo_len(&fifo)) {
        LOGE("peek len is not incorrect!");
        goto __END__;
    }

    if (0 != memcmp(&src[sz2], ele.buf[0].buf, ele.buf[0].len)) {
        LOGE("check peek data 1 fail!");
        goto __END__;
    }

    if (ele.buf[1].len > 0) {
        if (0 != memcmp(&src[capacity], ele.buf[1].buf, ele.buf[1].len)) {
            LOGE("check peek data 2 fail!");
            goto __END__;
        }
    }

    ret = 0;
    LOGI("pass");

__END__:
    kfifo_free(&fifo);
    return ret;
}
