# Trie (Prefix Tree) — Q&A
> Source: `Kirancse47/DSA` -> `Trie/` — 8 problems

**Cheat Sheet:**
- **Node Layout:** Typically an array of pointers `Node* children[26]` for lowercase English letters and a boolean `isEnd` flag. For larger alphabets, use an `unordered_map<char, Node*>`.
- **Core Operations:** `insert`, `search`, and `startsWith`. All take $O(L)$ time, where $L$ is the length of the word.
- **Space Complexity:** $O(\text{Total Characters} \times \text{Alphabet Size})$.
- **Variants in this folder:**
  - **Standard Dictionary:** `208. Implement Trie (Prefix Tree)`, `Trie | (Insert and Search)`
  - **Counting Prefixes/Words:** `Implement Trie-2 | INSERT | countWordsEqualTo() | countWordsStartingWith()`. Instead of a boolean `isEnd`, nodes maintain `countWords` and `countPrefixes`.
  - **Substrings / Longest Word:** `Count Distinct Substrings`, `Longest Word With All Prefixes | Complete String`. We insert all suffixes or traverse to find paths where every node has `isEnd == true`.
  - **Binary Trie for XOR:** `Maximum XOR of two numbers in an array`, `Minimum XOR value pair`, `1707. Maximum XOR With an Element From Array`. Instead of 26 letters, we use 2 children (0 and 1) for the 32 bits of an integer.
  - **Greedy XOR Descent:** To maximize XOR of a value `V` with elements in the Trie, traverse from MSB (bit 31) to LSB. At each step, if the opposite bit of `V`'s current bit exists, follow that branch to maximize the XOR sum (add $2^i$). Otherwise, follow the same bit. To minimize XOR, prefer the *same* bit.

## 1. 1707. Maximum XOR With an Element From Array

**Q:** Given an array of integers and multiple queries `[x, m]`, find the maximum XOR of `x` with any array element that is less than or equal to `m`.

**A:** Offline queries with a Binary Trie.

**Explanation:**
- Sort the queries by their `m` values to process them offline.
- Sort the input array `nums` in ascending order.
- Maintain a pointer in `nums` and insert elements into a binary Trie as long as they are $\le m$ for the current query.
- For each query, if the Trie is empty (no elements $\le m$), the answer is `-1`; otherwise, find the maximum XOR using the greedy opposite-bit descent.

**Complexity:** Time $O(n \log n + Q \log Q + Q \times 32 + n \times 32)$, Space $O(n \times 32 + Q)$.

```cpp
    class TrieNode{
        public:
            TrieNode *left;
            TrieNode *right;
            TrieNode(){
                left=NULL;
                right=NULL;
            }
    };
    
    void insert(TrieNode *head,int n){
        TrieNode *cur=head;
        for(int i=31;i>=0;i--){
            int b=(n>>i & 1);
            if(b){
                if(!cur->right){
                    cur->right=new TrieNode();
                }
                cur=cur->right;
            }else{
                if(!cur->left){
                    cur->left=new TrieNode();
                }
                cur=cur->left;
            }
        }
    }
    
    int max_xor(TrieNode *root, int val){        
        int xors=0;
        TrieNode *cur=root;
        for(int j=31;j>=0;j--){
            int b=(val>>j & 1);
            if(b){
                if(cur->left){
                    xors+=pow(2,j);
                    cur=cur->left;
                }else{
                    cur=cur->right;
                }
            }else{
                if(cur->right){
                    xors+=pow(2,j);
                    cur=cur->right;
                }else{
                    cur=cur->left;
                }
            }
        }
        return xors;
    }
    
    vector<int> maximizeXor(vector<int>& nums, vector<vector<int>>& queries) {
        int n=queries.size();
        vector<vector<int>> que;
        for(int i=0;i<n;i++){            
            que.push_back({queries[i][0],queries[i][1],i});
        }
        sort(que.begin(),que.end(),[](vector<int> &q1,vector<int> &q2){
           return q1[1]<q2[1]; 
        });
        
        vector<int> ans(n);
        TrieNode *root=new TrieNode();
        sort(nums.begin(),nums.end());
        int m=nums.size();
        int j=0;
        int count=0;
        for(int i=0;i<n;i++){
            while(j<m && nums[j]<=que[i][1]){
                insert(root,nums[j]);
                j++;
                count++;
            }
            if(count==0) ans[que[i][2]]=-1;
            else ans[que[i][2]]=max_xor(root,que[i][0]);
        }
        return ans;
    }
```

## 2. 208. Implement Trie (Prefix Tree)

**Q:** Implement a Trie class with `insert`, `search` (exact match), and `startsWith` (prefix match) methods for lowercase English strings.

**A:** Standard 26-ary Tree (Trie).

