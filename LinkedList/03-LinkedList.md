# Linked List — Q&A

> Source: `Kirancse47/DSA` -> `LinkedList/` — 28 problems (real count)

Ordering: Alphabetical by filename.

## Cheat sheet
Patterns actually present here:
- **Dummy/sentinel head**: Simplifies edge cases where the head changes (e.g., *21. Merge Two Sorted Lists*, *86. Partition List*).
- **Slow-fast (tortoise-hare) pointers**: Used for middle detection (*876. Middle of the Linked List*) and cycle detection (*141. Linked List Cycle*).
- **Floyd cycle start derivation**: Post-cycle intersection, one pointer resets to head, both move at same speed to find cycle start (*142. Linked List Cycle II*).
- **Iterative reversal**: Standard prev/cur/next pointer manipulation (*206. Reverse Linked List*).
- **Recursive reversal**: Recurses to end, then updates `head->next->next = head` (*206. Reverse Linked List*).
- **Reversal in k-groups**: Counts `k` nodes, reverses the sublist, and recurses/iterates for the rest (*25. Reverse Nodes in k-Group*).
- **Merging two sorted lists**: Uses a dummy node and a tail pointer to weave two lists together (*21. Merge Two Sorted Lists*).
- **Two-pass vs one-pass nth-from-end**: One-pass uses two pointers separated by `n` steps (*19. Remove Nth Node From End of List*).
- **In-place rearrangement**: Updates `next` pointers to regroup elements without extra space (*Arrange Consonants and Vowels*).
- **Random-pointer cloning with interleaving**: Clones nodes next to originals, copies random pointers, then separates (*138. Copy List with Random Pointer*).

```cpp
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* reverseList(ListNode* head) {
    ListNode *prev = nullptr, *curr = head, *nxt = nullptr;
    while (curr) {
        nxt = curr->next;
        curr->next = prev;
        prev = curr;
        curr = nxt;
    }
    return prev;
}
```

---

## 1. 1171. Remove Zero Sum Consecutive Nodes from Linked List

**Q:** Given the head of a linked list, repeatedly delete consecutive sequences of nodes that sum to 0 until there are no such sequences.

**A:** Prefix sum with Hash Map.

**Explanation:**
- A dummy node handles cases where the head itself is part of a zero-sum sequence.
- A hash map stores the most recent node for each running prefix sum.
- When a prefix sum is seen again, the sublist between the previous occurrence and the current node sums to 0. 
- The inner loop removes the deleted nodes' prefix sums from the hash map to prevent stale references, and the pointers are linked to bypass the zero-sum sublist.

**Complexity:** Time: O(n), Space: O(n) for the hash map storing prefix sums.

```cpp
ListNode* removeZeroSumSublists(ListNode* head) {
    ListNode* dummy=new ListNode(0);
    dummy->next=head;
    unordered_map<int,ListNode*> mp;
    int psum=0;
    mp[0]=dummy;
    ListNode *ptr=head;
    while(ptr){
        psum+=ptr->val;
        if(mp.find(psum)!=mp.end()){
            ListNode* to_del=mp[psum]->next;
            int tsum=psum+to_del->val;
            while(to_del!=ptr){
                mp.erase(tsum);
                to_del=to_del->next;
                tsum+=to_del->val;
            }
            mp[psum]->next=ptr->next;
        }else mp[psum]=ptr;
        ptr=ptr->next;
    }
    return dummy->next;
}
```

---

## 2. 138. Copy List with Random Pointer

**Q:** Given a linked list where each node has an additional random pointer, return a deep copy of the list.

**A:** Hash Map for node mapping, or in-place interleaving.

**Explanation:**
- Approach 1 (Hash Map): Maps each original node to its copy. A second pass assigns the `next` and `random` pointers using the map.
- Approach 2 (In-place): Interleaves cloned nodes immediately after their original nodes (`A -> A' -> B -> B'`).
- The random pointers are then copied as `t->next->random = t->random->next`.
- Finally, the list is unweaved into the original and the deep copy.

**Complexity:** Hash Map: Time O(n) and Space O(n). In-place: Time O(n) and Space O(1) auxiliary space.

