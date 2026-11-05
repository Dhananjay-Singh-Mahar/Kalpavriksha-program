#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

struct SmartMemoryManager {
    size_t start;
    size_t size;
    int allocation_id;
    int is_allocated;
};

struct SmartMemoryManager *Block = NULL;
unsigned char *memory = NULL;

size_t pool_size;
int count = 1, capacity = 1;
int next_id = 1, strategy = 1;

int Grow_memory();
int Find_block(size_t size);
int Find_ID(int id);
void Merge_memory();

void Allocate_memory();
void Release_memory();
void Resize_allocation();
void Display_memory_map();
void Display_memory_statistics();
void Change_allocation_strategy();
void Detect_fragmentation(size_t size);

int Grow_memory() {
    if (count < capacity)
        return 1;

    int new_capacity = capacity * 2;

    struct SmartMemoryManager *temp =
        realloc(Block, new_capacity * sizeof(struct SmartMemoryManager));

    if (temp == NULL)
        return 0;

    Block = temp;
    capacity = new_capacity;

    return 1;
}

int Find_block(size_t size) {
    int selected = -1;

    for (int i = 0; i < count; i++) {
        if (!Block[i].is_allocated && Block[i].size >= size) {

            if (strategy == 1)
                return i;

            if (selected == -1 ||
                (strategy == 2 && Block[i].size < Block[selected].size) ||
                (strategy == 3 && Block[i].size > Block[selected].size)) {
                selected = i;
            }
        }
    }

    return selected;
}

int Find_ID(int id) {
    for (int i = 0; i < count; i++) {
        if (Block[i].is_allocated &&
            Block[i].allocation_id == id)
            return i;
    }

    return -1;
}

void Remove_block(int index) {
    for (int i = index; i < count - 1; i++)
        Block[i] = Block[i + 1];

    count--;
}

void Merge_memory() {
    for (int i = 0; i < count - 1;) {
        if (!Block[i].is_allocated &&
            !Block[i + 1].is_allocated) {

            Block[i].size += Block[i + 1].size;
            Remove_block(i + 1);
        }
        else
            i++;
    }
}

void Allocate_memory() {
    size_t size;

    printf("Enter requested size: ");

    if (scanf("%zu", &size) != 1 ||
        size == 0 || size > pool_size) {
        printf("Invalid size!\n");
        return;
    }

    if (next_id == INT_MAX) {
        printf("Allocation ID limit reached!\n");
        return;
    }

    int index = Find_block(size);

    if (index == -1) {
        printf("Allocation failed!\n");
        Detect_fragmentation(size);
        return;
    }

    size_t old_size = Block[index].size;
    size_t old_start = Block[index].start;

    if (old_size > size && !Grow_memory()) {
        printf("Memory allocation failed!\n");
        return;
    }

    Block[index].size = size;
    Block[index].allocation_id = next_id;
    Block[index].is_allocated = 1;

    memset(memory + old_start, 0, size);

    if (old_size > size) {
        for (int i = count; i > index + 1; i--)
            Block[i] = Block[i - 1];

        Block[index + 1].start = old_start + size;
        Block[index + 1].size = old_size - size;
        Block[index + 1].allocation_id = -1;
        Block[index + 1].is_allocated = 0;

        count++;
    }

    printf("Allocation successful!\n");
    printf("Allocation ID: %d\n", next_id);
    printf("Start: %zu\n", old_start);
    printf("Size: %zu\n", size);

    next_id++;
}

void Release_memory() {
    int id;

    printf("Enter allocation ID: ");

    if (scanf("%d", &id) != 1 || id <= 0) {
        printf("Invalid ID!\n");
        return;
    }

    int index = Find_ID(id);

    if (index == -1) {
        printf("Invalid allocation ID!\n");
        return;
    }

    memset(memory + Block[index].start, 0, Block[index].size);

    Block[index].is_allocated = 0;
    Block[index].allocation_id = -1;

    Merge_memory();

    printf("Memory released successfully!\n");
}

void Resize_allocation() {
    int id;
    size_t new_size;

    printf("Enter allocation ID: ");

    if (scanf("%d", &id) != 1 || id <= 0) {
        printf("Invalid ID!\n");
        return;
    }

    int index = Find_ID(id);

    if (index == -1) {
        printf("Invalid ID!\n");
        return;
    }

    printf("Enter new size: ");

    if (scanf("%zu", &new_size) != 1 ||
        new_size == 0 || new_size > pool_size) {
        printf("Invalid size!\n");
        return;
    }

    size_t old_size = Block[index].size;
    size_t old_start = Block[index].start;

    if (new_size == old_size) {
        printf("Size unchanged!\n");
        return;
    }

    if (new_size < old_size) {
        if (!Grow_memory()) {
            printf("Memory allocation failed!\n");
            return;
        }

        Block[index].size = new_size;

        for (int i = count; i > index + 1; i--)
            Block[i] = Block[i - 1];

        Block[index + 1].start = old_start + new_size;
        Block[index + 1].size = old_size - new_size;
        Block[index + 1].allocation_id = -1;
        Block[index + 1].is_allocated = 0;

        count++;
        Merge_memory();

        printf("Allocation reduced successfully!\n");
        return;
    }

    size_t extra = new_size - old_size;

    if (index + 1 < count &&
        !Block[index + 1].is_allocated &&
        Block[index + 1].size >= extra) {

        if (Block[index + 1].size == extra)
            Remove_block(index + 1);
        else {
            Block[index + 1].start += extra;
            Block[index + 1].size -= extra;
        }

        Block[index].size = new_size;

        memset(memory + old_start + old_size, 0, extra);

        printf("Expanded in place!\n");
        return;
    }

    int target = Find_block(new_size);

    if (target == -1) {
        printf("Resize failed!\n");
        Detect_fragmentation(new_size);
        return;
    }

    size_t target_start = Block[target].start;
    size_t target_size = Block[target].size;

    if (target_size > new_size && !Grow_memory()) {
        printf("Memory allocation failed!\n");
        return;
    }

    memmove(memory + target_start, memory + old_start, old_size);

    memset(memory + target_start + old_size, 0, extra);

    Block[target].size = new_size;
    Block[target].allocation_id = id;
    Block[target].is_allocated = 1;

    if (target_size > new_size) {
        for (int i = count; i > target + 1; i--)
            Block[i] = Block[i - 1];

        Block[target + 1].start = target_start + new_size;
        Block[target + 1].size = target_size - new_size;
        Block[target + 1].allocation_id = -1;
        Block[target + 1].is_allocated = 0;

        count++;
    }

    for (int i = 0; i < count; i++) {
        if (Block[i].start == old_start &&
            Block[i].allocation_id == id &&
            Block[i].is_allocated) {

            Block[i].is_allocated = 0;
            Block[i].allocation_id = -1;
            break;
        }
    }

    Merge_memory();

    printf("Allocation moved successfully!\n");
    printf("New start: %zu\n", target_start);
}