**Explanation:**
- Create a `Node` struct containing an array of 26 child pointers and a boolean `flag` for the end of a word.
- For `insert`, traverse characters, instantiating missing child nodes, and set `flag = true` at the final node.
- For `search` and `startsWith`, traverse the nodes; return false if a child pointer is NULL. `search` requires the final node's `flag` to be true.

**Complexity:** Time $O(L)$ per operation where $L$ is word length. Space $O(\text{Total characters} \times 26)$.

```cpp
struct Node{
    Node* links[26];
    bool flag;
    Node(){
        for(int i=0;i<26;i++) links[i]=NULL;
        flag=false;
    }
    bool isContain(char c){
        return links[c-'a']!=NULL;
    }
    void put(char c,Node* n){
        links[c-'a']=n;
    }
    Node* get(char c){
        return links[c-'a'];
    }
    void setEnd(){
        flag=true;
    }
    bool isEnd(){
        return flag;
    }
};

class Trie {
    
private:
    Node* root;
public:    
    Trie() {
        root=new Node();
    }
    
    void insert(string word) {
        Node* temp=root;
        for(auto it : word){
            if(!temp->isContain(it)){
                temp->put(it,new Node());
            }
            temp=temp->get(it);
        }
        temp->setEnd();
    }
    
    bool search(string word) {
        Node* temp=root;
        for(auto it : word){
            if(!temp->isContain(it)){
                return false;
            }
            temp=temp->get(it);
        }
        return temp->flag;
    }
    
    bool startsWith(string prefix) {
        Node* temp=root;
        for(auto it : prefix){
            if(!temp->isContain(it)){
                return false;
            }
            temp=temp->get(it);
        }
        return true;
    }
};
```

## 3. Count Distinct Substrings

**Q:** Given a string, return the total number of distinct substrings including the empty string.

**A:** Trie insertion of all suffixes.