```cpp
// Using HashMap...
    Node* copyRandomList(Node* head) {
        unordered_map<Node*,Node*> mp;
        Node *t=head;
        while(t){
            mp[t]=new Node(t->val);
            t=t->next;
        }
        t=head;
        while(t){
            mp[t]->next=mp[t->next];
            if(t->random) mp[t]->random=mp[t->random];
            t=t->next;
        }
        return mp[head];
    }
    
// Modifing the LL...
    Node* copyRandomList(Node* head) {
        Node *t=head;
        while(t){
            Node *temp=t->next;
            t->next=new Node(t->val);
            t->next->next=temp;
            t=temp;
        }
        t=head;
        while(t){
            if(t->random){
                t->next->random=t->random->next;
            }
            t=t->next->next;
        }
        Node *d=new Node(0), *itr=head, *front=head, *copy=d;
        while(itr){
            front=itr->next->next;
            copy->next=itr->next;
            itr->next=front;
            copy=copy->next;
            itr=itr->next;
        }
        return d->next;
    }
```

---

## 3. 141. Linked List Cycle( Detect cycle)

**Q:** Given `head`, the head of a linked list, determine if the linked list has a cycle in it.

**A:** Slow and fast (tortoise and hare) pointers.

**Explanation:**
- `slow` moves one step, `fast` moves two steps at a time.
- If there is a cycle, the `fast` pointer will eventually overlap with the `slow` pointer.
- If `fast` reaches `NULL`, the list is linear and cycle-free.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
bool hasCycle(ListNode *head) {
        ListNode *slow=head,*fast=head;
        while(slow && fast && fast->next){
            slow=slow->next;
            fast=fast->next->next;
            if(slow==fast){
                return true;
            }
        }
        return false;
        
    }
```

---

## 4. 142. Linked List Cycle II( starting point of cycle)

**Q:** Given the head of a linked list, return the node where the cycle begins, or `NULL` if there is no cycle.

**A:** Floyd's Cycle-Finding Algorithm.

**Explanation:**
- First, detect the cycle using slow and fast pointers.
- Upon meeting, reset `fast` to the `head` of the list.
- Move both `slow` and `fast` one step at a time; their next meeting point is mathematically guaranteed to be the start of the cycle.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
    ListNode *detectCycle(ListNode *head) {
        ListNode *slow=head,*fast=head;
        while(slow && fast && fast->next){
            slow=slow->next;
            fast=fast->next->next;
            if(slow==fast){
                fast=head;
                while(slow!=fast){
                    slow=slow->next;
                    fast=fast->next;
                }
                return slow;
            }
        }
        return NULL;
    }
```

---

## 5. 146. LRU Cache

**Q:** Design a data structure that follows the constraints of a Least Recently Used (LRU) cache with `get` and `put` operations.

**A:** Doubly Linked List with Hash Map.

**Explanation:**
- A doubly linked list manages the usage order (most recently used at the front, least at the rear).
- A hash map provides O(1) access to the nodes by key.
- On `get` or `put`, the accessed node is moved to the front.
- If capacity is exceeded on `put`, the node at the rear is removed from both the list and the map.

**Complexity:** Time: O(1) per operation, Space: O(capacity) for the map and list nodes.

```cpp
struct Node{
    int key;
    int val;
    Node* next;
    Node* prev;
    Node(int k,int v){
        key=k;
        val=v;
        next=NULL;
        prev=NULL;
    }
};

class LRUCache {
    unordered_map<int,Node*> mp;
    int capacity;
    Node* front;
    Node* rear;
  public:
    LRUCache(int cap) {
        capacity=cap;
        front=new Node(-1,-1);
        rear=new Node(-1,-1);
        front->next=rear;
        rear->prev=front;
    }

    void putFront(Node* node){
        node->next=front->next;
        node->prev=front;
        
        front->next->prev=node;
        front->next=node;
    }
    
    void removeNode(Node* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    int get(int key) {
        if(mp.find(key)==mp.end()) return -1;
        Node* node=mp[key];
        removeNode(node);
        putFront(node);
        return node->val;
    }

    void put(int key, int value) {
        if(capacity==0) return;
        if(mp.find(key)!=mp.end()){
            Node* node=mp[key];
            node->val=value;
            removeNode(node);
            putFront(node);
        }else{
            if(mp.size()>=capacity){
                Node *temp=rear->prev;
                mp.erase(temp->key);
                removeNode(temp);
                delete(temp);
            }
            Node *node=new Node(key,value);
            mp[key]=node;
            putFront(node);
        }
    }
};

Link - https://practice.geeksforgeeks.org/problems/lru-cache/1
```

---

## 6. 160. Intersection of Two Linked Lists

**Q:** Given the heads of two singly linked lists `headA` and `headB`, return the node at which the two lists intersect.

**A:** Two-pointer traversal with swapping.

**Explanation:**
- Two pointers traverse each list.
- When a pointer reaches the end of its list, it redirects to the head of the other list.
- This neutralizes the length difference, so they will meet at the intersection node or `NULL` simultaneously.

