#include<stdio.h>
#include<stdlib.h>
#include<stdalign.h>

#define PROCESS_NAME_LEN 32
#define MIN_SLICE 10
#define DEFAULT_MEM_SIZE 1024
#define DEFAULT_MEM_START 0

#define MA_FF 1
#define MA_BF 2
#define MA_WF 3

int mem_size = DEFAULT_MEM_SIZE;
int ma_algorithm = MA_FF;
static int pid = 0;
int flag = 0;

struct free_block_type    //内存空闲分区块
{
    /* data */
    int size;
    int start_addr;
    struct free_block_type* next;
};
struct free_block_type* free_block; //空闲块链表头指针

struct allocated_block   //已分配的内存块
{
    /* data */
    int pid;
    int size;
    int start_addr;
    char process_name[PROCESS_NAME_LEN];
    struct allocated_block* next;
};
struct allocated_block* allocated_block_head = NULL;  //分配内存块链表头指针

struct free_block_type* init_free_block(int mem_size);
void display_menu();
int set_mem_size();
void set_algorithm();
void rearrange(int algorithm);
int new_process();
int allocate_mem(struct allocated_block* ab);
void kill_process();
int free_mem(struct allocated_block* ab);
struct allocated_block* find_process(int pid);
int dispose(struct allocated_block* free_ab);
int display_mem_usage();
void do_exit();
void rearrange_FF();
void rearrange_BF();
void rearrange_WF();
int total_free();
void memory_compaction(struct allocated_block* ab, int remain);
void sort_free_block();
void mergeFreeBlock();

int main() {
    char choice;
    pid = 0;
    free_block = init_free_block(mem_size);
    while (1) {
        display_menu();
        //fflush(stdin);
        //choice = getchar();
        scanf("%s", &choice);
        switch (choice)
        {
        case '1':set_mem_size();  break;
        case '2':set_algorithm();  flag = 1;  break;
        case '3':new_process();   flag = 1;  break;
        case '4':kill_process();  flag = 1;  break;
        case '5':display_mem_usage();  flag = 1;  break;
        case '0':do_exit(); exit(0);
        default:  break;
        }
    }
}

void do_exit() {
    exit(0);
}

struct free_block_type* init_free_block(int mem_size) {
    struct free_block_type* fb;
    fb = (struct free_block_type*)malloc(sizeof(struct free_block_type));
    if (fb == NULL) {
        printf("No memory left!\n");
        return NULL;
    }
    fb->size = mem_size;
    fb->start_addr = DEFAULT_MEM_START;
    fb->next = NULL;
    return fb;
}

void display_menu() {
    printf("\n");
    printf("1 - Set memory size (default=%d)\n", DEFAULT_MEM_SIZE);
    printf("2 - Select memory allocation algorithm\n");
    printf("3 - New process\n");
    printf("4 - Terminate a process\n");
    printf("5 - Display memory usage\n");
    printf("0 - Exit\n");
}

int set_mem_size() {
    int size;
    if (flag != 0) {
        printf("Cannot set memory size again\n");
        return 0;
    }
    printf("Total memory size = ");
    scanf("%d", &size);
    if (size > 0) {
        mem_size = size;
        free_block->size = mem_size;
    }
    flag = 1;
    return 1;
}

void set_algorithm() {
    int algorithm;
    printf("\t1 - First Fit\n");
    printf("\t2 - Best  Fit\n");
    printf("\t3 - Worst Fit\n");
    scanf("%d", &algorithm);
    if (algorithm >= 1 && algorithm <= 3) {
        ma_algorithm = algorithm;
    }
    rearrange(ma_algorithm);
}

void rearrange(int algorithm) {
    switch (algorithm) {
    case MA_FF: rearrange_FF();  break;
    case MA_BF: rearrange_BF();  break;
    case MA_WF: rearrange_WF();  break;
    }
}

int count_free_block() {
    struct free_block_type* head = free_block;
    int count = 0;
    while (head) {
        ++count;
        head = head->next;
    }
    return count;
}

