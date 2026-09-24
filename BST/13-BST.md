# Binary Search Tree — Q&A

> Source: `Kirancse47/DSA` -> `BST/` — 19 problems

## Cheat sheet
The defining BST invariant is that EVERY node in the left subtree is strictly less than the root, and EVERY node in the right subtree is strictly greater than the root—not just the immediate children. A common bug in `98. Validate Binary Search Tree` is checking only local children instead of passing a narrowing `(min, max)` range down the recursion.

The most exploited property of a BST is that an **inorder traversal yields elements in sorted order**. This is used directly in `230. Kth Smallest Element in a BST` (counting elements) and `1382. Balance a Binary Search Tree` (extracting to a sorted array, then rebuilding). For a greater-sum tree (`1038. Binary Search Tree to Greater Sum Tree`), a reverse-inorder traversal (right-root-left) keeps a running sum of larger elements.

Core operations (search, insert, delete) run in `O(h)` time and `O(h)` recursion space, where `h` is the tree height. Note that `h` is `O(log n)` in a balanced tree but degrades to `O(n)` if the tree is skewed. `700. Search in a Binary Search Tree` and `701. Insert into a Binary Search Tree` simply follow the BST property left or right.

**Deletion** (`450. Delete Node in a BST`) has three cases:
1. Leaf node: simply remove it.
2. One child: replace the node with its only child.
3. Two children: find the inorder successor (smallest in the right subtree), copy its value, and delete the successor.

**Traversal tricks:**
- **LCA** (`235. Lowest Common Ancestor of a Binary Search Tree`): Walk down while both targets are on the same side of the current node; the divergence point is the LCA.
- **Floor/Ceil/Successor**: Walk the tree, updating a candidate answer whenever moving left (for successor/ceil) or right (for predecessor/floor). Seen in `Ceil in BST`, `Closest Neighbour in BST`, and `Inorder Successor in BST`.
- **Validation**: Pass a range `(min, max)` updating the max when going left, and the min when going right.

---

## 1. 1008. Construct Binary Search Tree from Preorder Traversal

**Q:** Given a preorder traversal array of a BST, construct the BST and return its root.

**A:** Recursive reconstruction using an upper bound to group subtree nodes.

**Explanation:**
- Exploits preorder layout (root, left, right) combined with BST properties to know when a subtree ends.
- Maintains a global index to consume the array and an upper bound (`ub`) that restricts left-child values.
- Base case stops when the index reaches the end or the current value exceeds `ub`.

**Complexity:** Time `O(n)` since each element is processed once. Space `O(h)` for recursion depth.

```cpp
TreeNode* buildTree(vector<int>& preorder,int  &idx,int ub){
    if(idx==preorder.size() || ub<preorder[idx]) return NULL;
    TreeNode* node=new TreeNode(preorder[idx++]);
    node->left=buildTree(preorder,idx,node->val);
    node->right=buildTree(preorder,idx,ub);
    return node;
}

TreeNode* bstFromPreorder(vector<int>& preorder) {
    int n=preorder.size();
    if(n==0) return NULL;
    int idx=0;
    return buildTree(preorder,idx,INT_MAX);
}
```

---

## 2. 1038. Binary Search Tree to Greater Sum Tree

**Q:** Given the root of a BST, update every node's value to be the original value plus the sum of all strictly greater values in the tree.

**A:** Reverse inorder traversal with a running sum.

**Explanation:**
- Exploits the fact that a right-root-left traversal visits nodes in descending order.
- Carries a reference to a running `sum` state across the recursion.
- Base case simply returns on null pointers without updating the sum.

**Complexity:** Time `O(n)` to visit all nodes. Space `O(h)` for the recursion stack.

```cpp
void trav(TreeNode* root,int &sum){
    if(root){
        trav(root->right,sum);
        sum+=root->val;
        root->val=sum;
        trav(root->left,sum);
    }
}

TreeNode* bstToGst(TreeNode* root) {
    int sum=0;
    trav(root,sum);
    return root;
}
```

---

## 3. 1382. Balance a Binary Search Tree

**Q:** Given a potentially skewed BST, return a balanced BST containing the same node values.

**A:** Flatten the tree to a sorted array via inorder traversal, then recursively rebuild it.

**Explanation:**
- Inorder traversal guarantees an inherently sorted array of values.
- The tree is rebuilt by picking the middle of the current array segment as the root, ensuring the left and right halves are as equal as possible.
- Base case returns null when the start index exceeds the end index.

**Complexity:** Time `O(n)` to traverse and rebuild. Space `O(n)` to store the sorted array of values.