**Complexity:** Time: O(n + m), Space: O(1) auxiliary space.

```cpp
ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
        ListNode *ptra=headA, *ptrb=headB;
        while(ptra!=ptrb){
            ptra = ptra->next; 
            ptrb = ptrb->next;
            if(!ptra && !ptrb) return NULL;
            if(!ptra) ptra=headB;
            if(!ptrb) ptrb=headA;
        }
        return ptra;
    }
```

---

## 7. 19. Remove Nth Node From End of List

**Q:** Given the head of a linked list, remove the `n`-th node from the end of the list and return its head.

**A:** One-pass two-pointer technique.

**Explanation:**
- A dummy node ensures safe removal even if the head node is deleted.
- The `fast` pointer advances `n` steps ahead of the `slow` pointer.
- Both pointers advance together until `fast` reaches the last node; `slow` is then right before the node to delete.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
ListNode* removeNthFromEnd(ListNode* head, int n) {
        ListNode *d=new ListNode(0);
        d->next=head;
        ListNode *s=d, *f=d;
        while(n){
            f=f->next;
            n--;
        }
        while(f->next){
            f=f->next;
            s=s->next;
        }
        s->next=s->next->next;
        return d->next;
    }
```

---

## 8. 206. Reverse Linked List

**Q:** Given the head of a singly linked list, reverse the list, and return the reversed list.

**A:** Iterative three-pointer tracking or recursion.

**Explanation:**
- Iterative: Use `prevN`, `currN`, and `nextN` to reverse pointers in a single pass.
- Recursive: Recursively reverse the rest of the list and make the next node point back to the current node.
- Both handle empty and single-node list cases natively.

**Complexity:** Iterative: Time O(n), Space O(1). Recursive: Time O(n), Space O(n) for the call stack.

```cpp
// Iterative...
    ListNode* reverseList(ListNode* head) {
        if(!head) return head;
        ListNode *prevN=NULL,*currN=head,*nextN=NULL;
        while(currN){
            nextN=currN->next;
            currN->next=prevN;
            prevN=currN;
            currN=nextN;
        }
        return prevN;
    }
    
// Recursive.....
    ListNode* reverseList(ListNode* head,ListNode* prev=NULL) {
        if(!head) return prev;
        ListNode *temp=head->next;
        head->next=prev;
        return reverseList(temp,head);
    }
```

---

## 9. 21. Merge Two Sorted Lists

**Q:** Merge two sorted linked lists into one sorted list and return its head.

**A:** Dummy node and tail pointer weaving.

**Explanation:**
- A dummy node simplifies appending to the merged list.
- A pointer `ptr` compares current nodes of both lists and links to the smaller one.
- After one list is exhausted, the remaining non-empty list is appended in O(1) time.

**Complexity:** Time: O(n + m), Space: O(1) auxiliary space.

```cpp
ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
        ListNode *d=new ListNode();
        ListNode *ptr=d;
        
        while(list1 && list2){
            if(list1->val < list2->val){
                ptr->next=list1;
                list1=list1->next;
            }else{
                ptr->next=list2;
                list2=list2->next;
            }
            ptr=ptr->next;
        }
        if(list1) ptr->next=list1;
        else ptr->next=list2;
        return d->next;
    }
```

---

## 10. 234. Palindrome Linked List

**Q:** Given the `head` of a singly linked list, return true if it is a palindrome or false otherwise.

**A:** Find middle, reverse second half, and compare.

**Explanation:**
- Slow/fast pointers locate the middle of the list.
- The second half is reversed in-place starting from `slow->next`.
- Two pointers (one at `head`, one at the reversed half) compare values step by step.

**Complexity:** Time: O(n) since each node is visited a constant number of times, Space: O(n) due to the recursive reversal stack used here (could be O(1) if iterative reversal was used).

```cpp
    ListNode *reverseLL(ListNode *cur,ListNode *prev=NULL){
        if(!cur) return prev;
        ListNode *temp=cur->next;
        cur->next=prev;
        return reverseLL(temp,cur);
    }
    bool isPalindrome(ListNode* head) {
        ListNode *slow=head, *fast=head;
        while(fast && fast->next && fast->next->next){
            slow=slow->next;
            fast=fast->next->next;
        }
        slow->next=reverseLL(slow->next);
        ListNode *lptr=head, *rptr=slow->next;
        while(rptr){
            if(lptr->val!=rptr->val) return false;
            lptr=lptr->next;
            rptr=rptr->next;
        }
        return true;
    }
