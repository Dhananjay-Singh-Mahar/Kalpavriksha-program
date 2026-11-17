#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 50

typedef struct FreeBlock{
    int index;

    struct FreeBlock *next;
    struct FreeBlock *prev;
}FreeBlock;

typedef struct FileNode{
    char name[MAX_NAME+1];
    bool isDirectory;

    struct FileNode *parent;
    struct FileNode *child;

    struct FileNode *next;
    struct FileNode *prev;

    int *blockPointers;
    int blockCount;

    size_t contentSize;
}FileNode;

unsigned char **virtualDisk = NULL;

FreeBlock *head = NULL;
FreeBlock *tail = NULL;

FileNode *root = NULL;
FileNode *cwd = NULL;

bool blockAllocated[NUM_BLOCKS] = {false};

bool initializeVirtualDisk(){

    virtualDisk = malloc(NUM_BLOCKS * sizeof(unsigned char *));

    if(virtualDisk == NULL) return false;

    for(int i =0; i < NUM_BLOCKS; i++){
        virtualDisk[i] = calloc(BLOCK_SIZE, sizeof(unsigned char));

        if(virtualDisk[i] == NULL){
            for(int j = 0; j < i; j++){
                free(virtualDisk[j]);
            }
            free(virtualDisk);
            virtualDisk = NULL;

            return false;
        }
    }

    return true;
}

bool initializeFreeList(){
    memset(blockAllocated, false, sizeof(blockAllocated));

    head = NULL;
    tail = NULL;

    for(int i = 0; i < NUM_BLOCKS; i++){
        FreeBlock *newBlock = malloc(sizeof(FreeBlock));

        if(newBlock == NULL){
            FreeBlock *current = head;
            while(current!=NULL){
                FreeBlock *next = current->next;
                free(current);
                current = next;
            }
            head = NULL;
            tail = NULL;
            return false;
        } 

        newBlock->index = i;
        newBlock->next = NULL;
        newBlock->prev = tail;

        if(head==NULL) head = newBlock;
        else tail->next = newBlock;

        tail = newBlock;
    }
    return true;
}

FileNode *createNode(const char *name, bool isDirectory){
    FileNode *newNode = malloc(sizeof(FileNode));

    if(newNode == NULL) return NULL;

    strcpy(newNode->name, name);
    newNode->isDirectory = isDirectory;

    newNode->parent = NULL;
    newNode->child = NULL;

    newNode->next = NULL;
    newNode->prev = NULL;

    newNode->blockPointers = NULL;
    newNode->blockCount = 0;

    newNode->contentSize = 0;

    return newNode;
}

bool intializeRoot(){
    root = createNode("/",true);
    if(root == NULL) return false;

    cwd = root;
    return true;
}

void cleanupVirtualDisk(){
    if(virtualDisk == NULL) return;

    for(int i=0; i < NUM_BLOCKS; i++){
            free(virtualDisk[i]);
        }
    free(virtualDisk);
    virtualDisk = NULL;
}