```cpp
void inorder(TreeNode* root,vector<int> &ans){
    if(!root) return;
    inorder(root->left,ans);
    ans.push_back(root->val);
    inorder(root->right,ans);
}

TreeNode* solve(int s,int e,vector<int> &ans){
    if(s>e) return NULL;
    int m=s+(e-s)/2;
    TreeNode* root=new TreeNode(ans[m]);
    root->left=solve(s,m-1,ans);
    root->right=solve(m+1,e,ans);
    return root;
}

TreeNode* balanceBST(TreeNode* root) {
    vector<int> ans;
    inorder(root,ans);
    return solve(0,ans.size()-1,ans);
}
```

---

## 4. 173. Binary Search Tree Iterator

**Q:** Implement an iterator over a BST that provides elements in ascending order via `next()` and `hasNext()` methods.

**A:** Iterative inorder traversal using an explicit stack to pause and resume.

**Explanation:**
- Exploits inorder sorting but defers right-subtree exploration until the root is popped.
- Maintains a stack storing the path of left children to simulate the recursion stack.
- `next()` pops the smallest element and then pushes all left children of its right subtree.

**Complexity:** Time `O(1)` amortized per `next()` call, as every node is pushed and popped exactly once overall. Space `O(h)` for the maximum stack depth.

```cpp
stack<TreeNode*> st;

void pushA(TreeNode* node){
    while(node){
        st.push(node);
        node=node->left;
    }
}

BSTIterator(TreeNode* root) {
    pushA(root);
}

int next() {
    TreeNode *t=st.top();st.pop();
    pushA(t->right);
    return t->val;
}

bool hasNext() {
    return !st.empty();
}
```

---

## 5. 230. Kth Smallest Element in a BST

**Q:** Given a BST, find the kth smallest value (1-indexed).

**A:** Early-exit inorder traversal with a decrementing counter.

**Explanation:**
- Inorder traversal guarantees elements are processed in strict ascending order.
- Passes a reference to `k` down the recursion, decrementing it at each root.
- When `k` reaches 0, the current node is captured in `ans` and further processing can stop.

**Complexity:** Time `O(h + k)` as we stop at the kth element. Space `O(h)` for recursion.

```cpp
void inorder(TreeNode* root,int &k,int &ans){
    if(!root) return;
    inorder(root->left,k,ans);
    k--;
    if(k==0){
        ans=root->val;
        return;
    }
    inorder(root->right,k,ans);
}

int kthSmallest(TreeNode* root, int k) {
    int ans=-1;
    inorder(root,k,ans);
    return ans;
}
```

---

## 6. 235. Lowest Common Ancestor of a Binary Search Tree

**Q:** Find the lowest common ancestor (LCA) node of two given nodes in a BST.

**A:** Single-path descent until the nodes diverge.

**Explanation:**
- Exploits the invariant that the LCA must lie between the values of `p` and `q`.
- Walks down left if both `p` and `q` are smaller than the root, or right if both are larger.
- Base case triggers when the root's value splits the two nodes (or matches one), returning the root as the LCA.

**Complexity:** Time `O(h)` because it traverses down exactly one path without branching. Space `O(h)` for the recursive call stack.

```cpp
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if(!root) return root;
    if(root->val > p->val && root->val > q->val) return lowestCommonAncestor(root->left,p,q);
    else if(root->val < p->val && root->val < q->val) return lowestCommonAncestor(root->right,p,q);
    else return root;
}
```

---

## 7. 450. Delete Node in a BST

**Q:** Given a BST and a key, remove the node with that key while maintaining the BST properties.

**A:** Recursive search followed by structural splicing or inorder-successor replacement.

**Explanation:**
- Traverses down based on the BST invariant to find the target node.
- If the node has 0 or 1 child, it is cleanly replaced by its non-null child (or null).
- If the node has 2 children, its value is swapped with its inorder successor (smallest node in the right subtree), and the successor is then recursively deleted.

**Complexity:** Time `O(h)` to find the node and potentially find the successor. Space `O(h)` for the recursion stack.

```cpp
TreeNode* inorderSucc(TreeNode* root){
    TreeNode *cur=root;
    while(cur && cur->left){
        cur=cur->left;
    }
    return cur;
}

TreeNode* deleteNode(TreeNode* root, int key) {
    if(!root) return root;
    if(key>root->val){
        root->right=deleteNode(root->right,key);
    }
    else if(key<root->val){
        root->left=deleteNode(root->left,key);
    }
    else{
        if(!root->left){
            TreeNode *t=root->right;
            delete(root);
            return t;
        }
        else if(!root->right){
            TreeNode *t=root->left;
            delete(root);
            return t;
        }
        else{
            TreeNode *t=inorderSucc(root->right);
            root->val=t->val;
            root->right=deleteNode(root->right,root->val);
        }
    }
    return root;
}
```