void mergeFreeBlock() {
    struct free_block_type* head = free_block;
    struct free_block_type* cur = free_block;
    struct free_block_type* pre = free_block;
    if (head == NULL || head->next == NULL) {
        return;
    }
    while (pre) {
        while (pre->next && pre->size + pre->start_addr == pre->next->start_addr) {
            pre->size = pre->size + pre->next->size;
            struct free_block_type* tmp = pre->next;
            pre->next = pre->next->next;
            pre = pre->next;
            free(tmp);
            tmp = NULL;
            if (pre == NULL)  break;
        }
        if (pre == NULL)  break;
        pre = pre->next;
    }
}

//按照首次适应算法重新整理内存空闲块链表
void rearrange_FF() {
    sort_free_block();
}

void rearrange_BF() {
    struct free_block_type* first = free_block;
    if (first == NULL || first->next == NULL) {
        return;
    }
    struct free_block_type* second = NULL;
    while (first != second) {
        while (first->next != second) {
            if (first->size > first->next->size) {
                int tmp1 = first->start_addr;
                first->start_addr = first->next->start_addr;
                first->next->start_addr = tmp1;
                int tmp2 = first->size;
                first->size = first->next->size;
                first->next->size = tmp2;
            }
            first = first->next;
        }
        second = first;
        first = free_block;
    }
}

void rearrange_WF() {
    struct free_block_type* first = free_block;
    if (first == NULL || first->next == NULL) {
        return;
    }
    struct free_block_type* second = NULL;
    while (first != second) {
        while (first->next != second) {
            if (first->size < first->next->size) {
                int tmp1 = first->start_addr;
                first->start_addr = first->next->start_addr;
                first->next->start_addr = tmp1;
                int tmp2 = first->size;
                first->size = first->next->size;
                first->next->size = tmp2;
            }
            first = first->next;
        }
        second = first;
        first = free_block;
    }
}
int new_process() {
    struct allocated_block* ab;
    int size;
    int ret;
    ab = (struct allocated_block*)malloc(sizeof(struct allocated_block));
    if (!ab) {
        exit(-5);
    }
    ab->next = NULL;
    pid++;
    sprintf(ab->process_name, "PROCESS-%02d", pid);
    ab->pid = pid;
    printf("Memory for %s:", ab->process_name);
    scanf("%d", &size);
    if (size > 0) {
        ab->size = size;
    }
    ret = allocate_mem(ab);
    if (ret == 1 && allocated_block_head == NULL) {
        allocated_block_head = ab;
        return 1;
    }
    else if (ret == 1) {
        ab->next = allocated_block_head;
        allocated_block_head = ab;
        return 2;
    }
    else if (ret == -1) {
        printf("Allocation failed\n");
        free(ab);
        return -1;
    }
    return 3;
}

int allocate_mem(struct allocated_block* ab) {
    struct free_block_type* fbt, * pre;
    int request_size = ab->size;
    rearrange(ma_algorithm);
    fbt = pre = free_block;
    //补充 
    if (free_block == NULL) {
        return -1;
    }
    if (free_block->size > request_size) {  //头结点就适合
        fbt = free_block;
    }
    else {  //找后面的结点
        fbt = fbt->next;
        while (fbt) {
            if (fbt->size > request_size) {
                break;
            }
            pre = pre->next;
            fbt = fbt->next;
        }
    }
    if (fbt != NULL) {
        if (fbt->size - request_size >= MIN_SLICE) {
            fbt->size -= request_size;
            //增加一个已分配块
            ab->start_addr = fbt->start_addr;
            fbt->start_addr += request_size;
            rearrange(ma_algorithm);
            return 1;
        }
        else {  //需要删除这个空闲块

            //增加一个已分配块
            ab->start_addr = fbt->start_addr;
            ab->size = fbt->size;   //会产生碎片
            if (fbt == free_block) {   //要删的是头结点
                free_block = free_block->next;
            }
            else {  //要删的不是头结点
                pre->next = fbt->next;
            }
            struct free_block_type* tmp = fbt;
            fbt = fbt->next;
            free(tmp);
            tmp = NULL;
            rearrange(ma_algorithm);
            return 1;
        }
    }
    else {
        int total = total_free();
        if (total < request_size) {
            return -1;
        }
        else {
            memory_compaction(ab, total - request_size);
            //内存紧缩
            rearrange(ma_algorithm);
            return 1;
        }
    }
    return 1;
}