```

---

## 11. 237. Delete Node in a Linked List

**Q:** Delete a node in a singly linked list, given only access to that node (not the head).

**A:** Value copying and pointer skipping.

**Explanation:**
- We cannot modify the previous node's pointer since we don't have access to the head.
- Instead, copy the value of the next node into the current node.
- Then, bypass and delete the next node.

**Complexity:** Time: O(1), Space: O(1) auxiliary space.

```cpp
void deleteNode(ListNode* node) {
        // *node=*node->next;
        node->val=node->next->val;
        node->next=node->next->next;
    }
```

---

## 12. 23. Merge k Sorted Lists

**Q:** Given an array of `k` linked lists, each sorted in ascending order, merge all the linked lists into one sorted linked list.

**A:** Sequential merging or Min-Heap (Priority Queue).

**Explanation:**
- Approach 1 (Brute Force): Merges lists sequentially using a standard two-list merge helper.
- Approach 2 (MinHeap): Pushes the head of each list into a min-heap. At each step, extracts the smallest node, appends it, and pushes its next node to the heap.
- The MinHeap optimally picks the smallest current element among all `k` lists in logarithmic time.

**Complexity:** 
- Brute Force: Time: O(k * n) where n is total nodes, Space: O(1).
- MinHeap: Time: O(n log k), Space: O(k) for the priority queue.

```cpp
// Using Brute Force...
    ListNode *merge(ListNode *l1,ListNode *l2){
        ListNode *d=new ListNode(0), *p=d;
        while(l1 && l2){
            if(l1->val < l2->val){
                p->next=l1;
                l1=l1->next;
            }else{
                p->next=l2;
                l2=l2->next;
            }
            p=p->next;
        }
        if(l1) p->next=l1;
        if(l2) p->next=l2;
        return d->next;
    }
    
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int n=lists.size();
        if(n==0) return NULL;
        if(n==1) return lists[0];
        ListNode *ans=merge(lists[0],lists[1]);
        for(int i=2;i<n;i++){
            ans=merge(ans,lists[i]);
        }
        return ans;
    }
    
// Using MinHeap...
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        int n=lists.size();
        if(n==0) return NULL;
        if(n==1) return lists[0];
        
        priority_queue< pair<int,ListNode*>,vector<pair<int,ListNode*>>, greater<pair<int,ListNode*>> > q;
        for(int i=0;i<n;i++){
            if(lists[i]){
                q.push({lists[i]->val,lists[i]});
            }
        }
        
        ListNode *dummy=new ListNode(0), *ptr=dummy;
        while(!q.empty()){
            auto it=q.top();q.pop();
            ptr->next=it.second;
            ptr=ptr->next;
            if(it.second->next){
                q.push({it.second->next->val,it.second->next});
            }
        }
        return dummy->next;
    }
```

---

## 13. 2487. Remove Nodes From Linked List

**Q:** Remove every node which has a node with a strictly greater value anywhere to the right side of it.

**A:** Monotonic Stack.

**Explanation:**
- A stack stores nodes in monotonically decreasing order.
- While traversing, if the current node's value is greater than the stack's top, nodes are popped from the stack (effectively removing them).
- After processing all nodes, the remaining nodes in the stack are linked together to form the answer.

**Complexity:** Time: O(n) as each node is pushed and popped at most once, Space: O(n) for the stack.

```cpp
    ListNode* removeNodes(ListNode* head) {
        ListNode *ptr=head;
        stack<ListNode*> st;
        while(ptr){
            while(!st.empty() && st.top()->val < ptr->val){
                st.pop();
            }
            st.push(ptr);
            ptr=ptr->next;
        }
        ListNode *ans=NULL;
        while(!st.empty()){
            ListNode *t=ans;
            ans=st.top();st.pop();
            ans->next=t;
        }
        return ans;
    }
```

---

## 14. 25. Reverse Nodes in k-Group

**Q:** Given the head of a linked list, reverse the nodes of the list `k` at a time, and return the modified list.

**A:** Counting iterations and recursive sublist reversal.

**Explanation:**
- A pointer `cursor` scans ahead `k` steps. If it hits `nullptr` before counting `k`, it returns the original head (sublist too short to reverse).
- If `k` nodes are found, a standard iterative reversal reverses those `k` nodes.
- Recursively applies the same function to the remaining list and connects the reversed sublist to the result.

**Complexity:** Time: O(n), Space: O(n/k) for the recursion stack.

```cpp
 ListNode* reverseKGroup(ListNode* head, int k) {
    ListNode* cursor = head;
        for(int i = 0; i < k; i++){
            if(cursor == nullptr) return head;
            cursor = cursor->next;
        }
        ListNode* curr = head;
        ListNode* prev = nullptr;
        ListNode* nxt = nullptr;
        for(int i = 0; i < k; i++){
            nxt = curr->next;
            curr->next = prev;
            prev = curr;
            curr = nxt;
        }
        head->next = reverseKGroup(curr, k);
        return prev;
    }