---

## 8. 653. Two Sum IV - Input is a BST

**Q:** Determine if there exist two different nodes in a BST such that their values sum to a given target `k`.

**A:** Inorder flatten followed by the two-pointer sorted array technique.

**Explanation:**
- Inorder traversal converts the BST into a strictly sorted array.
- Two pointers (`i` at start, `j` at end) scan inwards, moving `i` right if the sum is too small or `j` left if it's too large.
- Base case for the search stops when the two pointers meet.

**Complexity:** Time `O(n)` for flattening and the two-pointer pass. Space `O(n)` to store the array.

```cpp
void inorder(TreeNode* root,vector<int> &a){
    if(!root) return;
    inorder(root->left,a);
    a.push_back(root->val);
    inorder(root->right,a);
}

bool findTarget(TreeNode* root, int k) {
    vector<int> a;
    inorder(root,a);
    int i=0,j=a.size()-1;
    while(i<j){
        int s=a[i]+a[j];
        if(s==k) return true;
        else if(s>k) j--;
        else i++;
    }
    return false;
}
```

---

## 9. 700. Search in a Binary Search Tree

**Q:** Search a BST for a specific value and return the subtree rooted at that node.

**A:** Simple structural descent matching the target value.

**Explanation:**
- Exploits the core BST property: go left if the target is smaller, right if it's larger.
- Both recursive and iterative approaches simply follow a single path from root to target.
- Base cases hit when a match is found or the traversal reaches a null pointer.

**Complexity:** Recursive: Time `O(h)`, Space `O(h)`. Iterative: Time `O(h)`, Space `O(1)`.

```cpp
// Recursive...
TreeNode* searchBST(TreeNode* root, int val) {
    if(!root) return NULL;
    if(val==root->val) return root;
    else if(val>root->val) return searchBST(root->right,val);
    else return searchBST(root->left,val);
}

// Iterative...
TreeNode* searchBST(TreeNode* root, int val) {
    if(!root) return NULL;
    TreeNode* cur=root;
    while(cur){
        if(val==cur->val) return cur;
        else if(val>cur->val) cur=cur->right;
        else cur=cur->left;
    }
    return NULL;
}
```

---

## 10. 701. Insert into a Binary Search Tree

**Q:** Insert a new value into a BST such that it remains a valid BST.

**A:** Structural descent to find the correct leaf insertion point.

**Explanation:**
- Follows the BST invariant down a single path to locate an empty spot.
- Iteratively tracks a `prev` pointer to attach the newly allocated node.
- Base case (or loop exit) happens when a null child is reached.

**Complexity:** Recursive: Time `O(h)`, Space `O(h)`. Iterative: Time `O(h)`, Space `O(1)`.

```cpp
// Iterative...
TreeNode* insertIntoBST(TreeNode* root, int val) {
    if(!root) return new TreeNode(val);
    TreeNode* cur=root,*prev=root;
    while(cur){
        prev=cur;
        if(val>cur->val) cur=cur->right;
        else cur=cur->left;
    }
    if(val>prev->val) prev->right=new TreeNode(val);
    else prev->left=new TreeNode(val);
    return root;
}

// Recursive...
TreeNode* insertIntoBST(TreeNode* root, int val) {
    if(!root) return new TreeNode(val);
    if(val > root->val) root->right=insertIntoBST(root->right,val);
    else root->left=insertIntoBST(root->left,val);
    return root;        
}
```

---

## 11. 98. Validate Binary Search Tree

**Q:** Determine if a given binary tree strictly satisfies all properties of a Binary Search Tree.

**A:** Recursive descent passing strict narrowing `(min, max)` boundary constraints.

**Explanation:**
- Prevents the common bug of only checking immediate children by ensuring every node respects the bounds of its ancestors.
- Updating the max bound when moving left and the min bound when moving right enforces global correctness.
- Base case considers a null node to be inherently valid.

**Complexity:** Time `O(n)` to validate every node. Space `O(h)` for the recursion.

```cpp
bool isValidBST(TreeNode* root,long mn=LONG_MIN,long mx=LONG_MAX) {
    if(!root) return true;
    if(root->val >= mx || root->val <= mn) return false;
    return (isValidBST(root->left,mn,root->val) && isValidBST(root->right,root->val,mx));
}
```

---

## 12. 99. Recover Binary Search Tree