void memory_compaction(struct allocated_block* ab, int remain) {
    sort_free_block();
    struct allocated_block* cur = allocated_block_head;
    if (cur == NULL) {
        return;
    }
    int curBegin = 0;
    while (cur) {
        cur->start_addr = curBegin;
        curBegin += cur->size;
        cur = cur->next;
    }
    ab->start_addr = curBegin;
    free_block->start_addr = curBegin+ab->size;
    free_block->size = remain;
    free_block->next = NULL;
    return;
}

int total_free() {
    struct free_block_type* head = free_block;
    int total = 0;
    while (head) {
        total += head->size;
        head = head->next;
    }
    return total;
}

void kill_process() {
    struct allocated_block* ab;
    int pid;
    printf("KIll Process, pid=");
    scanf("%d", &pid);
    ab = find_process(pid);
    if (ab != NULL) {
        free_mem(ab);
        dispose(ab);
    }
    else {
        printf("Cannot find the Process %d!\n", pid);
    }
}

struct allocated_block* find_process(int pid) {
    struct allocated_block* cur = allocated_block_head;
    while (cur) {
        if (cur->pid == pid)
            break;
        cur = cur->next;
    }
    return cur;
}

int free_mem(struct allocated_block* ab) {
    int algorithm = ma_algorithm;
    struct free_block_type* fbt, * pre, * work;
    fbt = (struct free_block_type*)malloc(sizeof(struct free_block_type));
    if (!fbt) {
        return -1;
    }
    //补充
    // 进行可能的合并，基本策略如下
    // 1. 将新释放的结点插入到空闲分区队列末尾
    // 2. 对空闲链表按照地址有序排列
    // 3. 检查并合并相邻的空闲分区
    // 4. 将空闲链表重新按照当前算法排序
    fbt->start_addr = ab->start_addr;
    fbt->size = ab->size;
    fbt->next = NULL;
    pre = free_block;
    if (pre == NULL) {  //空闲块头结点为空
        free_block = fbt;
    }
    else {
        work = pre->next;
        while (work) {
            pre = pre->next;
            work = work->next;
        }
        pre->next = fbt;
    }
    sort_free_block();
    mergeFreeBlock();
    rearrange(ma_algorithm);

    return 1;
}

int dispose(struct allocated_block* free_ab) {
    struct allocated_block* pre, * ab;
    if (free_ab == allocated_block_head) {
        allocated_block_head = allocated_block_head->next;
        free(free_ab);
        return 1;
    }
    pre = allocated_block_head;
    ab = allocated_block_head->next;
    while (ab != free_ab) {
        pre = ab;
        ab = ab->next;
    }
    pre->next = ab->next;
    free(ab);
    return 2;
}

int display_mem_usage() {
    struct free_block_type* fbt = free_block;
    struct allocated_block* ab = allocated_block_head;
    /*if (fbt == NULL) {
        return -1;
    }*/
    printf("-----------------------------------------------------------------------\n");
    //显示空闲区
    printf("Free Memory:\n");
    printf("%20s %20s\n", "   start_addr", "   size");
    while (fbt != NULL) {
        printf("%20d %20d\n", fbt->start_addr, fbt->size);
        fbt = fbt->next;
    }
    //显示已经分配的区域
    printf("\nUsed Memory:\n");
    printf("%10s %20s %10s %10s\n", "PID", "ProcessName", "Start_addr", "size");
    while (ab != NULL) {
        printf("%10d %20s %10d %10d\n", ab->pid, ab->process_name, ab->start_addr, ab->size);
        ab = ab->next;
    }
    printf("-----------------------------------------------------------------------\n");
    return 0;
}

//对空闲链表按照地址有序排列
void sort_free_block() {
    struct free_block_type* first = free_block;
    if (first == NULL || first->next == NULL) {
        return;
    }
    struct free_block_type* second = NULL;
    while (first != second) {
        while (first->next != second) {
            if (first->start_addr > first->next->start_addr) {
                int tmp1 = first->start_addr;
                first->start_addr = first->next->start_addr;
                first->next->start_addr = tmp1;
                int tmp2 = first->size;
                first->size = first->next->size;
                first->next->size = tmp2;
            }
            first = first->next;
        }
        second = first;
        first = free_block;
    }
}