```

---

## 15. 2. Add Two Numbers

**Q:** Add two numbers represented by two linked lists in reverse order (digits stored in reverse) and return the sum as a linked list.

**A:** Digit-by-digit addition with carry.

**Explanation:**
- A dummy node points to the resulting list.
- While traversing both lists or while a carry exists, extract values (0 if a list is exhausted), compute the sum, and derive the new carry.
- Create a new node for the current digit (`s % 10`) and move pointers forward.

**Complexity:** Time: O(max(n, m)), Space: O(max(n, m)) for the new resulting list.

```cpp
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode *ans=new ListNode(0);
        ListNode *ptr=ans;
        int c=0;
        while(l1 || l2){
            int a= l1 ? l1->val : 0;
            int b= l2 ? l2->val : 0;
            int s=a+b+c;
            c=s/10;
            ptr->next=new ListNode(s%10);
            ptr=ptr->next;
            if(l1) l1=l1->next;
            if(l2) l2=l2->next;
        }
        if(c){
            ptr->next=new ListNode(c);
        }
        return ans->next;
    }
```

---

## 16. 460. LFU Cache

**Q:** Design a data structure that follows the constraints of a Least Frequently Used (LFU) cache.

**A:** Two Hash Maps with custom Doubly Linked Lists.

**Explanation:**
- `keyNode` maps a key to its node, providing O(1) access.
- `freqList` maps a frequency to a doubly linked list containing all nodes with that frequency.
- When accessed (`get` or `put`), a node is removed from its current frequency list and added to the list for `freq + 1`.
- `minFreq` is dynamically updated. If capacity is hit, the least recently used node from the `minFreq` list is evicted.

**Complexity:** Time: O(1) per operation, Space: O(capacity) for the maps and nodes.

```cpp
struct Node {
    int key, value, freq;
    Node* prev;
    Node* next;
    Node(int k, int v) : key(k), value(v), freq(1), prev(nullptr), next(nullptr) {}
};

struct DLList {
    Node* head;
    Node* tail;
    int size;
    DLList() {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
        size = 0;
    }
    void addFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
        size++;
    }
    void remove(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        size--;
    }
    Node* removeLast() {
        if(size == 0) return nullptr;
        Node* node = tail->prev;
        remove(node);
        return node;
    }
};

class LFUCache {
    int capacity, minFreq, curSize;
    unordered_map<int, Node*> keyNode;
    unordered_map<int, DLList*> freqList;
public:
    LFUCache(int capacity) {
        this->capacity = capacity;
        minFreq = 0;
        curSize = 0;
    }

    void update(Node* node) {
        int freq = node->freq;
        freqList[freq]->remove(node);
        if(freq == minFreq && freqList[freq]->size == 0) {
            minFreq++;
        }
        node->freq++;
        if(!freqList.count(node->freq)) {
            freqList[node->freq] = new DLList();
        }
        freqList[node->freq]->addFront(node);
    }

    int get(int key) {
        if(!keyNode.count(key)) return -1;
        Node* node = keyNode[key];
        update(node);
        return node->value;
    }

    void put(int key, int value) {
        if(capacity == 0) return;

        if(keyNode.count(key)) {
            Node* node = keyNode[key];
            node->value = value;
            update(node);
        } else {
            if(curSize == capacity) {
                Node* toRemove = freqList[minFreq]->removeLast();
                keyNode.erase(toRemove->key);
                curSize--;
            }
            Node* node = new Node(key, value);
            minFreq = 1;
            if(!freqList.count(minFreq)) {
                freqList[minFreq] = new DLList();
            }
            freqList[minFreq]->addFront(node);
            keyNode[key] = node;
            curSize++;
        }
    }
};
```

---

## 17. 61. Rotate List

**Q:** Given the head of a linked list, rotate the list to the right by `k` places.

**A:** Sublist separation and re-linking using two-pointers.

**Explanation:**
- First pass calculates the length `n` of the list to handle cases where `k >= n` using `k % n`.
- `f` advances `k` steps, then `s` and `f` move together.
- When `f` is at the end, `s` is at the node before the new head. The list is split and the tail is connected to the original head.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
ListNode* solve(ListNode* head, int k){
        if(!k) return head;
        ListNode *s=head, *f=head;
        while(k){
            k--;
            f=f->next;
        }
        while(f->next){
            f=f->next;
            s=s->next;
        }
        ListNode *ptr=s->next;
        f->next=head;
        s->next=NULL;
        return ptr;
    }
    
    ListNode* rotateRight(ListNode* head, int k) {
        if(k==0 || !head || !head->next) return head;
        ListNode *ptr=head;
        int n=0;
        while(ptr){
            n++;
            ptr=ptr->next;
        }
        return solve(head,k%n);
    }
```