**Q:** Recover a BST where exactly two nodes have had their values mistakenly swapped.

**A:** Inorder traversal tracking out-of-order adjacent pairs.

**Explanation:**
- Exploits the fact that an inorder traversal of a valid BST is strictly increasing.
- Uses pointers (`first`, `mid`, `last`, `prev`) to record drops in sequence where `prev->val > root->val`.
- At the end, the values of `first` and either `last` (if nodes were non-adjacent) or `mid` (if adjacent) are swapped back.

**Complexity:** Time `O(n)` to visit all nodes. Space `O(h)` for the recursion stack (though code comment mentions s-O(1), standard recursion is O(h)).

```cpp
// t-O(n) s-O(1)
TreeNode* first, *prev, *mid, *last;
void inorder(TreeNode* root){
    if(!root) return;
    inorder(root->left);
    if(prev && root->val < prev->val){
        if(!first){
            first=prev;
            mid=root;
        }else{
            last=root;
        }
    }
    prev=root;
    inorder(root->right);
}

void recoverTree(TreeNode* root) {
    if(!root) return;
    first=NULL;mid=NULL;last=NULL;
    prev=new TreeNode(INT_MIN);
    inorder(root);
    if(first && last){
        swap(first->val,last->val);
    }else if(first && mid){
        swap(first->val,mid->val);
    }
}
```

---

## 13. Ceil in BST

**Q:** Find the smallest value in the BST that is greater than or equal to a given input.

**A:** Downward walk keeping track of the last seen larger-or-equal candidate.

**Explanation:**
- Exploits the BST property to prune halves of the tree; goes left if the current node is too large, right if too small.
- Updates the `ans` state only when branching left (i.e., when finding a new tighter ceiling).
- Stops immediately if an exact match is found.

**Complexity:** Time `O(h)` to traverse the path. Space `O(1)` for the iterative pointer.

```cpp
int findCeil(Node* root, int input) {
    if (root == NULL) return -1;
    int ans=-1;
    Node* cur=root;
    while(cur){
        if(input==cur->data) return input;
        else if(input<cur->data){
            ans=cur->data;
            cur=cur->left;
        }else cur=cur->right;
    }
    return ans;
}

Link - https://practice.geeksforgeeks.org/problems/implementing-ceil-in-bst/0/?category[]=Binary%20Search%20Tree&page=1&sortBy=accuracy&query=category[]Binary%20Search%20Treepage1sortByaccuracy#
```

---

## 14. Closest Neighbour in BST

**Q:** Find the largest value in the BST that is less than or equal to a given number `n`.

**A:** Downward walk updating a candidate answer whenever moving right.

**Explanation:**
- Effectively finds the "floor" of `n`.
- The answer candidate is updated exclusively when the current node is smaller than `n`, right before searching for a tighter lower bound in the right subtree.
- Base case triggers on a null pointer or an exact match.

**Complexity:** Time `O(h)` for a single root-to-leaf path. Space `O(h)` for recursion.

```cpp
void solve(Node *root,int n,int &ans){
    if(!root) return;
    if(root->key == n){
        ans=root->key;
        return;
    }
    else if(root->key <n){
        ans=max(ans,root->key);
        solve(root->right,n,ans);
    }
    else{
        solve(root->left,n,ans);
    }
}

int findMaxForN(Node* root, int n) {
    int ans=INT_MIN;
    solve(root,n,ans);
    if(ans==INT_MIN) return -1;
    return ans;
}

gfg link - https://www.geeksforgeeks.org/problems/closest-neighbor-in-bst/1
```

---

## 15. Inorder Successor in BST

**Q:** Given a BST and a node `x`, find the node that would immediately follow `x` in an inorder traversal.

**A:** Iterative downward search logging potential successors.

**Explanation:**
- Eliminates the need for parent pointers by starting the search from the root.
- Whenever a node larger than `x` is found, it is recorded as a candidate `succ`, and the search continues left to find a tighter bound.
- If the node is smaller than or equal to `x`, the search moves right.

**Complexity:** Time `O(h)` to walk the tree depth. Space `O(1)` since it runs iteratively.

```cpp
Node * inOrderSuccessor(Node *root, Node *x){
    Node *cur=root,*succ=NULL;
    while(cur){
        if(cur->data > x->data){
            succ=cur;
            cur=cur->left;
        }
        else cur=cur->right;
    }
    return succ;
}

Link - https://practice.geeksforgeeks.org/problems/inorder-successor-in-bst/1#
```

---

## 16. Kth common ancestor in BST

**Q:** Find the kth lowest common ancestor for two nodes in a BST (where k=1 is the lowest, k=2 is its parent, etc.).