void Display_memory_map() {
    printf("\nMemory Map (%zu bytes)\n", pool_size);

    for (int i = 0; i < count; i++) {
        if (Block[i].is_allocated)
            printf("[Allocated #%d: %zu bytes, Start: %zu]\n",
                   Block[i].allocation_id,
                   Block[i].size,
                   Block[i].start);
        else
            printf("[Free: %zu bytes, Start: %zu]\n",
                   Block[i].size,
                   Block[i].start);
    }
}

void Display_memory_statistics() {
    size_t allocated = 0, free_memory = 0, largest = 0;
    int allocated_blocks = 0, free_blocks = 0;

    for (int i = 0; i < count; i++) {
        if (Block[i].is_allocated) {
            allocated += Block[i].size;
            allocated_blocks++;
        }
        else {
            free_memory += Block[i].size;
            free_blocks++;

            if (Block[i].size > largest)
                largest = Block[i].size;
        }
    }

    printf("\nTotal memory: %zu bytes\n", pool_size);
    printf("Allocated: %zu bytes\n", allocated);
    printf("Free: %zu bytes\n", free_memory);
    printf("Allocated blocks: %d\n", allocated_blocks);
    printf("Free blocks: %d\n", free_blocks);
    printf("Largest free block: %zu bytes\n", largest);
}

void Detect_fragmentation(size_t requested) {
    size_t free_memory = 0, largest = 0;

    printf("Free block sizes: ");

    for (int i = 0; i < count; i++) {
        if (!Block[i].is_allocated) {
            printf("%zu ", Block[i].size);
            free_memory += Block[i].size;

            if (Block[i].size > largest)
                largest = Block[i].size;
        }
    }

    printf("\n");

    if (free_memory >= requested && largest < requested)
        printf("External fragmentation detected!\n");
}

void Change_allocation_strategy() {
    int choice;

    printf("\n1. First Fit\n2. Best Fit\n3. Worst Fit\n");
    printf("Enter strategy: ");

    if (scanf("%d", &choice) != 1 ||
        choice < 1 || choice > 3) {
        printf("Invalid strategy!\n");
        return;
    }

    strategy = choice;
    printf("Strategy changed successfully!\n");
}

int main() {
    printf("Enter Memory Pool Size: ");

    if (scanf("%zu", &pool_size) != 1 ||
        pool_size == 0 || pool_size > 1000000) {
        printf("Invalid pool size!\n");
        return 1;
    }

    memory = malloc(pool_size);
    Block = malloc(sizeof(struct SmartMemoryManager));

    if (memory == NULL || Block == NULL) {
        printf("Memory allocation failed!\n");
        free(memory);
        free(Block);
        return 1;
    }

    memset(memory, 0, pool_size);

    Block[0].start = 0;
    Block[0].size = pool_size;
    Block[0].allocation_id = -1;
    Block[0].is_allocated = 0;

    printf("\n1. First Fit\n2. Best Fit\n3. Worst Fit\n");
    printf("Select strategy: ");

    if (scanf("%d", &strategy) != 1 ||
        strategy < 1 || strategy > 3) {
        printf("Invalid strategy!\n");
        free(Block);
        free(memory);
        return 1;
    }

    int choice;

    do {
        printf("\n1. Allocate memory\n");
        printf("2. Release memory\n");
        printf("3. Resize allocation\n");
        printf("4. Display memory map\n");
        printf("5. Display memory statistics\n");
        printf("6. Change allocation strategy\n");
        printf("7. Exit\n");

        printf("Choice: ");

        if (scanf("%d", &choice) != 1)
            break;

        switch (choice) {
            case 1:
                Allocate_memory();
                break;

            case 2:
                Release_memory();
                break;

            case 3:
                Resize_allocation();
                break;

            case 4:
                Display_memory_map();
                break;

            case 5:
                Display_memory_statistics();
                break;

            case 6:
                Change_allocation_strategy();
                break;

            case 7:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid Input!\n");
        }

    } while (choice != 7);

    free(Block);
    free(memory);

    printf("Memory released successfully!\n");

    return 0;
}