---

## 18. 86. Partition List

**Q:** Given the head of a linked list and a value `x`, partition it such that all nodes less than `x` come before nodes greater than or equal to `x`, preserving relative order.

**A:** Two dummy lists weaving.

**Explanation:**
- Create two separate dummy-headed lists: one for elements `< x` (`b_list`) and one for elements `>= x` (`a_list`).
- Traverse the original list, appending nodes to either list based on their value.
- Connect the end of the `< x` list to the start of the `>= x` list and terminate the `>= x` list with `NULL`.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
    ListNode* partition(ListNode* head, int x) {
        if(!head || !head->next) return head;
        ListNode *ptr=head, *b_list=new ListNode(0), *before=b_list, *a_list=new ListNode(0), *after=a_list;
        while(ptr){
            if(ptr->val < x){
                before->next=ptr;
                before=before->next;
            }else{
                after->next=ptr;
                after=after->next;
            }
            ptr=ptr->next;
        }
        before->next=a_list->next;
        after->next=NULL;
        return b_list->next;
    }
```

---

## 19. 876. Middle of the Linked List

**Q:** Given the head of a singly linked list, return the middle node of the linked list.

**A:** Slow and fast (tortoise and hare) pointers.

**Explanation:**
- `slow` moves one node at a time while `fast` moves two nodes at a time.
- When `fast` reaches the end of the list (or `NULL`), `slow` will be exactly at the middle node.
- Works perfectly for both odd and even-length lists (returns second middle for even length).

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
ListNode* middleNode(ListNode* head) {
        ListNode *s=head,*f=head;
        while(f && f->next){
            s=s->next;
            f=f->next->next;
        }
        return s;
    }
```

---

## 20. 92. Reverse Linked List II

**Q:** Given the head of a singly linked list and two integers `left` and `right`, reverse the nodes of the list from position `left` to position `right`, and return the reversed list.

**A:** Targeted iterative sublist reversal.

**Explanation:**
- The list is traversed to find the nodes just before the `left` boundary (`llp`) and just after the `right` boundary (`rrp`).
- The sublist between `left` and `right` is detached and reversed using a standard iterative reversal up to the `rrp` boundary.
- The reversed sublist is then stitched back into the original list between `llp` and `rrp`.
- A special condition handles when the reversal starts at the `head` (`left == 1`).

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
    ListNode* reverseList(ListNode* head,ListNode* end) {
        if(!head) return head;
        ListNode *prevN=NULL,*currN=head,*nextN=NULL;
        while(currN!=end){
            nextN=currN->next;
            currN->next=prevN;
            prevN=currN;
            currN=nextN;
        }
        return prevN;
    }
    
    ListNode* reverseBetween(ListNode* head, int left, int right) {
        
        if(!head || !head->next || left>=right) return head;
        if(left==1){
            ListNode *temp1=head, *temp2, *it=head;
            int c=1;
            while(it){
                if(c==right) break;
                c++;
                it=it->next;
            }
            temp2=it->next;
            head=reverseList(head,temp2);
            temp1->next=temp2;
            return head;
        }
        int count=1;
        ListNode *itr=head, *llp=NULL, *lp=NULL, *rrp=NULL, *rp=NULL;
        while(itr){
            if(count==left-1){
                llp=itr;
                lp=itr->next;
            }
            else if(count==right){
                rp=itr;
                rrp=itr->next;
            }
            count++;
            itr=itr->next;
        }
        
        llp->next=reverseList(lp,rrp);
        lp->next=rrp;
        return head;
    }