**A:** LCA path discovery recording all ancestors in an array.

**Explanation:**
- Walks the standard BST LCA path (going left if both are smaller, right if both are larger).
- Pushes every visited node on the valid path into an array sequence.
- Returns the element located `k` steps backward from the end of the array (`n - k`).

**Complexity:** Time `O(h)` to walk down to the divergence point. Space `O(h)` to store the path array and for the recursion stack.

```cpp
void solve(Node *root,int k,int x,int y,vector<int> &ans){
    if(!root) return;
    ans.push_back(root->data);
    if(root->data < x && root->data < y){
        solve(root->right, k, x, y, ans);
    }else if(root->data > x && root->data > y){
        solve(root->left, k, x, y, ans);
    }
}

int kthCommonAncestor(Node *root, int k,int x, int y){
    vector<int> ans;
    solve(root,k,x,y,ans);
    int n = ans.size();
    if(n < k) return -1;
    return ans[n-k];
}

gfg link - https://www.geeksforgeeks.org/problems/kth-common-ancestor-in-bst/1
```

---

## 17. Largest BST

**Q:** Find the size of the largest subtree within a binary tree that is itself a valid BST.

**A:** Bottom-up postorder traversal returning structural bounds (`min`, `max`, `size`).

**Explanation:**
- Evaluates children first to check if a valid BST can be formed with the current root.
- A node forms a valid BST only if its value is strictly between the maximum of its left subtree and the minimum of its right subtree.
- If invalid, it bubbles up sentinel values (`INT_MAX` for max, `INT_MIN` for min) to forcibly invalidate all ancestors.

**Complexity:** Time `O(n)` as every node is visited exactly once. Space `O(h)` for the recursive stack.

```cpp
struct nodeVal{
    int maxNode,minNode,maxSize;
};

class Solution{
    public:
    nodeVal solve(Node* root){
        if(!root) return {INT_MIN,INT_MAX,0};
        auto left=solve(root->left);
        auto right=solve(root->right);
        if(left.maxNode<root->data && root->data<right.minNode){
            return {max(root->data,right.maxNode),min(root->data,left.minNode),1+left.maxSize+right.maxSize};
        }
        else{
            return {INT_MAX,INT_MIN,max(left.maxSize,right.maxSize)};
        }
    }
    int largestBst(Node *root){
        return solve(root).maxSize;
    }
};

Link - https://practice.geeksforgeeks.org/problems/largest-bst/1/
```

---

## 18. Minimum Absolute Difference In BST

**Q:** Find the minimum absolute difference between the values of any two different nodes in a BST.

**A:** Inorder traversal recording the previous node's value.

**Explanation:**
- Exploits the fact that the minimum difference must occur between two adjacent elements in the sorted (inorder) sequence.
- Maintains `prev` state across recursive calls to compare with the current node's value.
- Updates a global `ans` minimum difference if `abs(prev - root->data)` is strictly smaller.

**Complexity:** Time `O(n)` to traverse all elements. Space `O(h)` for recursion depth.

```cpp
void solve(Node *root,int &ans,int &prev){
    if(!root) return;
    solve(root->left,ans,prev);
    ans=min(ans,abs(prev-root->data));
    prev=root->data;
    solve(root->right,ans,prev);
}
int absolute_diff(Node *root){
    int ans=INT_MAX,prev=INT_MAX;
    solve(root,ans,prev);
    return ans;
}

gfg link - https://www.geeksforgeeks.org/problems/minimum-absolute-difference-in-bst-1665139652/1
```

---

## 19. Preorder Traversal and BST

**Q:** Determine if a given array represents a valid preorder traversal of a BST.

**A:** Iterative monotonic stack tracking the current subtree's lower bound root.

**Explanation:**
- Preorder visits root, then left, then right.
- A stack keeps track of paths; when a larger value appears, we've moved to a right subtree, so we pop nodes to find the new lower bound `root`.
- The array is invalid if any subsequent element violates the stored lower bound (`arr[i] < root`).

**Complexity:** Time `O(n)` as each array element is pushed and popped at most once. Space `O(n)` in the worst case for the stack.

```cpp
int canRepresentBST(int arr[], int N) {
    // code here
    stack<int> s;
    int root=INT_MIN;
    for(int i=0;i<N;i++){
        if(arr[i]<root) return 0;
        while(!s.empty() && s.top()<arr[i]){
            root=s.top();s.pop();
        }
        s.push(arr[i]);
    }
    return 1;
}

Link - https://practice.geeksforgeeks.org/problems/preorder-traversal-and-bst4006/1
```