void cleanupFreeList(){
    FreeBlock *current = head;
    while(current!=NULL){
        FreeBlock *next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
    tail = NULL;
}

bool initialzeVFS(){
    if(!initializeVirtualDisk()){
        printf("Virtual Disk initialization failed.\n");
        return false;
    }

    if(!initializeFreeList()){
        printf("Free list initialization failed.\n");
        cleanupVirtualDisk();
        return false;
    }

    if(!intializeRoot()){
        printf("Root initialization failed.\n");
        cleanupVirtualDisk();
        cleanupFreeList();
        return false;
    }

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    return true;
}

void insertChild(FileNode *parent, FileNode *child){

    child->parent = parent;

    if(parent->child == NULL){
        parent->child = child;

        child->next = child;
        child->prev = child;
    }
    else{
        FileNode *last = parent->child->prev;

        last->next = child;
        child->prev = last;

        child->next = parent->child;
        parent->child->prev=child;
    }
}

bool removeChild(FileNode *parent, FileNode *child){
    if(parent == NULL || child == NULL) return false;

    if(child->parent != parent) return false;

    if(child->isDirectory && child->child != NULL){
        printf("Directory is not empty.\n");
        return false;
    }

    if(child->next == child){
        parent->child = NULL;
    }
    else if(child == parent->child){
        FileNode *last = child->prev;

        parent->child = child->next;
        child->next->prev = last;
        last->next = child->next;
    }
    else{
        child->prev->next = child->next;
        child->next->prev = child->prev;
    }

    child->parent = NULL;
    child->next = NULL;
    child->prev= NULL;

    return true;
}

FileNode *findChild(FileNode *parent, const char *name){
    if(parent == NULL || name == NULL || parent->child == NULL) return NULL;

    FileNode *current = parent->child;

    do{
        if(strcmp(current->name,name)==0) return current;

        current = current->next;
    }while(current != parent->child);

    return NULL;
}

void mkdir(const char *name){
    if(name == NULL || strlen(name)==0 || strlen(name) > MAX_NAME){
        printf("Invalid Directory name.\n");
        return;
    }

    if(findChild(cwd, name)!=NULL){
        printf("Name already exists.\n");
        return;
    }

    FileNode *newDir = createNode(name, true);

    if(newDir == NULL){
        printf("Memory allocation failed.\n");
        return;
    }

    insertChild(cwd, newDir);
    printf("Directory created successfully.\n");
}

int allocateBlock(){
    if(head==NULL) return -1;

    int blockIndex = head->index;

    if (blockIndex < 0 || blockIndex >= NUM_BLOCKS ||
        blockAllocated[blockIndex]) {
        return -1;
    }

    FreeBlock *allcatedBlock = head;

    head = head->next;
    if(head != NULL){
        head->prev = NULL;
    }
    else{
        tail = NULL;
    }

    free(allcatedBlock);
    blockAllocated[blockIndex] = true;

    return blockIndex;
}

void create(const char *name){
    if(name == NULL || strlen(name) == 0 || strlen(name) > MAX_NAME){
        printf("Invalid file name.\n");
        return;
    }
    if(findChild(cwd,name)!=NULL){
        printf("Name already exists.\n");
        return;
    }

    FileNode *newFile = createNode(name,false);

    if(newFile == NULL){
        printf("Memory allocation failed.\n");
        return;
    }

    insertChild(cwd,newFile);

    printf("File Created Successfully.\n");
}

bool releaseBlocks(int index){
    if (index < 0 || index >= NUM_BLOCKS || !blockAllocated[index]) {
        return false;
    }

    FreeBlock *newBlock = malloc(sizeof(FreeBlock));

    if(newBlock == NULL){
        printf("Memory allcation failed.\n");
        return false;
    }
    newBlock->index = index;
    newBlock->prev = tail;
    newBlock->next = NULL;
    if(head == NULL){
        head = newBlock;
    }else{
        tail->next = newBlock;
    }
    tail = newBlock;
    blockAllocated[index] = false;
    
    return true;
}

int countFreeBlocks(){
    int count = 0;
    FreeBlock *temp = head;
    while(temp != NULL){
        count++;
        temp = temp->next;
    }

    return count;
}

void write(const char *name, const char *content){
    if(content == NULL || name == NULL || strlen(name)<=0 || strlen(name) > MAX_NAME){
        printf("Invalid name or content.\n");
        return;
    }
    FileNode *file = findChild(cwd, name);

    if(file == NULL){
        printf("File not found.\n");
        return;
    }

    if(file->isDirectory){
        printf("Cannot write to directory.\n");
        return;
    }

    if (file->blockPointers != NULL || file->blockCount != 0) {
        printf("File already contains data.\n");
        return;
    }

    size_t contentSize = strlen(content);
    if(contentSize == 0){
        printf("empty content. \n");
        return;
    }

    size_t storageSize = contentSize + 1;
    size_t requiredBlocks = (storageSize + BLOCK_SIZE-1)/BLOCK_SIZE;

    if(countFreeBlocks() < requiredBlocks){
        printf("Not enough disk space.\n");
        return;
    }

    int *blockPointers = malloc(requiredBlocks*sizeof(int));

    if(blockPointers == NULL){
        printf("Memory allocation failed.\n");
        return;
    }
     for(int i=0; i < requiredBlocks; i++){
        int blockIndex = allocateBlock();
        if(blockIndex == -1){
            for (int j = 0; j < i; j++) {
                if (!releaseBlocks(blockPointers[j])) {
                printf("Warning: Could not release block %d.\n", blockPointers[j]);
                }
            }
            free(blockPointers);
            blockPointers = NULL;

            return;
        }
        blockPointers[i] = blockIndex;
     }

     for (size_t i = 0; i < storageSize; i++) {

        size_t blockPosition = i / BLOCK_SIZE;
        size_t offset = i % BLOCK_SIZE;

        int diskBlock = blockPointers[blockPosition];

        virtualDisk[diskBlock][offset] = content[i];
    }

     file->contentSize = contentSize;
     file->blockPointers = blockPointers;
     file->blockCount = requiredBlocks;

     printf("File written successfully.\n");
}

void read(const char *name) {

    if (name == NULL || name[0] == '\0' ||
        strlen(name) > MAX_NAME) {
        printf("Invalid file name.\n");
        return;
    }

    FileNode *file = findChild(cwd, name);

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    if (file->isDirectory) {
        printf("Cannot read a directory.\n");
        return;
    }

    if (file->blockPointers == NULL || file->blockCount == 0) {
        printf("File is empty.\n");
        return;
    }

     for (size_t i = 0; i < file->contentSize; i++) {

        size_t blockPosition = i / BLOCK_SIZE;
        size_t offset = i % BLOCK_SIZE;

        if (blockPosition >= (size_t)file->blockCount) {
            printf("\nFile metadata is corrupted.\n");
            return;
        }

        int diskBlock = file->blockPointers[blockPosition];

        if (diskBlock < 0 || diskBlock >= NUM_BLOCKS) {
            printf("\nInvalid disk block index.\n");
            return;
        }

        putchar(virtualDisk[diskBlock][offset]);
    }

    printf("\n");
}

void delete(const char *name) {

    if (name == NULL || name[0] == '\0' ||
        strlen(name) > MAX_NAME) {
        printf("Invalid file name.\n");
        return;
    }

    FileNode *file = findChild(cwd, name);

    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    if (file->isDirectory) {
        printf("Cannot delete a directory using delete.\n");
        return;
    }

   FileNode *parent = file->parent;

if (!removeChild(parent, file)) {
    printf("Could not remove file from directory.\n");
    return;
}

bool releaseFailed = false;

for (int i = 0; i < file->blockCount; i++) {
    if (!releaseBlocks(file->blockPointers[i])) {
        releaseFailed = true;

        printf("Warning: Could not release block %d.\n",
               file->blockPointers[i]);
    }
}

free(file->blockPointers);
file->blockPointers = NULL;
file->blockCount = 0;
file->contentSize = 0;

free(file);

    if (releaseFailed) {
        printf("File deleted, but some disk blocks could not be released.\n");
    } else {
        printf("File deleted successfully.\n");
    }
}

void rmdir(const char *name) {

    if (name == NULL || name[0] == '\0' ||
        strlen(name) > MAX_NAME) {
        printf("Invalid directory name.\n");
        return;
    }

    FileNode *dir = findChild(cwd, name);

    if (dir == NULL) {
        printf("Directory not found.\n");
        return;
    }

    if (!dir->isDirectory) {
        printf("The specified name is not a directory.\n");
        return;
    }

    if (dir == root) {
        printf("Cannot remove root directory.\n");
        return;
    }

    if (dir->child != NULL) {
        printf("Directory is not empty.\n");
        return;
    }

    FileNode *parent = dir->parent;

    if (!removeChild(parent, dir)) {
        printf("Could not remove directory.\n");
        return;
    }

    free(dir);

    printf("Directory removed successfully.\n");
}

void ls() {

    if (cwd == NULL) {
        printf("Invalid current directory.\n");
        return;
    }

    if (cwd->child == NULL) {
        printf("Directory is empty.\n");
        return;
    }

    FileNode *current = cwd->child;

    do {

        printf("%s", current->name);

        if (current->isDirectory) {
            printf("/");
        }

        printf("\n");

        current = current->next;

    } while (current != cwd->child);
}

void cd(const char *name) {

    if (name == NULL || name[0] == '\0') {
        printf("Invalid directory name.\n");
        return;
    }

    if (strcmp(name, "/") == 0) {
        cwd = root;
        return;
    }

    if (strcmp(name, "..") == 0) {

        if (cwd->parent != NULL) {
            cwd = cwd->parent;
        }

        return;
    }

    FileNode *dir = findChild(cwd, name);

    if (dir == NULL) {
        printf("Directory not found.\n");
        return;
    }

    if (!dir->isDirectory) {
        printf("Cannot change directory to a file.\n");
        return;
    }

    cwd = dir;
}

void pwd() {

    if (cwd == NULL) {
        printf("Invalid current directory.\n");
        return;
    }

    FileNode *path[NUM_BLOCKS];
    int count = 0;

    FileNode *current = cwd;

    while (current != NULL) {

        if (count >= NUM_BLOCKS) {
            printf("Directory path is too deep.\n");
            return;
        }

        path[count++] = current;
        current = current->parent;
    }

    for (int i = count - 1; i >= 0; i--) {

        if (path[i] == root) {
            printf("/");
        } else {
            printf("%s", path[i]->name);

            if (i > 0) {
                printf("/");
            }
        }
    }

    printf("\n");
}

void df() {

    int freeBlocks = countFreeBlocks();
    int usedBlocks = NUM_BLOCKS - freeBlocks;

    size_t totalStorage = (size_t)NUM_BLOCKS * BLOCK_SIZE;
    size_t usedStorage = (size_t)usedBlocks * BLOCK_SIZE;
    size_t freeStorage = (size_t)freeBlocks * BLOCK_SIZE;

    printf("\n========== Disk Usage ==========\n");

    printf("Total blocks : %d\n", NUM_BLOCKS);
    printf("Used blocks  : %d\n", usedBlocks);
    printf("Free blocks  : %d\n", freeBlocks);

    printf("\nTotal storage: %.2f KB\n",
           totalStorage / 1024.0);

    printf("Used storage : %.2f KB\n",
           usedStorage / 1024.0);

    printf("Free storage : %.2f KB\n",
           freeStorage / 1024.0);

    printf("================================\n");
}

void freeFileTree(FileNode *dir) {

    if (dir == NULL) {
        return;
    }

    FileNode *first = dir->child;

    if (first != NULL) {

        FileNode *current = first;

        do {
            FileNode *next = current->next;

            if (current->isDirectory) {
                freeFileTree(current);
            } else {
                free(current->blockPointers);
            }

            free(current);

            current = next;

        } while (current != first);
    }
}

void cleanupVFS() {

    if (root != NULL) {
        freeFileTree(root);
        free(root);
        root = NULL;
        cwd = NULL;
    }

    cleanupFreeList();
    cleanupVirtualDisk();

    printf("VFS memory cleaned successfully.\n");
}

int main() {

    if (!initialzeVFS()) {
        printf("VFS initialization failed.\n");
        return 1;
    }

    char input[1024];

    while (true) {

        printf("\nVFS> ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        input[strcspn(input, "\n")] = '\0';

        // Ignore empty input
        if (input[0] == '\0') {
            continue;
        }

        // Exit command
        if (strcmp(input, "exit") == 0) {
            break;
        }

        printf("Command processing is not implemented yet.\n");
    }

    cleanupVFS();

    return 0;
}