```

---

## 21. Arrange Consonants and Vowels

**Q:** Given a singly linked list of characters, arrange them so that all vowels come before all consonants while maintaining relative order.

**A:** Two dummy lists weaving.

**Explanation:**
- Two dummy nodes (`dc` and `dv`) act as heads for the consonants list and vowels list respectively.
- Traverse the original list, checking each character. Vowels are appended to the vowel list, and consonants to the consonant list.
- Finally, connect the tail of the vowel list to the head of the consonant list, and terminate the consonant list.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
    struct Node* arrangeCV(Node* head) {
        Node *dc=new Node('#'), *dv=new Node('#'),*c=dc,*v=dv;
        while(head){
            if(head->data=='a' || head->data=='e' || head->data=='i' || head->data=='o' || head->data=='u'){
                v->next=head;
                v=v->next;
            }else{
                c->next=head;
                c=c->next;
            }
            head=head->next;
        }
        v->next=dc->next;
        c->next=NULL;
        return dv->next;
    }

gfg link - https://www.geeksforgeeks.org/problems/arrange-consonants-and-vowels/1
```

---

## 22. Decimal Equivalent of Binary Linked List

**Q:** Given a singly linked list of 0s and 1s representing a binary number, find its decimal equivalent modulo 10^9 + 7.

**A:** Bitwise shifting.

**Explanation:**
- A running total is maintained starting from 0.
- For each node, the total is left-shifted by 1 (equivalent to multiplying by 2), and the node's bit value is added (using bitwise OR).
- Modulo arithmetic is applied at each step to prevent overflow.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
    long long unsigned int decimalValue(Node *head) {
        Node* cur=head;
        long long unsigned int ans=0,mod=1e9+7;
        while(cur){
            ans=(ans<<1 | cur->data)%mod;
            cur=cur->next;
        }
        return ans;
    }