**Explanation:**
- A Trie uniquely stores prefixes of the strings inserted into it.
- By inserting every suffix of the original string into the Trie, every node created corresponds to a unique substring.
- During insertion, whenever a new node is allocated (meaning this prefix of the suffix wasn't seen before), increment the distinct substring count.
- Add 1 to the final answer to account for the empty string.

**Complexity:** Time $O(n^2)$, Space $O(n^2 \times 26)$ where $n$ is string length.

```cpp
struct Node{
    Node *a[26];
    Node(){
        for(int i=0;i<26;i++){
            a[i]=NULL;
        }
    }
};
int countDistinctSubstrings(string &s){
    //    Write your code here.
    int n=s.size();
    Node *root=new Node();
    int ans=0;
    for(int i=0;i<n;i++){
        Node *cur=root;
        for(int j=i;j<n;j++){
            if(!cur->a[s[j]-'a']){
                ans++;
                cur->a[s[j]-'a']=new Node();
            }
            cur=cur->a[s[j]-'a'];
        }
    }
    return ans+1;
}

Link - https://www.codingninjas.com/codestudio/problems/count-distinct-substrings_985292?leftPanelTab=0
```

## 4. Implement Trie-2 | INSERT | countWordsEqualTo() | countWordsStartingWith()

**Q:** Implement a Trie that supports inserting a word, counting exact occurrences of a word, counting words with a specific prefix, and erasing a word.

**A:** Trie augmented with prefix and word counts.

**Explanation:**
- Instead of a boolean end flag, each node maintains two counters: `cp` (count of prefixes passing through) and `ew` (count of words ending here).
- On `insert`, increment `cp` for every node visited, and `ew` at the final node.
- On `erase`, decrement `cp` for every node visited, and `ew` at the final node.
- Queries simply traverse to the end of the word/prefix and return `ew` or `cp` respectively (returning 0 if the path breaks).

**Complexity:** Time $O(L)$ per operation where $L$ is word length. Space $O(\text{Total characters} \times 26)$.

```cpp
struct Node{
    Node *a[26];
    int ew,cp;
    Node(){
        for(int i=0;i<26;i++){
            a[i]=NULL;
        }
        ew=0;
        cp=0;
    }
};
class Trie{
    Node *root;
    public:
    Trie(){
        // Write your code here.
        root=new Node();
    }

    void insert(string &word){
        // Write your code here.
        Node *cur=root;
        for(auto it : word){
            if(!cur->a[it-'a']){
                cur->a[it-'a']=new Node();
            }
            cur=cur->a[it-'a'];
            cur->cp++;
        }
        cur->ew++;
    }

    int countWordsEqualTo(string &word){
        // Write your code here.
        Node *cur=root;
        for(auto it : word){
            if(!cur->a[it-'a']) return 0;
            cur=cur->a[it-'a'];
        }
        return cur->ew;
    }

    int countWordsStartingWith(string &word){
        // Write your code here.
        Node *cur=root;
        for(auto it : word){
            if(!cur->a[it-'a']) return 0;
            cur=cur->a[it-'a'];
        }
        return cur->cp;
    }

    void erase(string &word){
        // Write your code here.
        Node *cur=root;
        for(auto it : word){
            if(!cur->a[it-'a']) return;
            cur=cur->a[it-'a'];
            cur->cp--;
        }
        cur->ew--;
    }
};

Link - https://www.codingninjas.com/codestudio/problems/implement-trie_1387095
```

## 5. Longest Word With All Prefixes | Complete String

**Q:** Given an array of strings, find the longest string such that every prefix of it is also present in the array.

**A:** Trie with complete path validation.

**Explanation:**
- Insert all strings from the array into a Trie and mark the end nodes.
- For each string, traverse its characters in the Trie and verify that every node along its path has `flag == true`.
- Maintain the longest valid string found, breaking ties lexicographically.

**Complexity:** Time $O(\sum L_i)$, Space $O(\sum L_i \times 26)$ where $L_i$ are string lengths.

```cpp
struct Node{
    Node *a[26];
    bool flag;
    Node(){
        for(int i=0;i<26;i++){
            a[i]=NULL;
        }
        flag=false;
    }
};
class Trie{
    Node *root;
    public:
    Trie(){
        root=new Node();
    }
    void insert(string &word){
        Node *cur=root;
        for(auto it : word){
            if(!cur->a[it-'a']){
                cur->a[it-'a']=new Node();
            }
            cur=cur->a[it-'a'];
        }
        cur->flag=true;
    }
    bool checkPrefixExist(string &word){
        Node *cur=root;
        for(auto it : word){
            if(cur->a[it-'a']){
                cur=cur->a[it-'a'];
                if(!cur->flag) return false;
            }
            else return false;
        }
        return true;
    }
};
string completeString(int n, vector<string> &a){
    // Write your code here.
    Trie *trie=new Trie();
    for(auto it : a){
        trie->insert(it);
    }
    string ans="";
    for(auto it : a){
        if(trie->checkPrefixExist(it)){
            if(it.size()>ans.size()) ans=it;
            else if(it.size()==ans.size() && it<ans) ans=it;
        }
    }
    if(ans.size()==0) return "None";
    return ans;
}

Link - https://www.codingninjas.com/codestudio/problems/complete-string_2687860
```

## 6. Maximum XOR of two numbers in an array

**Q:** Given an array of numbers, find the maximum possible XOR of any two elements in the array.

**A:** Binary Trie with greedy descent.

**Explanation:**
- Insert all elements into a binary Trie, where each node has 2 children (0 and 1) representing bits from MSB (31) to LSB (0).
- For each number in the array, find its maximum XOR pair by traversing the Trie.
- At each bit, greedily try to traverse to the opposite bit (to maximize the XOR result to 1); if not available, traverse to the same bit.

**Complexity:** Time $O(n \times 32)$, Space $O(n \times 32)$ where $n$ is array size.

```cpp
    class TrieNode{
        public:
            TrieNode *left;
            TrieNode *right;
            TrieNode(){
                left=NULL;
                right=NULL;
            }
    };
    
    void insert(TrieNode *head,int n){
        TrieNode *cur=head;
        for(int i=31;i>=0;i--){
            int b=(n>>i & 1);
            if(b){
                if(!cur->right){
                    cur->right=new TrieNode();
                }
                cur=cur->right;
            }else{
                if(!cur->left){
                    cur->left=new TrieNode();
                }
                cur=cur->left;
            }
        }
    }
    
    int max_xor(int arr[] , int n){
        TrieNode *head=new TrieNode();
        for(int i=0;i<n;i++){
            insert(head,arr[i]);
        }
        int ans=INT_MIN;
        for(int i=0;i<n;i++){
            int val=arr[i];
            int xors=0;
            TrieNode *cur=head;
            for(int j=31;j>=0;j--){
                int b=(val>>j & 1);
                if(b){
                    if(cur->left){
                        xors+=pow(2,j);
                        cur=cur->left;
                    }else{
                        cur=cur->right;
                    }
                }else{
                    if(cur->right){
                        xors+=pow(2,j);
                        cur=cur->right;
                    }else{
                        cur=cur->left;
                    }
                }
            }
            ans=max(ans,xors);
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/maximum-xor-of-two-numbers-in-an-array/0
```

## 7. Minimum XOR value pair

**Q:** Given an array of numbers, find the minimum XOR value of any two distinct elements in the array.

**A:** Binary Trie with greedy descent for minimum.

**Explanation:**
- Iterate through the array, querying the minimum XOR of the current element with previously inserted elements, and then insert the current element into a binary Trie.
- To minimize the XOR result, greedily try to traverse to the *same* bit in the Trie (to make the XOR result 0 at that bit).
- If the same bit doesn't exist, traverse to the opposite bit and add $2^i$ to the XOR sum.

**Complexity:** Time $O(n \times 32)$, Space $O(n \times 32)$ where $n$ is array size.

```cpp
// Type 1...
class Trie{
    public:
        Trie *left;
        Trie *right;
        Trie(){
            left=NULL;
            right=NULL;
        }
};

class Solution{   
public:

    void insert(Trie *head,int n){
        Trie *cur=head;
        for(int i=31;i>=0;i--){
            int b=(n>>i & 1);
            if(b){
                if(!cur->right){
                    cur->right=new Trie();
                }
                cur=cur->right;
            }else{
                if(!cur->left){
                    cur->left=new Trie();
                }
                cur=cur->left;
            }
        }
    }
    
    int min_xor(Trie *head,int val){
        int xors=0;
        Trie *cur=head;
        for(int j=31;j>=0;j--){
            int b=(val>>j & 1);
            if(b){
                if(cur->right){
                    cur=cur->right;
                }else{
                    cur=cur->left;
                    xors+=pow(2,j);
                }
            }else{
                if(cur->left){
                    cur=cur->left;
                }else{
                    cur=cur->right;
                    xors+=pow(2,j);
                }
            }
        }
        return xors;
    }
    
    int minxorpair(int N, int arr[]){
        Trie *head=new Trie();
        
        insert(head,arr[0]);
        int ans=INT_MAX;
        for(int i=1;i<N;i++){
            ans=min(ans,min_xor(head,arr[i]));
            insert(head,arr[i]);
        }
        return ans;
    }
    

// Type 2....
class TrieNode
{
public:
    TrieNode *child[2];
    TrieNode()
    {
        this->child[0] = NULL;
        this->child[1] = NULL;
    }
};
class Solution
{
public:
    TrieNode *root;
    void insert(int x)
    {
        TrieNode *curr = root;
        bitset<32> bt(x);
        for (int i = 31; i >= 0; i--)
        {
            if (curr->child[bt[i]] == NULL)
                curr->child[bt[i]] = new TrieNode();
            curr = curr->child[bt[i]];
        }
    }
    int minXOR(int x)
    {
        int ans = 0;
        TrieNode *curr = root;
        bitset<32> bt(x);
        for (int i = 31; i >= 0; i--)
        {
            if (curr->child[bt[i]])
                curr = curr->child[bt[i]];
            else
            {
                ans += 1 << i;
                curr = curr->child[!bt[i]];
            }
        }
        return ans;
    }

    int minxorpair(int N, int arr[])
    {
        root = new TrieNode();
        insert(arr[0]);
        int ans = INT_MAX;
        for (int i = 1; i < N; i++)
        {
            ans = min(ans, minXOR(arr[i]));
            insert(arr[i]);
        }
        return ans;
    }
};

Link - https://practice.geeksforgeeks.org/problems/minimum-xor-value-pair/1
```

## 8. Trie | (Insert and Search)

**Q:** Implement basic Trie insert and search operations.

**A:** Standard Trie.

**Explanation:**
- Use a `Node` structure with an array of 26 pointers and a boolean `flag`.
- `insert` follows the string characters, allocating nodes as needed and setting the `flag`.
- `search` follows the string and checks the `flag` of the final node. `isPrefix` does the same but returns true if the traversal completes without hitting a NULL pointer.

**Complexity:** Time $O(L)$ per operation where $L$ is word length. Space $O(\text{Total characters} \times 26)$.

```cpp
struct Node{
    Node* link[26];
    bool flag;
    Node(){
        for(int i=0;i<26;i++){
            link[i]=NULL;
        }
        flag=false;
    }
};

class Trie {
  public:
    Node *head;
    Trie() {
        head=new Node();
    }

    void insert(string &word) {
        // insert word into Trie
        Node *ptr=head;
        for(auto it : word){
            if(!ptr->link[it-'a']) ptr->link[it-'a']=new Node();
            ptr=ptr->link[it-'a'];
        }
        ptr->flag=true;
    }

    bool search(string &word) {
        // search word in the Trie
        Node *ptr=head;
        for(auto it : word){
            if(!ptr->link[it-'a']) return false;
            ptr=ptr->link[it-'a'];
        }
        return ptr->flag;
    }

    bool isPrefix(string &word) {
        // search prefix word in the Trie
        Node *ptr=head;
        for(auto it : word){
            if(!ptr->link[it-'a']) return false;
            ptr=ptr->link[it-'a'];
        }
        return true;
    }
};

Link - https://practice.geeksforgeeks.org/problems/trie-insert-and-search0651/1
```
