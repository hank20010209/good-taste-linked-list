# Indirect Pointer in Linked List
## 前言
受到 [你所不知道的 C 語言: linked list 和非連續記憶體](https://hackmd.io/@sysprog/c-linked-list) 的啟發，記錄對於 Indirect Pointer 的學習歷程，並嘗試使用 Rust 實現此概念。
## 直觀的 Linked List 實作
首先定義 Linked List 資料結構，定義節點本身以及 Linked List 本身
```c
typedef struct Node {
    int value;
    struct Node *next;
}Node;
```
```c
typedef struct List {
    Node *head;
}List;
```

### init_list
> To init the Linked List
```c
List *init_list()
{
    List *l = (List *)malloc(sizeof(List));
    l->head = NULL;
    return l;
}
```
初始化 Linked List，並將初始化完成的 Linked List 回傳

### find_node
> Find Node in Linked List
> Return the pointer point to Node if in Linked List
> Return NULL  pointer if not in Linked List
```c
Node *find_node(List *list, int target)
{
    Node *temp = list->head;
    while(temp && temp->value != target) {
        temp = temp->next;
    }
    return temp;
}
```
### insert_tail
> insert Node to the tail of Linked List
```c
void insert_tail(List *list, int value)
{
    Node *curr = list->head;
    Node *prev = NULL;

    Node* new = (Node *)malloc(sizeof(Node));
    new->next = NULL;
    new->value = value;

    while(curr) {
        prev = curr;
        curr = curr->next;
    }
    if(!prev)
        list->head = new;
    else
        prev->next = new;
}
```
在 Linked List 的尾端插入節點，使用 `prev` 和 `curr` 去走訪節點，`prev` 為 `NULL` 的情況，表示 Linked List 沒有任何節點或是為 `NULL`，否則就向 `prev->next` 插入節點，圖像化如下

在 `prev` 不為 `NULL`，也就是 Linked List 不為空的情況下，存在節點的情況下

![](https://i.imgur.com/6IdhzOS.png)

在 `prev` 為 `NULL`，也就是 Linked List 為空，沒有節點的情況下

![](https://i.imgur.com/SnNkF5Z.png)

### remove_node
> Remove the Node in Linked List
> If target not in Linked List, return
```c
void remove_node(List *list, Node *target)
{
    if(!target) return;
    Node *prev = NULL;
    Node *curr = list->head;

    while(curr != target) {
        prev = curr;
        curr = curr->next;
    }
    
    if(!prev)
        list->head = target->next;
    else
        prev->next = target->next;
}
```
使用 `prev` 以及 `curr` 走訪 Linked List 找到目標節點，接下來分成兩種情況進行討論，分別為刪除的為頭節點或是其他節點

刪除其他節點的情況
![](https://i.imgur.com/PIrPweq.png)

刪除頭節點的情況
![](https://i.imgur.com/bpAicd7.png)

### 觀察 remove_node 以及 insert_tail
可以發現到在 `remove_node` 以及 `insert_tail` 都會出現特定情況需要進行處理，通過 `if-else` 判斷是否為頭節點，而以下將使用間接指標 (或稱為指標的指標) 對程式碼進行優化，除去對於特例的判斷

## Indirect Pointer 概念
前面 `remove_node` 的想法為將節點 `Node` 的記憶體地址儲存到 `curr` 和 `prev` 中，通過 `curr` 和 `prev` 走訪 Linked List，找到 `target` 時便停止。

而 Indirect Pointer 的想法為運用指向到節點記憶體地址的指標進行操作，原先的 `curr` 和 `prev` 中儲存的是節點本身的記憶體地址，反參考 `curr` 和 `prev` 我們能夠得到節點本身，而 Indirect Pointer 中儲存的是指向到節點記憶體地址的指標，如果我們對其進行反參考，我們會得到的是該節點的記憶體地址，概念說明如下

```c=
#include <stdio.h>
#include <stdlib.h>

void foo(int *a_ptr)
{
    a_ptr = NULL;
}

int main(void)
{
    int a = 3;
    int *a_ptr = &a;
    printf("%d\n", *a_ptr);
    foo(a_ptr);
    printf("%d\n", *a_ptr);
}
```
上面這一段程式碼，我們有一個指向到整數的指標變數 `a_ptr`，接著我們將指標變數傳入 `foo()` 函式，讓 `a_ptr` 由原先的指向變數 `a` 變為指向到 `NULL`。

我們可以通過第 15 行查看對 `a_ptr` 的修改是否成功，如果修改成功，則第 15 行會出現對空指標進行反參考的錯誤，造成記憶體區段錯誤。

以下為執行結果
```
3
3
```

可以發現我們並沒有成功修改 `a_ptr` 所指向的物件，原因為 C 語言的函式傳遞是以傳值進行的，傳入函式的參數會產生一份複本到函式中，而我們在函式中修改的參數，實際上修改的是剛剛產生的複本。舉一個簡單的例子
```c
#include <stdio.h>
#include <stdlib.h>

void foo(int a)
{
    a = 10;
}

int main(void)
{
    int a = 3;
    printf("%d\n", a);
    foo(a);
    printf("%d\n", a);
}
```
上面這個例子十分容易理解，我們在 `foo()` 中修改的 `a`，實際上是在函式中產生的複本，不會影響到 `main` 中的 `a`。
![](https://i.imgur.com/z4BX4BQ.png)

如果要針對 `a` 進行修改，我們需要傳入指向到 `a` 的指標變數，修改 `a` 記憶體地址中儲存的資料，範例如下
```c
#include <stdio.h>
#include <stdlib.h>

void foo(int *a)
{
    *a = 10;
}

int main(void)
{
    int a = 3;
    printf("%d\n", a);
    foo(&a);
    printf("%d\n", a);
}
```
圖解如下
![](https://i.imgur.com/2tkzpvA.png)

這裡可以注意到，對於 call by value，如果我們修改傳入函式中的值，實際上我們是修改到他的複本，但是我們可以修改他背後的資料，以 `*a` 而言，指標變數中儲存的是記憶體地址，而我們對其反參考可以得到記憶體地址儲存的資料，對於 call by value，我們無法修改變數中儲存的值，也就是我們無法修改指標中儲存的記憶體地址，但是我們可以通過反參考指標修改背後的資料。

對應到上面的範例，我們是傳入指標變數到 `foo()` 中，在 `foo()` 中我們得到 `*a` 的複本，稱為 `copy *a`，之後我們反參考 `copy *a`，修改記憶體地址指向的資料。

接著回到我們在一開始看到的例子
```c=
#include <stdio.h>
#include <stdlib.h>

void foo(int *a_ptr)
{
    a_ptr = NULL;
}

int main(void)
{
    int a = 3;
    int *a_ptr = &a;
    printf("%d\n", *a_ptr);
    foo(a_ptr);
    printf("%d\n", *a_ptr);
}
```
這裡我們嘗試修改變數的值，也就是修改指標變數儲存的記憶體地址，將其修改為 `NULL`，我們修改到的是變數複製出來的複本，而不是在 `main()` 中 `a_ptr` 本身，因此在第 15 行不會發生反參考空指標所導致的記憶體區段錯誤。

這裡整理一下，如果我們要對 `a` 進行修改，我們需要傳入 `*a`。而如果我們要修改 `*a`，我們會想到我們可能要傳入 `**a`，`*a` 為指標，而 `**a` 稱為指標的指標，又稱為間接指標。我們可以通過間接指標去修改指標變數中儲存的值，只需要反參考間接指標，修改其值即可，以下範例
```c
#include <stdio.h>
#include <stdlib.h>

void foo(int **a_ptr)
{
    *a_ptr = NULL;
}

int main(void)
{
    int a = 3;
    int *a_ptr = &a;
    printf("%d\n", *a_ptr);
    foo(&a_ptr);
    printf("%d\n", *a_ptr);
}
```
output:
```
3
Segmentation fault (core dumped)
```
從以上範例可以看到，我們通過間接指標，去修改指標內儲存的值。

而接下來我們將使用間接指標的手法，對 Linked List 的實作進行優化。

## 應用 Indirect Pointer 的 Linked List
### remove_node
前面 `remove_node` 的想法為我們使用一個指標變數，儲存該節點的記憶體地址，反參考後就可以得到節點本身。

而這邊使用間接指標的想法，我們儲存的指標，是指向到該節點的記憶體地址，如果我們反參考指標，得到的是節點的記憶體地址。

以下為實作部分
```c=
void remove_list_node(List *list, Node *target)
{
    Node **indirect = &list->head;
    
    while (*indirect != target)
        indirect = &(*indirect)->next;
    *indirect = target->next;
}
```
我們使用指向到 `Node` 指標的指標變數 `indirect`，裡面儲存指向到 `list->head` 的指標，也就是說 `indirect` 是一個指標的指標，也稱為間接指標，indirect is a pointer to pointer, which mean a pointer to your pointer value，your pointer value point to `Node`, so the type of pointer value is `Node *`, and the type of the pointer to your pointer value is `Node **`.

之後我們開始通過 `indirect` 去走訪 Linked List，`*indirect` 這邊為對 `indirect` 進行反參考，得到的會是節點的記憶體地址，型別為 `Node *`，通過節點的記憶體地址和 `target` 進行比較，`target` 同樣也是記憶體地址，意義為節點的記憶體地址。

第 6 行，首先我們對 `indirect` 進行反參考，得到 `Node *`，也就是 `Node` 的記憶體地址，通過箭頭運算子取得成員 `next`，成員 `next` 指向到下一個節點，型別為 `Node *`，意義為指向到節點的記憶體地址，接著我們對他進行取址，得到 `Node **`，意義為指向到指向到節點的記憶體地址，這邊聽起來有些繞口，概念上就是儲存的是指向到節點的記憶體地址。

舉一個簡單的例子
```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int a;
    int b;
}Node;

int main(void)
{
    Node a = {.a = 1, .b = 2};
    Node *a_ptr = &a;
    Node **a_ptr_ptr = &a_ptr;

    printf("%d", (*a_ptr_ptr)->a);
}
```
圖解為以下
![](https://i.imgur.com/8mH16MB.png)

指標變數本身也是一個變數，有自己的記憶體地址，只是該變數裡面儲存的值為記憶體地址
```c
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int a;
    int b;
}Node;

int main(void)
{
    Node a = {.a = 1, .b = 2};
    Node *a_ptr = &a;
    Node **a_ptr_ptr = &a_ptr;

    printf("Address of Node a: %-10x\n\n", &(a));
    
    printf("Context of a_ptr: %-10x\n", a_ptr);
    printf("Dereference of a_ptr: %-10x\n", *a_ptr);
    printf("Address of a_ptr: %-10x\n\n", &(a_ptr));

    printf("Context of a_ptr_ptr: %-10x\n", a_ptr_ptr);
    printf("Dereference of a_ptr_ptr: %-10x\n", *a_ptr_ptr);
    printf("Address of a_ptr_ptr: %-10x\n", &(a_ptr_ptr));
}
```

output
```
Address of Node a: 61ff18    

Context of a_ptr: 61ff18
Dereference of a_ptr: 1
Address of a_ptr: 61ff14

Context of a_ptr_ptr: 61ff14        
Dereference of a_ptr_ptr: 61ff18    
Address of a_ptr_ptr: 61ff10
```
![](https://i.imgur.com/VjdQ25M.png)

`Node a` 的記憶體地址為 `0x61ff18`。

`Node *a_ptr` 為指標變數，記憶體地址為 `0x61ff14`，內容為 `0x61ff18`，為一個指向到 `Node` 的指標，對 `a_ptr` 進行反參考，意味著存取 `a_ptr` 指標變數內容所指向的資料，也就是 `0x61ff18` 所指向的資料，存取到 `Node a` 的第一個成員，得到 1。

`Node **a_ptr_ptr` 為指標變數，記憶體地址為 `0x61ff10`，內容為 `0x61ff14`，為一個指向到 `Node *` 的指標，對 `a_ptr_ptr` 進行反參考，意味著存取 `a_ptr_ptr` 指標變數內容所指向的資料，也就是 `0x61ff14` 所指向的資料，`0x61ff14` 中的資料為 `0x61ff18`。


將這個概念應用在 `remove_list_node` 中，圖解如下

這是初始情況，`indirect` 為一個指標的指標，也就是 `indirect` 為一個指標變數，儲存指向到節點的指標。(加上記憶體地址方便理解)
![](https://i.imgur.com/udqaR8S.png)

不同於原先的實作，指向到節點本身，這邊是儲存指向到節點的指標，概念上為儲存要更新的位置，而不是要更新的節點本身。反參考 `indirect` 得到的是指向到節點的記憶體地址。

接著看到走訪，假設目前情況如下
![](https://i.imgur.com/fh9rxIF.png)


首先先對 `indirect` 進行反參考，會得到節點的下一個節點的指標，概念上為接下來我們要更新的地址
![](https://i.imgur.com/FQztUmR.png)

接著我們通過箭頭運算子存取成員 `next`，也就是 `(*indirect)->next`
![](https://i.imgur.com/aqoYayc.png)

接著我們取出指向到下一個節點的指標，也就是取出指向 `Node *` 的指標 
`&(*indirect)->next`，型別為 `Node **`
![](https://i.imgur.com/eYB6zOh.png)


:::info
由走訪節點的過程中我們可以觀察到一件事情，`indirect` 所維護的，是我們要更新的位置，我們接下來要更新的位置是儲存在 `next` 中的，型別為 `Node *`，而我們要存取要更新的位置，因此需要使用 `Node **` 進行操作。

在上面的實作中，我們可以通過 `*indirect` 來得到 `Node *`，進而去存取下一個節點的資訊。

:::

如果 `*indirect` 就是我們要刪除的節點，也就是 `*indirect == target`，則圖解為以下
![](https://i.imgur.com/tuh69YT.png)

我們可以通過 `*indirect` 得到要刪除的節點本身，也就是 `target`，型別為 `Node *`，目前 `*indirect` 的內容為前一個節點指向的下一個節點，於是，我們可以直接通過修改 `*indirect` 去改變前一個節點所指向的下一個節點，概念上可以理解為修改前一個節點中，`next` 的內容。

`*indirect = target->next` 的意義為將前一個節點的下一個節點改變為 `target` 的下一個節點。

![](https://i.imgur.com/3pDFN2z.png)

這時候的 `*indirect` 為以下
![](https://i.imgur.com/vHNKHqw.png)

可以注意到，實際上 `target` 並沒有被我們移除。

#### remove_node 頭節點情況
在最一開始的 `while` 迴圈判斷時，會先判斷 `*indirect != target`，而我們最一開始條件便不成立了，因此不會進入到 `while` 迴圈，目前圖解如下
![](https://i.imgur.com/TsYeqrv.png)

接著我們通過 `*indirect` 獲得第一個節點本身，型別為 `Node *`，第一個節點本身也就是 `list_head`，接著我們將 `*indirect` 設為頭節點的下一個節點，也就是將 `*indirect` 設為 `target->next`，圖解如下
![](https://i.imgur.com/hecyskV.png)

### indirect pointer vs direct pointer
我們在最一開始直覺上的實作，是使用 direct pointer 的方式，直接針對節點本身進行操作，但是會有一些例外狀況需要處理，如頭節點為特殊情況。

而 indirect pointer 的想法為我們針對接下來要更新的位置進行判斷，好處是不會有例外情況進行處理，因為我們並沒有直接針對頭節點進行操作。

### [Leetcode 21. Merge Two Sorted Lists](https://leetcode.com/problems/merge-two-sorted-lists/description/)
給定兩個已經完成排序的單向鏈結串列，請將這兩個鏈結串列進行合併，並回傳合併完成的鏈結串列頭節點。

直覺上的做法，是建立一個頭節點，接著我們開始走訪 `list1` 和 `list2`，將 `val` 較小的節點串到我們建立的節點上，完成後回傳我們建立頭節點的下一個節點 (我們建立的頭節點概念上是一個 dummy node)

![](https://i.imgur.com/zCJbFJ2.png)

以下為該實作程式碼
```c
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2){
    struct ListNode *head = (struct ListNode *)malloc(sizeof(struct ListNode));
    struct ListNode *ptr = head;

    while(list1 && list2) {
        if(list1->val < list2->val) {
            ptr->next = list1;
            list1 = list1->next;
        }
        else {
            ptr->next = list2;
            list2 = list2->next;
        }
        ptr = ptr->next;
    }
    ptr->next = list1 ? list1 : list2; 
    return head->next;
}
```

這裡我們的 `head` 使用 `malloc` 配置了一塊記憶體空間，而我們可以使用間接指標的方法優化這一段空間，讓我們不用使用 `malloc` 去對 `head` 進行記憶體空間的分配

前面的想法為 `head` 為一個實際存在的節點，而我們通過 `ptr` 去維護 `head` 後面串接的節點，但實際上 `head` 為一個 dummy node，內容沒有意義，只是為了讓我們串接後面的節點。

這邊使用間接指標的想法，概念上是改變 `head` 所指向的節點，直接修改 `head` 的內容，我們直接宣告一個 `head` pointer，指向到節點，並初始化為 NULL，之後我們使用 `**ptr = &head`，`ptr` 裡面儲存的，是 `head` 這個指標變數本身的指標，反參考 `ptr` 會得到 `head` 指標變數所在的記憶體地址。

我們可以直接通過 `*ptr = some address` 直接改變 `head` 的內容，也就是 `head` 所儲存的指向到節點的記憶體地址。

接著我們可以通過 `&(*ptr)->next` 得到指向 `head` 下一個節點的指標的指標。

以下為初始情況

![](https://i.imgur.com/0y1vXur.png)

加入第一個節點 (直接修改 `head` 的內容，也就是讓 `head` 直接指向某一個節點)，`(*ptr) = list?`

![](https://i.imgur.com/XYsbVfV.png)

通過 `(*ptr)` 存取到節點本身的指標，通過箭頭運算子存取成員

![](https://i.imgur.com/2taDQHU.png)

這裡的概念如同前面提及的 `remove_node` 一樣，我們是維護記憶體地址本身，而非節點本身。

實作如下
```c
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2){
    struct ListNode *head;
    struct ListNode **ptr = &head;

    while(list1 && list2) {
        if(list1->val < list2->val) {
            *ptr = list1;
            list1 = list1->next;
        }
        else {
            *ptr = list2;
            list2 = list2->next;
        }
        ptr = &(*ptr)->next;
    }
    (*ptr) = list1 ? list1 : list2; 
    return head;
}
```
接著觀察到 `if-else` 的部分，發現要做的操作都是反參考 `ptr`，指向到下一個要接上的節點，接著更新到下一個節點，我們可以使用一個指標變數去指向 `L1` 或是 `L2`。

```c
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2){
    struct ListNode *head;
    struct ListNode **ptr = &head;
    struct ListNode **node;
    while(list1 && list2) {
        node = (list1->val < list2->val) ? (&list1) : (&list2);
        *ptr = *node;
        ptr = &(*ptr)->next;
        *node = (*node)->next;
    }
    (*ptr) = list1 ? list1 : list2; 
    return head;
}
```
使用 `for` 迴圈可以減少一些行數
```c
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2){
    struct ListNode *head = NULL, **ptr = &head, **node = NULL;
    
    for(; list1 && list2; *node = (*node)->next, ptr = &(*ptr)->next) {
        node = (list1->val < list2->val) ? (&list1) : (&list2);
        *ptr = *node;
    }
    (*ptr) = list1 ? list1 : list2; 
    return head;
}
```
雖然可以將操作寫入 for 迴圈中，但程式碼將不易閱讀
```c
struct ListNode* mergeTwoLists(struct ListNode* list1, struct ListNode* list2){
    struct ListNode *head = NULL, **ptr = &head, **node = NULL;
    for(; list1 && list2; node = (list1->val < list2->val) ? (&list1) : (&list2), *ptr = *node, *node = (*node)->next, ptr = &(*ptr)->next);
    return (*ptr) = list1 ? list1 : list2, head;
}
```

### [Leetcode 2095. Delete the Middle Node of a Linked List](https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/description/)
給定一單向鏈結串列，刪除中間的節點後回傳

直覺做法
```c
struct ListNode* deleteMiddle(struct ListNode* head){
    struct ListNode* slow_prev;
    struct ListNode* slow = head;
    struct ListNode* fast = head;
    
    if(!head->next) return NULL;
    while(fast && fast->next){
        slow_prev = slow;
        slow = slow->next;
        fast = fast->next->next;
    }
    slow_prev->next = slow->next;
    return head;
}
```
我們可以嘗試使用間接指標，上面的作法次我們使用一個指標去指向到節點，如果對該指標進行反參考，我們會得到節點本身，而使用間接指標，我們要操作的對象是針對節點中 `next` 這個成員，要修改 `next`，我們需要使用 `Node **` (指向到 `next`) 來對 `next` 進行操作，也就是間接指標，以下範例
```c
struct ListNode* deleteMiddle(struct ListNode* head){
    struct ListNode **indirect = &head;
    for(struct ListNode *fast = head; fast && fast->next; fast = fast->next->next)
        indirect = &(*indirect)->next;
    *indirect = (*indirect)->next;
    return head;
}
```
這邊走訪時概念和上方刪除節點的概念相同，都是針對接下來要操作的位置進行操作。
![](https://i.imgur.com/tKSrMCD.png)

這邊同樣可以發現，實際上我們沒有將節點移除，這邊我們嘗試將節點占用的記憶進行釋放。

我們需要維護一個指標，指向到 `*indirect` 的前一個節點

```c
struct ListNode* deleteMiddle(struct ListNode* head){
    struct ListNode **indirect = &head;
    struct ListNode *prev = NULL;
    for(struct ListNode *fast = head; fast && fast->next; fast = fast->next->next) {
        prev = *indirect;
        indirect = &(*indirect)->next;
    }
    prev->next = (*indirect)->next;
    free(*indirect);
    return head;
}
```
反參考 `indirect` 得到的是下一個節點，而在 `indirect` 更新之前，我們先使用一個指標儲存 `indirect`，達到儲存前一個節點的目標。

迴圈結束後，`*indirect` 為中間節點，因此只要我們將 `prev->next = (*indirect)->next` 便完成移除中間節點的操作了，之後我們再將 `*indirect` 使用 `free()` 進行釋放便完成操作，以給定單向鏈結串列 `[1,3,4,7,1,2,6]` 為例，在迴圈走訪結束後，`prev` 指向節點的值為 4，`*indirect` 指向節點的值為 7。

![](https://i.imgur.com/Pqp0XLC.png)

接著我們把 `prev` 的下一個節點設為 `*indirect` 的下一個節點，這邊其實 `prev->next = (*indirect)->next` 的意義等同於 `(*indirect) = (*indirect)->next`，相當於把指向到 7 的節點設為指向到 1 的節點，也就是，這邊 `*indirect` 的內容為 1
![](https://i.imgur.com/fwpy7lp.png)

接著我們會將 `*indirect` 進行釋放，可以發現到我們將指向到 1 的節點進行釋放了，這會導致我們在後續走訪的時候存取到空指標。

我們試著執行後，會發現 leetcode 中的 AddressSanitizer 會發出 heap-use-after-free 的錯誤。

要解決這個問題，我們可以使用一個指標去存放 `(*indirect)->next`，實作如下
```c
struct ListNode* deleteMiddle(struct ListNode* head){
    struct ListNode **indirect = &head;
    struct ListNode *prev = NULL;
    
    if(!head->next) return NULL;
    for(struct ListNode *fast = head; fast && fast->next; fast = fast->next->next) {
        prev = *indirect;
        indirect = &(*indirect)->next;
    }
    struct ListNode *next = (*indirect)->next;
    free(*indirect);
    prev->next = next;
    return head;
}
```
圖解如下，首先是 `*next = (*indirect)->next` 的概念如下
![](https://i.imgur.com/L0OLSiT.png)

這邊沒有直接讓 `next` 畫在 `*indirect` 節點的 `next` 下方，是為了不和 `Node **` 混淆。

接著 `prev->next = next`
![](https://i.imgur.com/JAw2rux.png)

如此我們便完成了釋放中間節點的操作。

也可以使用以下作法
```c
struct ListNode* deleteMiddle(struct ListNode* head){
    struct ListNode **indirect = &head;
    struct ListNode *prev = NULL;
    
    if(!head->next) return NULL;
    for(struct ListNode *fast = head; fast && fast->next; fast = fast->next->next) {
        prev = *indirect;
        indirect = &(*indirect)->next;
    }
    struct ListNode *del = (*indirect);
    prev->next = (*indirect)->next;
    free(del);
    return head;
}
```

### Rust 版本
在 Rust 版本中如果我們想要使用快慢指標，則會發現到一些問題，最一開始 `slow` 和 `fast` 都需要指向到 `head`，接著 `slow` 和 `fast` 都需要走訪 Linked List，因此 `slow` 和 `fast` 都需要是可變的。
```rust
impl Solution {
    pub fn delete_middle(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        
    }
}
```
`delete_middle` 函式傳入了一個 `Option<Box<ListNode>>`，`Option` 如同其他的 `enum` 型別，我們可以使用 `match` 去匹配其所有的結果，在 Linked List 的實作中，會有空節點的情況發生，也就是 `nullptr`，但是在 rust 中並沒有 `nullptr` 存在，而是通過 `Option`，在 `Option` 中有兩個成員，一個為 `None`，另外一個為 `Some(val)`，我們可以使用 `match` 對 `Option` 進行解構，或是使用更加簡單的方式，使用 `?` 進行解構，以下舉例
```rust
struct Person {
    job: Option<Job>,
}

#[derive(Clone, Copy)]
struct Job {
    phone_number: Option<PhoneNumber>,
}

#[derive(Clone, Copy)]
struct PhoneNumber {
    area_code: Option<u8>,
    number: u32,
}

impl Person {
    fn work_phone_area_code(&self) -> Option<u8> {
        match self.job {
            Some(job) => {
                match job.phonstruct Person {
    job: Option<Job>,
}

#[derive(Clone, Copy)]
struct Job {
    phone_number: Option<PhoneNumber>,
}

#[derive(Clone, Copy)]
struct PhoneNumber {
    area_code: Option<u8>,
    number: u32,
}

impl Person {
    fn work_phone_area_code(&self) -> Option<u8> {
        match self.job {
            Some(job) => {
                match job.phone_number {
                    Some(phone_number) => 
                        match phone_number.area_code {
                          Some(area_code) => Some(area_code),
                            number => number,
                            None => None,
                        },
                    None => None,
                }
            }
            None => None,
        }
    }
}


fn main() {
    let p = Person {
        job: Some(Job {
            phone_number: Some(PhoneNumber {
                area_code: Some(61),
                number: 439222222,
            }),
        }),
    };
    
    println!("{}", p.work_phone_area_code().unwrap());
}
```
可以看到 `work_phone_area_code` 的部分，如果使用 `match` 進行解構，將會看起來十分的雜亂，這邊可以使用 `?` 對 `Option` 進行解構
```rust
impl Person {
    fn work_phone_area_code(&self) -> Option<u8> {
        self.job?.phone_number?.area_code
    }
}
```
`x?` 為表達式，能夠對他進行取值，`self.job?phone_number` 意義為對 `Option<job>` 取值，當 `job` 為 `Some(value)` 時，取出 `phone_number`。

回到題目本身，我們需要 `slow` 和 `fast` 對 Linked List 進行走訪，`slow` 需要為 `head` 的可變引用，因為我們需要修改他所指向的下一個節點，而 `fast` 只是用來走訪 Linked List 的，因此只需要不可變引用即可，實作如下
```rust
impl Solution {
    pub fn delete_middle(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut slow = &mut head;
        let mut fast = &head;
    }
}
```
但上面這個寫法有一個問題，我們無法同時持有對於某一個物件的可變引用以及不可變引用，為此，我們需要再創造一個新的物件 `head`，他的內容需要和 `head` 相同，但是是兩個獨立的物件
```rust
impl Solution {
    pub fn delete_middle(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut slow = &mut head;
        let mut fast = &(head.clone());
    }
}
```
概念如下
```rust
#[derive(Clone)]
struct Obj {
    a:i32
}

fn main() {
    let mut a: Obj = Obj {
        a: 1,
    };
    let mut b = &a;
    let mut c = &(a.clone());
    
    println!("{:p}", &a);
    println!("{:p}", b);
    println!("{:p}", c);
}
```
output
```
0x7ffe4eb9ed58
0x7ffe4eb9ed58
0x7ffe4eb9ed5c
```
可以看到 `a` 和 `c` 是不同的物件，如此我們便可以利用 `fast` 以及 `slow` 對 Linked List 完成走訪了
```rust
impl Solution {
    pub fn delete_middle(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut fast = &(head.clone());
        let mut slow = &mut head;
        
        while fast.is_some() && fast.as_ref()?.next.is_some() {
            slow = &mut (slow.as_mut()?.next);
            fast = &(fast.as_ref()?.next.as_ref()?.next);
        }
        
        *slow = (*slow).as_mut()?.next.take();
        
        head
    }
}
```
[source](https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/solutions/2703610/rust-fast-slow-pointers/?languageTags=rust)
同樣的，我們也可以使用 indirect pointer 的概念進行操作
```rust
impl Solution {
    pub fn delete_middle(mut head: Option<Box<ListNode>>) -> Option<Box<ListNode>> {
        let mut fast = &(head.clone());
        let mut indirect = &mut head;
        
         while fast.is_some() && fast.as_ref()?.next.is_some() {
            fast = &(fast.as_ref()?.next.as_ref()?.next);
            indirect = &mut indirect.as_mut().unwrap().next;
        }
        *indirect = indirect.as_mut().unwrap().next.take();
        head
    }
}
```
## 參考資料
[你所不知道的 C 語言: linked list 和非連續記憶體](https://hackmd.io/@sysprog/c-linked-list)

[Rust-lang](https://rust-lang.tw/book-tw/title-page.html)

[Rust. Fast&Slow pointers Solution](https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/solutions/2703610/rust-fast-slow-pointers/?languageTags=rust)