gff link - https://www.geeksforgeeks.org/problems/decimal-equivalent-of-binary-linked-list/1
```

---

## 23. Find pairs with given sum in doubly linked list

**Q:** Given a sorted doubly linked list and a target sum, find all pairs of nodes whose sum equals the target.

**A:** Two-pointer traversal from both ends.

**Explanation:**
- Since the list is doubly linked and sorted, initialize a `left` pointer at the head and a `right` pointer at the tail.
- If the sum of `left` and `right` values is equal to the target, record the pair and move both pointers inward.
- If the sum is less than the target, move the `left` pointer forward; if greater, move the `right` pointer backward.
- Stop when `left` and `right` pointers cross each other.

**Complexity:** Time: O(n), Space: O(1) auxiliary space (excluding the array to store results).

```cpp
    vector<pair<int, int>> findPairsWithGivenSum(Node *head, int target){
        vector<pair<int,int>> ans;
        if(!head) return ans;
        Node *left=head,*right=head;
        while(right->next!=NULL) right=right->next;
        while(left->data < right->data){
            int sum=left->data+right->data;
            if(sum==target){
                ans.push_back({left->data,right->data});
                left=left->next;
                right=right->prev;
            }else if(sum<target){
                left=left->next;
            }else{
                right=right->prev;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/find-pairs-with-given-sum-in-doubly-linked-list/1
```

---

## 24. Flattening a Linked List

**Q:** Given a linked list where every node represents a linked list (via a bottom pointer) and is sorted, flatten the list into a single sorted list.

**A:** Recursive merge of bottom lists.

**Explanation:**
- Recursion is used to go to the end of the `next` list.
- Starting from the end, two adjacent sorted lists (connected by `bottom` pointers) are merged into one sorted list.
- The `merge` helper function weaves the two lists based on their values, using a dummy node.
- The merged list is then merged with the list before it, propagating up until the entire structure is flattened.

**Complexity:** Time: O(N * M) where N is the number of main nodes and M is the number of bottom nodes, Space: O(N) for recursion stack.

```cpp
Node* merge(Node* head1, Node* head2){
    Node *dummy=new Node(0), *ptr=dummy;
    while(head1 && head2){
        if(head1->data > head2->data){
            ptr->bottom=head2;
            ptr=ptr->bottom;
            head2=head2->bottom;
        }else{
            ptr->bottom=head1;
            ptr=ptr->bottom;
            head1=head1->bottom;
        }
    }
    if(head1){
        ptr->bottom=head1;
    }else{
        ptr->bottom=head2;
    } 
    return dummy->bottom;
}

Node *flatten(Node *root){
   if(!root || !root->next) return root;
   root->next=flatten(root->next);
   root=merge(root,root->next);
   return root;
   // return merge(root,flatten(root->next));;
}
```

---

## 25. Merge 2 sorted linked list in reverse order

**Q:** Given two sorted linked lists, merge them such that the resulting merged list is in reverse order.

**A:** Merge sorting followed by iterative list reversal.

**Explanation:**
- The two sorted lists are first merged into a single sorted list in ascending order using a dummy node.
- Once the ascending sorted list is formed, it is iteratively reversed using standard three-pointer traversal (`prevN`, `curN`, `nextN`).
- Returning the reversed head produces the final descending list.

**Complexity:** Time: O(n + m), Space: O(1) auxiliary space.

```cpp
    struct Node *reverseLL(Node *head){
        if(!head || !head->next) return head;
        Node *prevN=NULL,*curN=head,*nextN=NULL;
        while(curN){
            nextN=curN->next;
            curN->next=prevN;
            prevN=curN;
            curN=nextN;
        }
        return prevN;
    }
    
    struct Node * mergeResult(Node *node1,Node *node2){
        Node *d=new Node;
        Node *ptr=d;
        while(node1 && node2){
            if(node1->data < node2->data){
                ptr->next=node1;
                ptr=ptr->next;
                node1=node1->next;
            }else{
                ptr->next=node2;
                ptr=ptr->next;
                node2=node2->next;
            }
        }
        if(node1){
            ptr->next=node1;
        }else ptr->next=node2;
        return reverseLL(d->next);
    }

gfg link - https://www.geeksforgeeks.org/problems/merge-2-sorted-linked-list-in-reverse-order/1
```

---

## 26. Remove loop in Linked Lis

**Q:** Given the head of a linked list that may contain a loop, detect and remove the loop to make it a standard null-terminated list.

**A:** Floyd's Cycle-Finding Algorithm with a trailing pointer.

**Explanation:**
- `slow` and `fast` pointers are used to detect a cycle. A trailing `pre` pointer keeps track of the node before `slow`.
- When they meet, reset `fast` to `head` and move both `slow` and `fast` at the same pace.
- The node `pre` tracks the position directly behind `slow`. When `slow` and `fast` meet again at the loop start, `pre` will point to the loop's end.
- Set `pre->next = NULL` to break the cycle.

**Complexity:** Time: O(n), Space: O(1) auxiliary space.

```cpp
    void removeLoop(Node* head) {
        Node *slow=head, *fast=head, *pre=NULL;
        while(fast && fast->next){
            pre=slow;
            slow=slow->next;
            fast=fast->next->next;
            if(slow==fast){
                fast=head;
                while(fast!=slow){
                    fast=fast->next;
                    pre=slow;
                    slow=slow->next;
                }
                pre->next=NULL;
            }
        }
    }
```

---

## 27. Reverse a Linked List in groups of given size.

**Q:** Given a linked list, reverse the nodes of the linked list `k` at a time and return its modified list.

**A:** Recursive counting group reversal.

**Explanation:**
- Iteratively reverse up to `k` nodes using three pointers (`prevN`, `currN`, `nextN`), while maintaining a counter.
- If there are remaining nodes (`nextN` is not `NULL`), recursively call the function on the remaining list.
- Attach the result of the recursive call to `head->next` (which becomes the tail of the reversed sublist).
- Return `prevN` which is the new head of the reversed sublist.

**Complexity:** Time: O(n), Space: O(n/k) for the recursion stack.

```cpp
    struct node *reverse (struct node *head, int k){ 
        if(!head) return head;
        node *prevN=NULL,*currN=head,*nextN=NULL;
        int count=1;
        while(currN && count<=k){
            nextN=currN->next;
            currN->next=prevN;
            prevN=currN;
            currN=nextN;
            count++;  
        }
        if(nextN){
            head->next=reverse(nextN,k);
        }
        return prevN;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/reverse-a-linked-list-in-groups-of-given-size/1
```

---

## 28. Sort a linked list of 0s, 1s and 2s

**Q:** Given a linked list consisting of only 0s, 1s, and 2s, sort it in ascending order.

**A:** Two-pass frequency counting and value overwriting.

**Explanation:**
- First pass: Traverse the list and maintain an array `freq` of size 3 to count the occurrences of 0, 1, and 2.
- Second pass: Traverse the list again. While a frequency count for a value is greater than 0, overwrite the node's data with that value and decrement the frequency.
- Move sequentially from 0 to 2 to ensure the list is sorted.

**Complexity:** Time: O(n), Space: O(1) auxiliary space (array is fixed size 3).

```cpp
    Node* segregate(Node *head) {
        int freq[3]={0};
        Node *ptr=head;
        while(ptr){
            freq[ptr->data]++;
            ptr=ptr->next;
        }
        ptr=head;
        while(ptr){
            if(freq[0]){
                ptr->data=0;
                freq[0]--;
            }
            else if(freq[1]){
                ptr->data=1;
                freq[1]--;
            }
            else{
                ptr->data=2;
                freq[2]--;
            }
            ptr=ptr->next;
        }
        return head;
    }
```
