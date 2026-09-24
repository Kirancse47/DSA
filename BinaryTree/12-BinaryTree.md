# Binary Tree — Q&A

> Source: `Kirancse47/DSA` -> `BinaryTree/` — 59 problems.
> Every answer carries the complete solution code exactly as it appears in the repo.

Ordering: alphabetical, matching the source directory.

## Cheat sheet

### Node
```cpp
struct TreeNode { int val; TreeNode *left, *right; };
```

### Traversals

| Traversal | Order | Recursive | Iterative | Typical use |
|---|---|---|---|---|
| Preorder | root, left, right | trivial | one stack, push right then left | serialize, copy a tree |
| Inorder | left, root, right | trivial | stack, go left then pop then go right | sorted order in a BST |
| Postorder | left, right, root | trivial | two stacks, or one stack with a last-visited pointer | delete a tree, bottom-up aggregates |
| Level order | breadth first | n/a | queue, process one level per outer iteration | width, views, zigzag, levels |
| Morris | inorder / preorder | n/a | thread the tree via rightmost predecessor | `O(1)` space traversal |

### The two shapes of recursion
1. **Top-down** — pass information DOWN as parameters (depth, path sum, valid range).
   Used by: path sum, validate BST, root-to-leaf paths.
2. **Bottom-up** — return information UP from children, combine at the parent.
   Used by: height, diameter, balanced check, LCA, max path sum.
   The classic trick: have the helper RETURN the height while UPDATING a global answer,
   so diameter/max-path costs one `O(n)` pass instead of `O(n^2)`.

### Complexity conventions used in this file
`n` = number of nodes, `h` = height, `w` = maximum width of any level.
- Any full traversal is `O(n)` time.
- Recursive space is `O(h)`: `O(log n)` for a balanced tree, `O(n)` for a skewed one.
- Level-order space is `O(w)`, worst case `O(n)` at the widest level.
- Morris traversal is `O(1)` extra space.

### Recurring problems and their signature move
- Height / depth: `1 + max(left, right)`.
- Diameter: `left_height + right_height` at each node, tracked globally.
- Balanced: return -1 upward as a sentinel for "already unbalanced".
- LCA: return the node if it matches, else the non-null side; if both sides are
  non-null this node IS the LCA.
- Views (left/right/top/bottom): level-order BFS plus a map keyed by horizontal distance.
- Serialize/deserialize: preorder with an explicit null marker.

---

## 100. Same Tree

**Q:** Given the root pointers of two binary trees `p` and `q`, determine if they are structurally identical and have the same node values.

**A:** Synchronous pre-order DFS traversing both trees simultaneously.

**Explanation:**
- The traversal compares the current nodes; if both are null they match, if one is null they do not.
- It recursively verifies that both the left and right subtrees of `p` and `q` are identical.
- The base case returns true when reaching two null pointers simultaneously, and false on value mismatch or structural mismatch.

**Complexity:** Time: `O(n)` where `n` is the minimum number of nodes in the two trees. Space: `O(h)` for the recursion stack, `O(log n)` if balanced, `O(n)` if skewed.

```cpp
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if(!p && !q) return true;
        if(!p || !q) return false;
        return ((p->val == q->val) && isSameTree(p->left,q->left) && isSameTree(p->right,q->right));
    }
```

## 101. Symmetric Tree

**Q:** Given the root pointer of a binary tree, determine whether it is a mirror of itself (symmetric around its center).

**A:** Synchronous DFS comparing the left subtree's left child with the right subtree's right child, and vice versa.

**Explanation:**
- A helper function takes two node pointers (initially the left and right children of the root) to compare opposite structural branches.
- The recursive step checks if `t1->left` mirrors `t2->right` AND `t1->right` mirrors `t2->left`.
- Base cases handle reaching null pointers simultaneously (true) or structural/value mismatches (false).

**Complexity:** Time: `O(n)` to visit all nodes. Space: `O(h)` for the recursion stack, `O(log n)` if balanced, `O(n)` if skewed.

```cpp
    bool isS(TreeNode* t1, TreeNode* t2){
        if(!t1 && !t2) return true;
        if(!t1 || !t2) return false;
        return ((t1->val==t2->val) && isS(t1->left,t2->right) && isS(t1->right,t2->left));
    }
    
    bool isSymmetric(TreeNode* root) {
        if(!root) return true;
        return isS(root->left, root->right);
    }
```

## 1026. Maximum Difference Between Node and Ancestor

**Q:** Given the root of a binary tree, find the maximum absolute difference between the values of any node and one of its ancestors.

**A:** Pre-order DFS tracking the minimum and maximum values encountered along the path from root to leaf.

**Explanation:**
- The recursive function pushes the current minimum and maximum values down the tree path.
- At each node, it updates a global/reference `ans` with the maximum absolute difference between the current node's value and the tracked path min/max.
- It then updates the path min/max including the current node's value before branching left and right.

**Complexity:** Time: `O(n)` to visit every node exactly once. Space: `O(h)` for the recursion stack, `O(log n)` if balanced, `O(n)` if skewed.

```cpp
    void solve(TreeNode* root,int &ans,int mn,int mx){
        if(!root) return;
        ans=max(ans,max(abs(root->val - mn),abs(root->val - mx)));
        mn=min(mn,root->val);
        mx=max(mx,root->val);
        solve(root->left,ans,mn,mx);
        solve(root->right,ans,mn,mx);
    }

    int maxAncestorDiff(TreeNode* root) {
        int ans=0,mn=root->val,mx=root->val;
        solve(root,ans,mn,mx);
        return ans;
    }
```

## 102. Binary Tree Level Order Traversal

**Q:** Given the root of a binary tree, return the level order traversal of its nodes' values as a list of lists.

**A:** Iterative breadth-first search (BFS) using a queue to process nodes level by level.

**Explanation:**
- A queue is initialized with the root; a while loop runs as long as the queue is not empty.
- Inside the loop, the current queue size dictates how many nodes are popped and processed for the current level, ensuring levels are kept separate.
- The values are collected into a level-specific vector, and non-null left and right children are enqueued for the next level.

**Complexity:** Time: `O(n)` to process every node. Space: `O(w)` where `w` is the maximum width of the tree (at most `n/2` for the last level).

```cpp
  vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> ans;
        if(!root) return ans;
        queue<TreeNode*> q;
        q.push(root);
        vector<int> res;
        while(!q.empty()){
            int size=q.size();
            res.clear();
            for(int i=0;i<size;i++){
                TreeNode *temp=q.front();q.pop();
                res.push_back(temp->val);
                if(temp->left) q.push(temp->left);
                if(temp->right) q.push(temp->right);
            }
            ans.push_back(res);
        }
        return ans;
    }
```

## 103. Binary Tree Zigzag Level Order Traversal

**Q:** Given the root of a binary tree, return the zigzag level order traversal (left to right, then right to left alternating) as a list of lists.

**A:** Iterative BFS using a queue with a boolean flag to toggle insertion direction for each level's array.

**Explanation:**
- Like standard level order traversal, a queue tracks nodes and current level size.
- A `ltr` (left-to-right) boolean flag determines whether to insert the popped node's value at the current index `i` or in reverse at `size - 1 - i`.
- The flag is negated after each level finishes processing.

**Complexity:** Time: `O(n)` to visit each node. Space: `O(w)` where `w` is the maximum width of the tree for the queue.

```cpp
    vector<vector<int>> zigzagLevelOrder(TreeNode* root) {
        vector<vector<int>> ans;
        if(!root) return ans;
        queue<TreeNode*> q;
        q.push(root);
        bool ltr=true;
        while(!q.empty()){
            int size=q.size();
            vector<int> res(size);
            for(int i=0;i<size;i++){
                TreeNode *temp=q.front();q.pop();
                int ind= ltr ? i : size-1-i;
                res[ind]=temp->val;
                if(temp->left) q.push(temp->left);
                if(temp->right) q.push(temp->right);
            }
            ltr=!ltr;
            ans.push_back(res);
        }
        return ans;
    }
```

## 104. Maximum Depth of Binary Tree(Height of BT)

**Q:** Given the root of a binary tree, return its maximum depth (the number of nodes along the longest path from root to leaf).

**A:** Post-order DFS returning the height of the current subtree by taking the maximum of left and right child heights plus one.

**Explanation:**
- The recursive function computes the height of the left and right subtrees.
- It returns `1 + max(left_height, right_height)` back up the recursion stack.
- The base case returns `0` when encountering a null pointer.

**Complexity:** Time: `O(n)` as every node is visited once. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
  int maxDepth(TreeNode* root) {
        if(!root) return 0;
        int l=maxDepth(root->left);
        int r=maxDepth(root->right);
        return 1+max(l,r);
    }
```

## 105. Construct Binary Tree from Preorder and Inorder Traversal

**Q:** Given two integer arrays representing the preorder and inorder traversals of a binary tree, construct and return the binary tree.

**A:** Divide and conquer recursively picking the root from preorder and finding its split point in a mapped inorder array.

**Explanation:**
- The first element of the current preorder segment is the root.
- A hash map is used to achieve `O(1)` lookups for the root's position within the inorder array.
- The inorder index cleanly splits the remaining elements into left and right subtrees, allowing calculation of sizes to advance the preorder indices correctly.

**Complexity:** Time: `O(n)` to build the map and construct the tree. Space: `O(n)` for the hash map plus `O(h)` recursion stack space.

```cpp
    TreeNode* build(vector<int> &in,int is,int ie,vector<int> &pre,int ps,int pe,unordered_map<int,int> &mp){
        if(is>ie || ps>pe) return NULL;
        TreeNode* ne=new TreeNode(pre[ps]);
        int pos=mp[pre[ps]];
        int numsLeft=pos-is;
        ne->left=build(in,is,pos-1,pre,ps+1,ps+numsLeft,mp);
        ne->right=build(in,pos+1,ie,pre,ps+numsLeft+1,pe,mp);
        return ne;
    }
    
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        int n=inorder.size();
        unordered_map<int,int> mp;
        for(int i=0;i<n;i++) mp[inorder[i]]=i;
        return build(inorder,0,n-1,preorder,0,n-1,mp);
    }
```

## 106. Construct Binary Tree from Inorder and Postorder Traversal

**Q:** Given two integer arrays representing the inorder and postorder traversals of a binary tree, construct and return the binary tree.

**A:** Divide and conquer recursively picking the root from the end of postorder and splitting the inorder array using a hash map.

**Explanation:**
- The last element of the current postorder segment represents the root node.
- A hash map provides `O(1)` lookups to find this root's index in the inorder array.
- The inorder index determines the size of the left subtree, dictating how to partition the postorder array for the subsequent left and right recursive calls.

**Complexity:** Time: `O(n)` to map values and construct the tree. Space: `O(n)` for the hash map plus `O(h)` recursion stack space.

```cpp
    TreeNode* build(vector<int> &in,int is,int ie,vector<int> &post,int ps,int pe,unordered_map<int,int> &mp){
        if(is>ie || ps>pe) return NULL;
        TreeNode* ne=new TreeNode(post[pe]);
        int pos=mp[post[pe]];
        int numsLeft=pos-is;
        ne->left=build(in,is,pos-1,post,ps,ps+numsLeft-1,mp);
        ne->right=build(in,pos+1,ie,post,ps+numsLeft,pe-1,mp);
        return ne;
    }
    
    TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
        int n=inorder.size();
        unordered_map<int,int> mp;
        for(int i=0;i<n;i++) mp[inorder[i]]=i;
        return build(inorder,0,n-1,postorder,0,n-1,mp);
    }
```

## 110. Balanced Binary Tree

**Q:** Given the root of a binary tree, determine if it is height-balanced (the left and right subtrees of every node differ in height by no more than 1).

**A:** Post-order DFS returning the height of subtrees while hijacking the return value (`-1`) to propagate imbalance up the stack.

**Explanation:**
- The optimized approach calculates the height of the left and right subtrees recursively.
- If any subtree is found to be unbalanced (difference in height > 1) or returns `-1`, the current call immediately returns `-1` upwards.
- The base case for a null node returns `0`, and a balanced node returns its true height (`1 + max(l, r)`).

**Complexity:** Brute Force: Time `O(n^2)` due to repeated height calculations, Space `O(h)`. Optimized: Time `O(n)` since each node is visited once, Space `O(h)` recursion stack (`O(log n)` balanced, `O(n)` skewed).

```cpp
// Brute Force...O(n^2)
    int Height(TreeNode* root) {
        if(!root) return 0;
        int l=Height(root->left);
        int r=Height(root->right);
        return 1+max(l,r);
    }
    bool isBalanced(TreeNode* root) {
        if(!root) return true;
        return (abs(Height(root->left)-Height(root->right))<=1) && isBalanced(root->left) && isBalanced(root->right);
    }
    
// Optimized...O(n)
    int dfsHeight(TreeNode* root) {
        if(!root) return 0;
        int l=dfsHeight(root->left);
        if(l==-1) return -1;
        int r=dfsHeight(root->right);
        if(r==-1) return -1;
        
        if(abs(l-r)>1) return -1;
        return 1+max(l,r);
    }
    bool isBalanced(TreeNode* root) {
        if(!root) return true;
        return dfsHeight(root)!=-1;
    }
```

## 112. Path Sum

**Q:** Given the root of a binary tree and an integer `targetSum`, return true if there exists a root-to-leaf path whose node values sum to `targetSum`.

**A:** Pre-order DFS accumulating the sum along the current path and checking it upon reaching a leaf node.

**Explanation:**
- The recursive function passes the running sum (excluding the current node's value until the check) down to child calls.
- The leaf check (`!root->left && !root->right`) adds the leaf's value to the running sum and compares it against `targetSum`.
- It returns true immediately if any path is valid, short-circuiting the remaining traversal.

**Complexity:** Time: `O(n)` in the worst case if the target sum is at the rightmost leaf or absent. Space: `O(h)` for the recursion stack (`O(log n)` balanced, `O(n)` skewed).

```cpp
bool solve(int sum,TreeNode* root, int targetSum){
        if(!root->left && !root->right){
            if(sum+root->val==targetSum) return true;
            else return false;
        }
        if(root->left && solve(root->val+sum,root->left,targetSum)) return true;
        if(root->right && solve(root->val+sum,root->right,targetSum)) return true;
        return false;
    }
    
    bool hasPathSum(TreeNode* root, int targetSum) {
        if(!root) return false;
        return solve(0,root,targetSum);
    }
```

## 113. Path Sum II

**Q:** Given the root of a binary tree and an integer `targetSum`, return all root-to-leaf paths where each path's node values sum to `targetSum`.

**A:** Backtracking DFS that builds a path vector and decrements the target sum as it descends.

**Explanation:**
- The recursive function pushes the current node's value to a reference `res` vector and subtracts it from the remaining target sum.
- If it hits a leaf and the remaining target perfectly matched the leaf's value, `res` is appended to the global `ans` array.
- The last inserted node is popped from `res` after both recursive calls finish to backtrack state.

**Complexity:** Time: `O(n^2)` worst case (all paths are valid, string copying per path) but typically `O(n)`. Space: `O(h)` for the recursion stack and path array (`O(log n)` balanced, `O(n)` skewed).

```cpp
    void solve(TreeNode* root,int t,vector<int> &res,vector<vector<int>> &ans){
        if(!root) return;
        res.push_back(root->val);
        if(!root->left && !root->right && t==root->val) ans.push_back(res);
        solve(root->left,t-root->val,res,ans);
        solve(root->right,t-root->val,res,ans);
        res.pop_back();
    }

    vector<vector<int>> pathSum(TreeNode* root, int targetSum) {
        vector<vector<int>> ans;
        vector<int> res;
        if(!root) return ans;
        solve(root,targetSum,res,ans);
        return ans;
    }
```

## 114. Flatten Binary Tree to Linked List

**Q:** Given the root of a binary tree, flatten it in-place to a right-leaning "linked list" following the tree's pre-order traversal.

**A:** Uses multiple approaches: reverse post-order DFS passing a `prev` pointer, iterative stack DFS, or O(1) space Morris traversal manipulating threaded links.

**Explanation:**
- **Recursive:** A reverse post-order (Right, Left, Root) traversal stores the previously visited node in a global `prev`, wiring the current node's right pointer to `prev` and left to null.
- **Iterative:** Simulates pre-order traversal with a stack, pushing the right child then the left, wiring the current popped node's right to the next item on top of the stack.
- **Morris Traversal:** Rewires the tree directly by finding the inorder predecessor of the current node and linking its right child to the current node's right child, flattening the left subtree progressively.

**Complexity:** Recursive: Time `O(n)`, Space `O(h)` recursion stack. Iterative: Time `O(n)`, Space `O(h)` explicit stack. Morris Traversal: Time `O(n)`, Space `O(1)`.

```cpp
// Recursive... T=O(N) C=O(N)
    TreeNode* prev=NULL;
    void flatten(TreeNode* root) {
        if(!root) return;
        flatten(root->right);
        flatten(root->left);
        root->right=prev;
        root->left=NULL;
        prev=root;
    }
    
//Itarative.....  T=O(N) C=O(N)
    void flatten(TreeNode* root) {
        if(!root) return;
        stack<TreeNode*> st;
        st.push(root);
        while(!st.empty()){
            TreeNode* cur=st.top();st.pop();
            if(cur->right) st.push(cur->right);
            if(cur->left) st.push(cur->left);
            if(!st.empty()){
                cur->right=st.top();
                cur->left=NULL;
            }
        }
    }
    
//Using Morist Traversal.....  T=O(N) C=O(1)
    void flatten(TreeNode* root) {
        if(!root) return;
        TreeNode* cur=root;
        while(cur){
            if(cur->left){
                TreeNode* prev=cur->left;
                while(prev->right) prev=prev->right;
                prev->right=cur->right;
                cur->right=cur->left;
                cur->left=NULL;
            }
            cur=cur->right;
        }
    }
```

## 124. Binary Tree Maximum Path Sum

**Q:** Given the root of a binary tree, return the maximum path sum of any non-empty path connecting any two nodes.

**A:** Post-order DFS returning the max single-leg branch sum while updating a global max for a potential overarching path.

**Explanation:**
- The recursion returns the maximum sum of a straight path extending downwards from the current node (`root->val + max(left, right)`).
- It bounds negative child paths at `0` using `max(0, pathSum(...))` so detrimental subtrees are excluded.
- The global/reference `ans` is updated at each node assuming the path curves through the current node linking the left and right subtrees (`root->val + left + right`).

**Complexity:** Time: `O(n)` to visit every node once. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
   int pathSum(TreeNode* root,int &ans){
        if(!root) return 0;
        int lp=max(0,pathSum(root->left,ans));
        int rp=max(0,pathSum(root->right,ans));
        ans=max(ans,root->val+lp+rp);
        return root->val+max(lp,rp);
    }
    
    int maxPathSum(TreeNode* root) {
        if(!root) return 0;
        int ans=INT_MIN;
        pathSum(root,ans);
        return ans;
    }
```

## 129. Sum Root to Leaf Numbers

**Q:** Given the root of a binary tree where each node contains a digit 0-9, return the total sum of all root-to-leaf numbers.

**A:** Pre-order DFS propagating a base-10 accumulated integer down to the leaves.

**Explanation:**
- The recursive function pushes the path's accumulated number (`res * 10 + current value`) downwards.
- When reaching a leaf (`!root->left && !root->right`), it updates the reference `ans` with the total string of digits interpreted as an integer.
- Base cases simply return out to prevent processing null pointers.

**Complexity:** Time: `O(n)` to traverse all nodes exactly once. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
    void solve(int res,TreeNode *root,int &ans){
        if(!root) return;
        if(!root->left && !root->right){
            ans+=res*10+root->val;
            return;
        }
        if(root->left) solve(res*10+root->val,root->left,ans);
        if(root->right) solve(res*10+root->val,root->right,ans);
    }

    int sumNumbers(TreeNode* root) {
        int ans=0;
        solve(0,root,ans);
        return ans;
    }
```

## 1325. Delete Leaves With a Given Value

**Q:** Given the root of a binary tree and an integer `target`, recursively delete all leaf nodes that have the value `target`.

**A:** Post-order DFS pruning nodes from the bottom up, modifying tree links via return values.

**Explanation:**
- The function recurses completely down to the leaves before performing any checks (post-order).
- Subtree return values overwrite the current node's `left` and `right` children, enforcing deletion.
- A node is deleted (by returning `nullptr`) if it becomes a leaf after pruning its children and matches the `target` value.

**Complexity:** Time: `O(n)` to visit all nodes. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
    TreeNode* removeLeafNodes(TreeNode* root, int target) {
        if (!root) return nullptr;
        root->left = removeLeafNodes(root->left, target);
        root->right = removeLeafNodes(root->right, target);
        if (!root->left && !root->right && root->val == target)
            return nullptr;
        return root;
    }
```

## 1367. Linked List in Binary Tree

**Q:** Given a singly linked list and a binary tree root, return true if the linked list elements correspond to a downward path within the tree.

**A:** BFS to find starting nodes matching the list head, launching a synchronous DFS downward check on candidates.

**Explanation:**
- A BFS queue traverses the tree searching for nodes whose value matches the linked list's `head->val`.
- A recursive helper function `isL` acts as a synchronous pre-order DFS, moving down both the linked list and tree.
- If the linked list hits null, the path is fully matched (true); if the tree hits null early or values mismatch, the path fails (false).

**Complexity:** Time: `O(n * min(n, l))` where `n` is tree nodes and `l` is list length; worst case checks every node as a starting point. Space: `O(w)` for the BFS queue plus `O(l)` for the DFS stack depth.

```cpp
    bool isL(ListNode* head,TreeNode* root){
        if(!head) return true;
        if(!root) return false;
        if(head->val!=root->val) return false;
        return (isL(head->next,root->left) || isL(head->next,root->right));
    }

    bool isSubPath(ListNode* head, TreeNode* root) {
        if(!head) return true;
        if(!root) return false;
        queue<TreeNode*>q;
        q.push(root);
        while(!q.empty()){
            auto it=q.front();q.pop();
            if(it->val==head->val){
                if(isL(head,it)) return true;
            }
            if(it->left) q.push(it->left);
            if(it->right) q.push(it->right);
        }
        return false;
    }
```

## 1448. Count Good Nodes in Binary Tree

**Q:** Given the root of a binary tree, return the count of "good" nodes, defined as a node having no ancestors with a greater value.

**A:** Pre-order DFS propagating the maximum value seen so far down the current path.

**Explanation:**
- The recursive function pushes `maxv` (the highest value seen from the root to this point) down to children.
- At each node, if the node's value is greater than or equal to `maxv`, it counts as a good node and updates the global/reference `gn`.
- The `maxv` parameter passed to the children is updated if the current node establishes a new maximum for the path.

**Complexity:** Time: `O(n)` to traverse each node once. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
    void solve(TreeNode *root,int maxv,int &gn){
        if(!root) return;
        if(root->val>=maxv){
            maxv=root->val;
            gn++;
        }
        solve(root->left,maxv,gn);
        solve(root->right,maxv,gn);
    }
    
    int goodNodes(TreeNode* root) {
        int gn=0;
        solve(root,root->val,gn);
        return gn;
    }
```

## 144. Binary Tree Preorder Traversal

**Q:** Given the root of a binary tree, return the preorder traversal (Root, Left, Right) of its nodes' values.

**A:** Straightforward recursive DFS, or an iterative approach using an explicit stack.

**Explanation:**
- **Recursive:** Appends the current node's value to a global/member vector, then recursively calls left, then right.
- **Iterative:** Employs a stack initialized with the root. Inside a loop, pops the node, records the value, and pushes the right child before the left child so left gets processed first.

**Complexity:** Recursive: Time `O(n)`, Space `O(h)` for the recursion stack. Iterative: Time `O(n)`, Space `O(h)` for the explicit stack.

```cpp
// Recursive...
    vector<int> ans;
    vector<int> preorderTraversal(TreeNode* root) {
        if(!root) return ans;
        ans.push_back(root->val);
        preorderTraversal(root->left);
        preorderTraversal(root->right);
        return ans;
    }
    
// Iterative...
    vector<int> preorderTraversal(TreeNode* root) {
        vector<int> ans;
        if(!root) return ans;
        stack<TreeNode*> st;
        st.push(root);
        while(!st.empty()){
            TreeNode *t=st.top();st.pop();
            ans.push_back(t->val);
            if(t->right) st.push(t->right);
            if(t->left) st.push(t->left);
        }
        return ans;
    }
```

## 145. Binary Tree Postorder Traversal

**Q:** Given the root of a binary tree, return the postorder traversal (Left, Right, Root) of its nodes' values.

**A:** Straightforward recursive DFS, or an iterative approach using two explicit stacks to simulate a reversed traversal.

**Explanation:**
- **Recursive:** Recurses left, recurses right, then pushes the current node's value to the answer array.
- **Iterative:** The first stack pushes the current node, then its left child, then its right child. Popped elements stream into a second stack which naturally flips the order (Root, Right, Left) into standard Postorder (Left, Right, Root) upon final popping.

**Complexity:** Recursive: Time `O(n)`, Space `O(h)` for the recursion stack. Iterative: Time `O(n)`, Space `O(n)` required to hold all nodes in the second stack.

```cpp
// Recursive...
    vector<int> ans;
    vector<int> postorderTraversal(TreeNode* root) {
        if(!root) return ans;
        postorderTraversal(root->left);
        postorderTraversal(root->right);
        ans.push_back(root->val);
        return ans;
    }

// Iterative...
    vector<int> postorderTraversal(TreeNode* root) {
        vector<int> ans;
        if(!root) return ans;
        stack<TreeNode*> st1,st2;
        st1.push(root);
        while(!st1.empty()){
            TreeNode *t=st1.top();st1.pop();
            st2.push(t);
            if(t->left) st1.push(t->left);
            if(t->right) st1.push(t->right);
        }
        while(!st2.empty()){
            ans.push_back(st2.top()->val);
            st2.pop();
        }
        return ans;
    }
```

## 199. Binary Tree Right Side View

**Q:** Given the root of a binary tree, return the values of the nodes you can see ordered from top to bottom when viewing the tree from the right side.

**A:** Iterative level-order BFS using a queue to extract the final element of each level.

**Explanation:**
- The algorithm processes nodes level-by-level using a standard queue loop initialized with the root.
- A nested loop processes all nodes at the current level length; the condition `if(i == n-1)` extracts only the very last node processed at that level (the rightmost node).
- Children are pushed left then right ensuring proper left-to-right processing order internally.

**Complexity:** Time: `O(n)` to visit all nodes. Space: `O(w)` where `w` is the maximum width of the tree for the queue.

```cpp
    vector<int> rightSideView(TreeNode* root) {
        vector<int> ans;
        if(!root) return ans;
        queue<TreeNode*> q;
        q.push(root);
        while(!q.empty()){
            int n=q.size();
            for(int i=0;i<n;i++){
                TreeNode* t=q.front();q.pop();
                if(i==n-1) ans.push_back(t->val);
                if(t->left) q.push(t->left);
                if(t->right) q.push(t->right);
            }
        }
        return ans;
    }
```

## 2196. Create Binary Tree From Descriptions

**Q:** Given a 2D array of tree descriptions `[parent, child, isLeft]`, construct the unique binary tree and return its root.

**A:** Hash map mapping integers to dynamically created `TreeNode` pointers to link descriptions, paired with a set to deduce the root.

**Explanation:**
- The first pass iterates descriptions, instantiates missing tree nodes into the hash map, links the child to the parent (left or right depending on the flag), and records the child value in a "seen" set.
- The root of the tree is strictly the node value that appears as a parent but never as a child.
- A second pass looks for the parent value not present in the children set to return the root node pointer.

**Complexity:** Time: `O(n)` where `n` is the number of descriptions, evaluating each edge twice. Space: `O(n)` for the hash map to hold node pointers and the children set.

```cpp
    TreeNode* createBinaryTree(vector<vector<int>>& des) {
        unordered_map<int,TreeNode*> mp;
        unordered_set<int> s;
        for(auto it : des){
            if(mp.find(it[0])==mp.end()) mp[it[0]]=new TreeNode(it[0]);
            if(mp.find(it[1])==mp.end()) mp[it[1]]=new TreeNode(it[1]);
            if(it[2]) mp[it[0]]->left=mp[it[1]];
            else mp[it[0]]->right=mp[it[1]];
            s.insert(it[1]);
        }
        for(auto it : des){
            if(s.find(it[0])==s.end()) return mp[it[0]];
        }
        return NULL;
    }
```

## 222. Count Complete Tree Nodes

**Q:** Given the root of a complete binary tree, return the number of nodes efficiently without a full traversal.

**A:** Logarithmic depth traversal leveraging the full structure of complete binary trees to math out perfect subtrees.

**Explanation:**
- The algorithm calculates the depth of the leftmost and rightmost paths originating from the current node.
- If these left and right heights match, it guarantees the subtree is perfect, and its node count is computed algebraically via bit shifts `(1 << height) - 1`.
- If the heights misalign, it defaults to standard recursive traversal for children until perfectly structured subtrees are encountered lower down.

**Complexity:** Time: `O((log n)^2)` because navigating to depth takes `O(log n)` and is done `O(log n)` times. Space: `O(log n)` for the recursion stack.

```cpp
    int lh(TreeNode* root){
        int h=0;
        while(root){
            h++;
            root=root->left;
        }
        return h;
    }
    int rh(TreeNode* root){
        int h=0;
        while(root){
            h++;
            root=root->right;
        }
        return h;
    }
    int countNodes(TreeNode* root) {
        if(!root) return 0;
        int l=lh(root);
        int r=rh(root);
        if(l==r) return (1<<l)-1;
        return 1+countNodes(root->left)+countNodes(root->right);
    }
```

## 236. Lowest Common Ancestor of a Binary Tree

**Q:** Given the root of a binary tree and two nodes `p` and `q`, find their lowest common ancestor (LCA).

**A:** Post-order DFS returning the matched node pointer upwards; intersection marks the LCA.

**Explanation:**
- The recursive function acts as a boolean signal (represented via pointer): it returns the node itself if it matches `p` or `q`, or null otherwise.
- In the post-order phase, if both left and right recursive calls return non-null, the current node sits at the divergence and is hence the LCA.
- If only one side returns non-null, that subtree contains both target nodes (or just one target, with the other deeper), so the non-null pointer is passed straight upwards.

**Complexity:** Time: `O(n)` to visit nodes, stopping traversal paths early if descendants are irrelevant. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if(!root) return root;
        if(root==p || root==q) return root;
        TreeNode* leftN=lowestCommonAncestor(root->left,p,q);
        TreeNode* rightN=lowestCommonAncestor(root->right,p,q);
        if(leftN && rightN) return root;
        if(!leftN && !rightN) return NULL;
        if(leftN) return leftN;
        return rightN;
    }
```

## 2385. Amount of Time for Binary Tree to Be Infected

**Q:** Given the root of a binary tree and a starting node value, determine how many minutes (levels) it takes for an infection to spread to the entire tree.

**A:** Graph conversion via DFS to build parent pointers, followed by an undirected BFS from the starting node mapping infection time.

**Explanation:**
- A DFS traverses the tree building a child-to-parent mapping (`mp`) to enable traveling upwards, simultaneously capturing a pointer to the target `start` node.
- An iterative BFS simulates the infection spread using a queue (initialized with the `start` node) and a `vi` set tracking infected nodes.
- At each step, infection spreads to unvisited left children, right children, and the mapped parent, incrementing an answer counter `ans` upon level completion.

**Complexity:** Time: `O(n)` for DFS mapping and `O(n)` for BFS traversal. Space: `O(n)` for the parent hash map, visited set, and queue.

```cpp
    void traverse(TreeNode *r,int s,TreeNode **st,map<TreeNode*,TreeNode*>&mp){
        if(!r) return;
        if(r->left) mp[r->left]=r;
        if(r->right) mp[r->right]=r;
        if(r->val == s) *st=r;
        traverse(r->left,s,st,mp);
        traverse(r->right,s,st,mp);
    }
    int amountOfTime(TreeNode* root, int start) {
        map<TreeNode*,TreeNode*> mp;
        TreeNode* st=NULL;
        unordered_set<int> vi;
        traverse(root,start,&st,mp);
        if(!st) return -1;
        queue<TreeNode*> q;
        q.push(st);
        vi.insert(st->val);
        int ans=0;
        while(!q.empty()){
            int len=q.size();
            for(int i=0;i<len;i++){
                TreeNode *tmp=q.front();q.pop();
                if(tmp->left && vi.find(tmp->left->val)==vi.end()) {
                    q.push(tmp->left);
                    vi.insert(tmp->left->val);
                }
                if(tmp->right && vi.find(tmp->right->val)==vi.end()) {
                    q.push(tmp->right);
                    vi.insert(tmp->right->val);
                }
                if(mp.find(tmp)!=mp.end() && vi.find(mp[tmp]->val)==vi.end()) {
                    q.push(mp[tmp]);
                    vi.insert(mp[tmp]->val);
                }
            }
            ans++;
        }
        return ans-1;
    }
```

## 297. Serialize and Deserialize Binary Tree

**Q:** Design an algorithm to serialize a binary tree to a string representation and deserialize it back into a full tree structure.

**A:** Iterative level-order BFS generating and parsing a comma-separated string with '#' marking null leaves.

**Explanation:**
- **Serialize:** A queue handles a standard BFS. Null pointers output "#,"; valid nodes output their value alongside pushing left and right children to process sequentially.
- **Deserialize:** A stringstream splits the encoded string. The queue reconstructs the tree level-by-level, pulling pairs of comma-separated chunks for left and right children, ignoring linking if "#" is found.
- The base case for both routines guards against empty or missing root inputs.

**Complexity:** Serialize: Time `O(n)`, Space `O(w)` queue and `O(n)` string construction. Deserialize: Time `O(n)`, Space `O(w)` queue and `O(n)` stringstream memory.

```cpp
    // Encodes a tree to a single string.
    string serialize(TreeNode* root) {
        string ans="";
        if(!root) return ans;
        queue<TreeNode*> q;
        q.push(root);
        while(!q.empty()){
            TreeNode* t=q.front();q.pop();
            if(!t) ans.append("#,");
            else{
                ans.append(to_string(t->val)+',');
                q.push(t->left);
                q.push(t->right);
            }
        }
        return ans;
    }

    // Decodes your encoded data to tree.
    TreeNode* deserialize(string data) {
        if(data.size()==0) return NULL;
        stringstream s(data);
        string str;
        getline(s,str,',');
        TreeNode *root=new TreeNode(stoi(str));
        queue<TreeNode*> q;
        q.push(root);
        while(!q.empty()){
            TreeNode *node=q.front();q.pop();
            getline(s,str,',');
            if(str=="#"){
                node->left=NULL;
            }
            else{
                TreeNode *leftNode = new TreeNode(stoi(str));
                node->left=leftNode;
                q.push(leftNode);
            }
            
            getline(s,str,',');
            if(str=="#"){
                node->right=NULL;
            }
            else{
                TreeNode *rightNode = new TreeNode(stoi(str));
                node->right=rightNode;
                q.push(rightNode);
            }
        }
        return root;
    }
```

## 404. Sum of Left Leaves

**Q:** Given the root of a binary tree, return the sum of all left leaves.

**A:** Pre-order DFS navigating children while passing a boolean/integer flag denoting whether the edge taken was a left connection.

**Explanation:**
- The recursive function incorporates a parameter `isL` (`1` if coming from a left child edge, `0` if right edge).
- When a node is identified strictly as a leaf (`!root->left && !root->right`), it adds its value to the global/reference `ans` only if `isL` is true.
- Traversal branches update this state accordingly (`1` passed left, `0` passed right).

**Complexity:** Time: `O(n)` as every node is visited exactly once. Space: `O(h)` for the recursion stack, `O(log n)` balanced, `O(n)` skewed.

```cpp
  void solve(TreeNode*root,int &ans,int isL){
      if(!root) return;
      if(!root->left && !root->right && isL) ans+=root->val;
      if(root->left) solve(root->left,ans,1);
      if(root->right) solve(root->right,ans,0);
  }
  int sumOfLeftLeaves(TreeNode* root) {
      int ans=0;
      solve(root,ans,0);
      return ans;
  }
```

## 429. N-ary Tree Level Order Traversal

**Q:** Given an N-ary tree root, return the level order traversal of its nodes' values as a list of lists.

**A:** Iterative level-order BFS using a queue to group nodes uniformly across N branches.

**Explanation:**
- Exactly like binary tree level-order traversal, a queue is initialized with the root.
- A bounded loop processes all items currently enqueued at that specific level length (`size`).
- Instead of manually checking left and right children, an internal loop iterates through the node's `children` array and pushes valid branches onto the queue.

**Complexity:** Time: `O(n)` visiting every node strictly once. Space: `O(w)` where `w` is the max tree width (potentially near `n` for shallow, highly-branched trees).

```cpp
    vector<vector<int>> levelOrder(Node* root) {
        vector<vector<int>> ans;
        if(!root) return ans;
        queue<Node*> q;
        q.push(root);
        while(!q.empty()){
            int size=q.size();
            vector<int> temp;
            for(int i=0;i<size;i++){
                Node *it=q.front();q.pop();
                temp.push_back(it->val);
                for(int j=0;j < it->children.size();j++){
                    if(it->children[j]){
                        q.push(it->children[j]);
                    }
                }
            }
            ans.push_back(temp);
        }
        return ans;
    }
```

## 437. Path Sum III

**Q:** Given the root of a binary tree and an integer `targetSum`, return the number of valid downward paths (not necessarily starting at root or ending at leaf) summing to `targetSum`.

**A:** Pre-order DFS combining running path sums with a hash map frequency counter (prefix sum technique).

**Explanation:**
- As traversal runs downward, it accumulates a running sum; a match implies `sum - target` has been seen previously in this specific lineage.
- The hash map (`mp`) tracks the frequency of prefix sums encountered on the path back to the root (initialized with `0` occurring `1` time).
- Backtracking is strictly enforced (`mp[sum]--`) after subtree recursive calls to prevent separate branches from interpreting each other's prefix sums.

**Complexity:** Time: `O(n)` reading each node once assuming `O(1)` map interactions. Space: `O(h)` for the recursion stack and the hash map elements limited strictly to current path length.

```cpp
    void solve(TreeNode* root,long long sum,int t,unordered_map<long long,int> &mp,int &ans){
        if(!root) return;
        sum+=root->val;
        ans+=mp[sum-t];
        mp[sum]++;
        solve(root->left,sum,t,mp,ans);
        solve(root->right,sum,t,mp,ans);
        mp[sum]--;
    }

    int pathSum(TreeNode* root, int targetSum) {
        int ans=0;
        if(!root) ans;
        unordered_map<long long,int> mp;
        mp[0]++;
        solve(root,0,targetSum,mp,ans);
        return ans;
    }
```

## 513. Find Bottom Left Tree Value

**Q:** Given the root of a binary tree, return the value of the leftmost node located in the last row of the tree.

**A:** Iterative level-order BFS tracking the very first node observed during each level jump.

**Explanation:**
- Standard queue-based BFS extracts and counts nodes per level length (`n`).
- The `if(i == 0)` check explicitly targets the first node in a level (leftmost).
- Because traversal goes downwards level by level, `ans` constantly overwrites until the final layer finishes, leaving it permanently pinned on the bottom leftmost value.

**Complexity:** Time: `O(n)` fully analyzing every node. Space: `O(w)` where `w` is maximum tree width held within the queue.

```cpp
    int findBottomLeftValue(TreeNode* root) {
        queue<TreeNode*> q;
        q.push(root);
        int ans=-1;
        while(!q.empty()){
            int n=q.size();
            for(int i=0;i<n;i++){
                auto it = q.front();q.pop();
                if(i==0) ans=it->val;
                if(it->left) q.push(it->left);
                if(it->right) q.push(it->right);
            }
        }
        return ans;
    }
```

## 543. Diameter of Binary Tree

**Q:** Given the root of a binary tree, find the length of the diameter (longest path between any two nodes).

**A:** Post-order DFS computing left and right subtree heights while concurrently tracking the maximal path linkage (diameter).

**Explanation:**
- The optimized technique mirrors maximum depth traversal but introduces a reference `ans` maintaining the biggest sum of left and right heights.
- It calculates node heights natively (`1 + max(l, r)`) and passes them upward.
- The `ans` variable is evaluated at every intersection `max(ans, l + r)` evaluating paths crossing through the current root.

**Complexity:** Brute Force: Time `O(n^2)` recalculating heights continuously, Space `O(h)`. Optimized: Time `O(n)` observing each node once, Space `O(h)` for the recursion stack (`O(log n)` balanced, `O(n)` skewed).

```cpp
//BROUTE FORCE...O(n^2)
    int height(TreeNode* root){
        if(!root) return 0;
        int l=height(root->left); 
        int r=height(root->right);
        return 1+max(l,r);
    }
    int ans=0;
    int diameterOfBinaryTree(TreeNode* root) {
        if(!root) return 0;
        int l=height(root->left); 
        int r=height(root->right);
        ans=max(ans,l+r);
        diameterOfBinaryTree(root->left);
        diameterOfBinaryTree(root->right);
        return ans;
    }
    
//OPTIMIZE...O(n)
    int height(TreeNode* root,int &ans){
        if(!root) return 0;
        int l=height(root->left,ans); 
        int r=height(root->right,ans);
        ans=max(ans,l+r);
        return 1+max(l,r);
    }
    
    int diameterOfBinaryTree(TreeNode* root) {
        int ans=0;
        height(root,ans);
        return ans;
    }
```
## 623. Add One Row to Tree

**Q:** Given the root of a binary tree, an integer `val`, and an integer `depth`, add a row of nodes with value `val` at the given depth.

**A:** Level-order BFS to reach the layer just above the target depth, then insert new nodes.

**Explanation:**
- BFS is used to traverse the tree layer by layer, decrementing `depth` until we reach `depth - 1`.
- When the target parent level is reached, for each node in the queue, we replace its left and right children with new nodes containing `val`, and attach the original children to the new nodes.
- A special base case handles `depth == 1` by creating a new root with the original root as its left child.

**Complexity:** Time `O(n)` to traverse down to the depth, Space `O(w)` where `w` is the maximum width of the tree at the target level.

```cpp
    TreeNode* addOneRow(TreeNode* root, int val, int depth) {
        if(depth==1) return new TreeNode(val,root,NULL);
        queue<TreeNode*> q;
        q.push(root);
        depth-=2;
        while(!q.empty() && depth){
            int len=q.size();
            for(int i=0;i<len;i++){
                TreeNode* tmp=q.front();q.pop();
                if(tmp->left) q.push(tmp->left);
                if(tmp->right) q.push(tmp->right);
            }
            depth--;
        }
        while(!q.empty()){
            TreeNode* tmp=q.front();q.pop();
            tmp->left=new TreeNode(val,tmp->left,NULL);
            tmp->right=new TreeNode(val,NULL,tmp->right);
        }
        return root;
    }
```

---

## 637. Average of Levels in Binary Tree

**Q:** Given the root of a binary tree, return the average value of the nodes on each level in the form of an array.

**A:** Level-order BFS aggregating sums per level.

**Explanation:**
- BFS explores the tree level by level, maintaining a queue of nodes for the current level.
- For each level, iterates through all its nodes, summing their values and capturing the size.
- The average is computed and pushed to the result array before progressing to the next level's children.

**Complexity:** Time `O(n)` to visit all nodes, Space `O(w)` for the queue where `w` is the max width of the tree.

```cpp
    vector<double> averageOfLevels(TreeNode* root) {
        vector<double> ans;
        queue<TreeNode*> q;
        q.push(root);
        while(!q.empty()){
            int len=q.size();
            long long int sum=0;
            for(int i=0;i<len;i++){
                TreeNode* it=q.front();q.pop();
                if(it->left) q.push(it->left);
                if(it->right) q.push(it->right);
                sum+=it->val;
            }
            ans.push_back((double)sum/double(len));
        }
        return ans;
    }
```

---

## 662. Maximum Width of Binary Tree

**Q:** Given the root of a binary tree, return the maximum width among all levels, counting null nodes between valid nodes.

**A:** Level-order BFS using a queue of node-to-index pairs to calculate width.

**Explanation:**
- BFS is used alongside zero-indexed positions; for a node at index `i`, its children are at `2*i + 1` and `2*i + 2`.
- To prevent integer overflow, the index relative to the level's minimum index (`id - minId`) is stored.
- At each level, the width is evaluated as the difference between the maximum and minimum scaled indices plus one.

**Complexity:** Time `O(n)` to visit every node, Space `O(w)` to store nodes of the widest level.

```cpp
    int widthOfBinaryTree(TreeNode* root) {
        if(!root) return 0;
        long long int ans=0;
        queue<pair<TreeNode*,long long int>> q;
        q.push({root,0});
        while(!q.empty()){
            int size=q.size();
            long long int minId=q.front().second;
            long long int first,last;
            for(int i=0;i<size;i++){
                long long int id=q.front().second-minId;
                TreeNode* node=q.front().first;
                q.pop();
                if(i==0) first=id;
                if(i==size-1) last=id;
                if(node->left) q.push({node->left,id*2+1});
                if(node->right) q.push({node->right,id*2+2});
            }
            
            ans=max(ans,last-first+1);
        }
        return ans;
    }
```

---

## 814. Binary Tree Pruning

**Q:** Given the root of a binary tree where every node's value is either 0 or 1, return the same tree where every subtree not containing a 1 has been removed.

**A:** Post-order DFS traversal resolving whether subtrees contain any 1s.

**Explanation:**
- Post-order traversal guarantees children are evaluated before deciding to prune the current parent.
- Each recursive call evaluates if the left or right subtrees contain a 1; if not, the respective child pointer is severed (`NULL`).
- Returns true if the node itself is 1 or either of its subtrees contains a 1.

**Complexity:** Time `O(n)` to visit all nodes, Space `O(h)` for the recursion stack depth.

```cpp
    bool containOne(TreeNode *root){
        if(!root) return false;
        bool leftOne=containOne(root->left);
        bool rightOne=containOne(root->right);
        
        if(!leftOne) root->left=NULL;
        if(!rightOne) root->right=NULL;
        return (root->val==1 || leftOne || rightOne);
    }
    
    TreeNode* pruneTree(TreeNode* root) {
        if(containOne(root)) return root;
        return NULL;
    }
```

---

## 863. All Nodes Distance K in Binary Tree

**Q:** Given the root of a binary tree, a target node, and an integer `k`, return an array of the values of all nodes that have a distance `k` from the target node.

**A:** Two-pass graph approach: map parents via BFS, then perform BFS outwards from the target.

**Explanation:**
- The first BFS traverses the tree downwards to build a hash map mapping each node to its parent, treating the tree like an undirected graph.
- The second BFS initiates at the target node, exploring left, right, and parent neighbors layer by layer.
- A visited map ensures we don't traverse back; stops when distance reaches `k` and collects the queue contents.

**Complexity:** Time `O(n)` for traversing the tree twice, Space `O(n)` to store parent references, visited states, and the BFS queue.

```cpp
    void makeParent(TreeNode* root,unordered_map<TreeNode*,TreeNode*> &parent){
        queue<TreeNode*> q;
        q.push(root);
        while(!q.empty()){
            TreeNode *t=q.front();q.pop();
            if(t->left){
                q.push(t->left);
                parent[t->left]=t;
            }
            if(t->right){
                q.push(t->right);
                parent[t->right]=t;
            }
        }
    }
    
    vector<int> distanceK(TreeNode* root, TreeNode* target, int k) {
        vector<int> ans;
        if(!root || !target) return ans;
        unordered_map<TreeNode*,TreeNode*> parent;
        makeParent(root,parent);
        
        unordered_map<TreeNode*,bool> vis;
        queue<TreeNode*> q;
        q.push(target);
        vis[target]=true;
        int dis=1;
        while(!q.empty() && dis<=k){
            int size=q.size();
            for(int i=0;i<size;i++){
                TreeNode* t=q.front();q.pop();
                
                if(t->left && !vis[t->left]){
                    q.push(t->left);
                    vis[t->left]=true;
                }
                if(t->right && !vis[t->right]){
                    q.push(t->right);
                    vis[t->right]=true;
                }
                if(parent[t] && !vis[parent[t]]){
                    q.push(parent[t]);
                    vis[parent[t]]=true;
                }
            }
            dis++;
        }
        while(!q.empty()){
            ans.push_back(q.front()->val);
            q.pop();
        }
        return ans;
    }
```

---

## 94. Binary Tree Inorder Traversal

**Q:** Given the root of a binary tree, return the inorder traversal of its nodes' values.

**A:** Three distinct approaches: Recursive, Iterative with a stack, and Morris traversal.

**Explanation:**
- The recursive approach naturally visits left, pushes the value, then right.
- The iterative approach uses an explicit stack, pushing left children until null, then processing the node and moving right.
- The Morris traversal avoids a stack by temporarily linking a node's inorder predecessor's right pointer to the node itself, reverting it upon second visit.

**Complexity:** Recursive/Iterative: Time `O(n)`, Space `O(h)` for the stack.
Morris: Time `O(n)`, Space `O(1)` aside from the result array.

```cpp
//Recursive...
    vector<int> ans;
    vector<int> inorderTraversal(TreeNode* root) {
        if(!root) return ans;
        inorderTraversal(root->left);
        ans.push_back(root->val);
        inorderTraversal(root->right);
        return ans;
    }
    
//Iterative...
    vector<int> inorderTraversal(TreeNode* root) {
        vector<int> ans;
        stack<TreeNode*> st;
        TreeNode *cur=root;
        while(!st.empty() || cur){
            if(cur){
                st.push(cur);
                cur=cur->left;
            }else{
                TreeNode *t=st.top();st.pop();
                ans.push_back(t->val);
                cur=t->right;
            }
        }
        return ans;
    }
    
//Morris.....   
    vector<int> inorderTraversal(TreeNode* root){
        vector<int> ans;
        TreeNode *cur=root;
        while(cur){
            if(!cur->left){
                ans.push_back(cur->val);
                cur=cur->right;
            }else{
                TreeNode *prev=cur->left;
                while(prev->right && prev->right!=cur){
                    prev=prev->right;
                }
                if(!prev->right){
                    prev->right=cur;
                    cur=cur->left;
                }else{
                    prev->right=NULL;
                    ans.push_back(cur->val);
                    cur=cur->right;
                }
            }
        }
        return ans;
    }
```

---

## 968. Binary Tree Cameras

**Q:** Given the root of a binary tree, return the minimum number of cameras needed to monitor all nodes (a camera monitors its parent, itself, and its children).

**A:** Post-order DFS with state signaling (need camera, covered, has camera).

**Explanation:**
- Uses post-order DFS to process leaves first, propagating three states up: 0 (has camera), 1 (covered), -1 (needs camera).
- If either child needs a camera (`-1`), the current node must host a camera, incrementing the global count and returning `0`.
- If either child has a camera (`0`), the current node is covered (`1`); otherwise, it needs a camera (`-1`).
- A final check on the root ensures that if the root is left needing a camera, one is added.

**Complexity:** Time `O(n)` to traverse all nodes, Space `O(h)` for the recursion stack.

```cpp
// 0 - camera, 1 - coverd, -1 - need camera
    int cam=0;
    int solve(TreeNode *root){
        if(!root) return 1;
        int left=solve(root->left);
        int right=solve(root->right);
        if(left==-1 || right==-1){
            cam++;
            return 0;
        }else if(left==0 || right==0){
            return 1;
        }
        else return -1;
    }
    
    int minCameraCover(TreeNode* root) {
        if(!root) return 0;
        if(!root->left && !root->right) return 1;
        int ans=solve(root);
        if(ans==-1) cam++;
        return cam;
    }
```

---

## 979. Distribute Coins in Binary Tree

**Q:** Given the root of a binary tree with `n` nodes and `n` coins, return the minimum number of moves required to make every node have exactly one coin.

**A:** Post-order DFS returning the net coin balance of subtrees.

**Explanation:**
- Post-order DFS determines the excess or deficit of coins in the left and right subtrees.
- Each node needs exactly 1 coin; its balance is pushed to its parent (`root->val - 1`).
- The total moves are accumulated globally by adding the absolute value of the coin flow moving through the current node.

**Complexity:** Time `O(n)` for a single traversal, Space `O(h)` for the recursion stack depth.

```cpp
    int distributeCoins(TreeNode* root,TreeNode* parent=NULL) {
        if(!root) return 0;
        int move = distributeCoins(root->left,root)+distributeCoins(root->right,root);
        int x=root->val-1;
        if(parent) parent->val+=x;
        move+=abs(x);
        return move;
    }
```

---

## 987. Vertical Order Traversal of a Binary Tree

**Q:** Given the root of a binary tree, return the vertical order traversal of its nodes' values, sorting by row and then value for nodes at the same position.

**A:** Level-order BFS maintaining horizontal and vertical coordinates using a nested map structure.

**Explanation:**
- Uses BFS with a queue holding the node along with its `(x, y)` coordinates, where `x` is the column and `y` is the depth.
- Nodes are inserted into a `map<int, map<int, multiset<int>>>` which automatically groups and sorts by `x` (column), then `y` (row), and finally value.
- After traversal, the map is iterated to flatten the grouped multisets into the final 2D result array.

**Complexity:** Time `O(n log n)` due to multiset and map insertions, Space `O(n)` to store node coordinates and the nested maps.

```cpp
  vector<vector<int>> verticalTraversal(TreeNode* root) {
        vector<vector<int>> ans;
        if(!root) return ans;
        map<int,map<int,multiset<int>>> mp;
        queue<pair<TreeNode*,pair<int,int>>> q;
        q.push({root,{0,0}});
        while(!q.empty()){
            auto p=q.front();q.pop();
            TreeNode* n=p.first;
            int x=p.second.first, y=p.second.second;
            mp[x][y].insert(n->val);
            if(n->left) q.push({n->left,{x-1,y+1}});
            if(n->right) q.push({n->right,{x+1,y+1}});
        }
        for(auto it : mp){
            vector<int> t;
            for(auto e : it.second){
                t.insert(t.end(),e.second.begin(),e.second.end());
            }
            ans.push_back(t);
        }
        return ans;
    }
```

---

## 1. Binary Tree to DLL

**Q:** Given a Binary Tree, convert it to a Doubly Linked List in-place following inorder traversal, returning the head of the DLL.

**A:** Inorder DFS maintaining a reference to the previously visited node to build bi-directional links.

**Explanation:**
- Recursively performs standard inorder traversal (left, node, right).
- Maintains a reference parameter `prev`; for the first processed node, it is captured as the `head`.
- For subsequent nodes, it links `prev->right` to the current node and `root->left` to `prev`, then updates `prev` to the current node.

**Complexity:** Time `O(n)` to visit each node exactly once, Space `O(h)` for the recursion stack.

```cpp
    void solve(Node* root,Node* &head,Node* &prev,int &f){
        if(!root) return;
        solve(root->left,head,prev,f);
        if(f==0){
            head=root;
            prev=root;
            f=1;
        }else{
            prev->right=root;
            root->left=prev;
            prev=root;
        }
        solve(root->right,head,prev,f);
    }
    Node * bToDLL(Node *root){
        // your code here
        if(!root) return root;
        Node *head=NULL,*prev=NULL;
        int f=0;
        solve(root,head,prev,f);
        return head;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/binary-tree-to-dll/1
```

---

## 2. Bottom View of Binary Tree

**Q:** Given a binary tree, return the bottom view from left to right (the last node seen at each horizontal distance).

**A:** Level-order BFS using a map to overwrite values at each horizontal distance.

**Explanation:**
- BFS traverses the tree, assigning horizontal distances (root is 0, left is `h-1`, right is `h+1`).
- A `map<int, int>` stores the horizontal distance as the key. Because it is level-order, later nodes naturally overwrite earlier nodes at the same horizontal distance.
- Iterating the ordered map yields the bottom-most nodes from leftmost to rightmost columns.

**Complexity:** Time `O(n log n)` due to map operations, Space `O(n)` for the map and BFS queue.

```cpp
    vector <int> bottomView(Node *root) {
        vector<int> ans;
        if(!root) return ans;
        map<int,int> m;
        queue<pair<Node*,int>> q;
        q.push({root,0});
        while(!q.empty()){
            Node* t=q.front().first;
            int h=q.front().second;
            q.pop();
            m[h]=t->data;
            if(t->left) q.push({t->left,h-1});
            if(t->right) q.push({t->right,h+1});
        }
        
        for(auto it=m.begin();it!=m.end();it++){
            ans.push_back(it->second);
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/bottom-view-of-binary-tree/1
```

---

## 3. Boundary Traversal of binary tree

**Q:** Given a binary tree, return the values of its boundary nodes starting from the root, going down the left boundary, then leaves from left to right, and up the right boundary.

**A:** Three-part segmented DFS extracting left boundary, all leaves, and reversed right boundary.

**Explanation:**
- `leftB` traverses the left boundary (preferring left child, falling back to right) excluding leaves, pushing values on the way down.
- `leaf` performs a standard DFS specifically to capture all leaf nodes in left-to-right order.
- `rightB` traverses the right boundary (preferring right child, falling back to left) excluding leaves, pushing values on the way up (post-order).

**Complexity:** Time `O(n)` to visit nodes across the segmented passes, Space `O(h)` for the recursion stack.

```cpp
  void leftB(Node* root, vector<int> &res){
        if(!root || (!root->left && !root->right)) return;
        res.push_back(root->data);
        if(root->left) leftB(root->left,res);
        else leftB(root->right,res);
    }
    
    void leaf(Node* root, vector<int> &res){
        if(!root) return;
        if(!root->left && !root->right) res.push_back(root->data);
        leaf(root->left,res);
        leaf(root->right,res);
    }
    
    void rightB(Node* root, vector<int> &res){
        if(!root || (!root->left && !root->right)) return;
        if(root->right) rightB(root->right,res);
        else rightB(root->left,res);
        res.push_back(root->data);
    }
    
    vector<int> boundary(Node *root){
        vector<int> res;
        if(!root) return res;
        res.push_back(root->data);
        leftB(root->left,res);
        leaf(root->left,res);
        leaf(root->right,res);
        rightB(root->right,res);
        return res;
    }

Link - https://practice.geeksforgeeks.org/problems/boundary-traversal-of-binary-tree/1/?page=2&category[]=Tree&curated[]=1&curated[]=7&sortBy=submissions
```

---

## 4. Burning Tree

**Q:** Given a binary tree and a target node value, return the minimum time required to burn the entire tree starting from the target node.

**A:** Two-pass BFS: map parents to treat the tree as an undirected graph, then run multi-source BFS from the target.

**Explanation:**
- The first pass is a BFS to find the target node pointer and populate a map linking every node to its parent.
- The second pass is a level-order BFS starting from the target node, spreading to the left child, right child, and parent.
- A visited map prevents revisiting burnt nodes, and the depth of the BFS minus one represents the total burn time.

**Complexity:** Time `O(n)` for parent mapping and burning BFS, Space `O(n)` for the parent map, visited map, and queue.

```cpp
    Node* makeParent(Node* root,unordered_map<Node*,Node*> &parent,int target){
        Node* tar=NULL;
        queue<Node*> q;
        q.push(root);
        while(!q.empty()){
            Node *t=q.front();q.pop();
            if(t->data==target) tar=t;
            if(t->left){
                q.push(t->left);
                parent[t->left]=t;
            }
            if(t->right){
                q.push(t->right);
                parent[t->right]=t;
            }
        }
        return tar;
    }
    
    int minTime(Node* root, int target){
        if(!root) return 0;
        unordered_map<Node*,Node*> parent;
        Node* targetNode=makeParent(root,parent,target);
        
        unordered_map<Node*,bool> vis;
        queue<Node*> q;
        q.push(targetNode);
        int dis=0;
        while(!q.empty()){
            int size=q.size();
            for(int i=0;i<size;i++){
                Node* t=q.front();q.pop();
                vis[t]=true;
                if(t->left && !vis[t->left]){
                    q.push(t->left);
                }
                if(t->right && !vis[t->right]){
                    q.push(t->right);
                }
                if(parent[t] && !vis[parent[t]]){
                    q.push(parent[t]);
                }
            }
            dis++;
        }
        return dis-1;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/burning-tree/1
```

---

## 5. Children Sum Property

**Q:** Given a binary tree, modify it in-place such that every node's value equals the sum of its children's values, without decrementing any node's value.

**A:** Top-down aggressive increment followed by bottom-up strict sum re-calculation.

**Explanation:**
- During the top-down pass, if the children's sum is less than the node, the children's values are eagerly bumped up to match the node's value to prevent deficits.
- Then, it recursively calls itself on the left and right subtrees.
- During the bottom-up return, the parent updates its value to perfectly match the newly fixed sum of its children, ensuring the property holds everywhere.

**Complexity:** Time `O(n)` to visit every node, Space `O(h)` for the recursion stack depth.

```cpp
void changeTree(BinaryTreeNode < int > * root) {
    if(!root) return;
    int child=0;
    if(root->left) child+=root->left->data;
    if(root->right) child+=root->right->data;
    if(child>=root->data) root->data=child;
    else{
        if(root->left) root->left->data=root->data;
        if(root->right) root->right->data=root->data;
    }
    changeTree(root->left);
    changeTree(root->right);
    int tot=0;
    if(root->left) tot+=root->left->data;
    if(root->right) tot+=root->right->data;
    if(root->left || root->right) root->data=tot;
}  

Link - https://www.codingninjas.com/codestudio/problems/childrensumproperty_790723?topList=striver-sde-sheet-problems&utm_source=striver&utm_medium=website&leftPanelTab=0
```

---

## 6. Code Testcase Testcase Test Result 2331. Evaluate Boolean Binary Tree

**Q:** Given a full binary tree representing a boolean expression (0=False, 1=True, 2=OR, 3=AND), evaluate and return the boolean result.

**A:** Post-order DFS recursively evaluating left and right subtrees.

**Explanation:**
- The base case immediately returns the boolean value if the node is a leaf (`0` or `1`).
- For internal nodes, it evaluates both children recursively.
- It applies the logical OR (`||`) if the node value is `2`, or logical AND (`&&`) if the node value is `3`.

**Complexity:** Time `O(n)` to evaluate all nodes, Space `O(h)` for the recursion stack.

```cpp
    bool evaluateTree(TreeNode* root) {
        if(!root) return false;
        if(root->val==1 || root->val==0) return root->val;
        else if(root->val==2) return (evaluateTree(root->left) || evaluateTree(root->right));
        return (evaluateTree(root->left) && evaluateTree(root->right));
    }
```

---

## 7. Construct Binary Tree from Parent Array

**Q:** Given an array representing a tree where `parent[i]` is the parent of node `i`, construct the binary tree and return its root.

**A:** Hash map index-to-node creation followed by a wiring pass.

**Explanation:**
- First pass iterates the array to instantiate all nodes and store them in an `unordered_map` keyed by index.
- Second pass iterates again; if `parent[i] == -1`, it identifies the root.
- For other nodes, it retrieves the parent node from the map and wires the current node as its left child (if empty) or right child.

**Complexity:** Time `O(n)` for two linear passes, Space `O(n)` to store the node mapping.

```cpp
    Node *createTree(vector<int> parent) {
        int n=parent.size();
        unordered_map<int,Node*> mp;
        for(int i=0;i<n;i++){
            mp[i]=new Node(i);
        }
        Node *root=NULL;
        for(int i=0;i<n;i++){
            if(parent[i]==-1) root=mp[i];
            else{
                if(mp[parent[i]]->left){
                    mp[parent[i]]->right=mp[i];
                }else{
                    mp[parent[i]]->left=mp[i];
                }
            }
        }
        return root;
    }

gfg link - https://www.geeksforgeeks.org/problems/construct-binary-tree-from-parent-array/1
```

---

## 8. Count the nodes at distance K from leaf

**Q:** Given a binary tree and an integer `k`, find the number of unique nodes that are exactly at distance `k` from any leaf node.

**A:** Pre-order DFS with a downward distance-checking helper function.

**Explanation:**
- The main `count` function traverses every node in the tree.
- For each node, a helper `heightK` recursively checks if there is any leaf exactly `k` steps below it in its subtree.
- If a matching leaf is found, the current node is counted, and we proceed to check its children.

**Complexity:** Time `O(n^2)` in the worst skewed case because each node initiates a DFS, Space `O(h)` for the recursion stack.

```cpp
    bool heightK(Node* node, int k){
        if(!node){
            return false;
        }
        if(!node->left && !node->right){
            if(k==0) return true;
            return false;
        }
        if(heightK(node->left, k-1) || heightK(node->right, k-1)) return true;
        return false;
    }
    int count(Node* root, int k){
        if(!root) return 0; 
        if(heightK(root, k)) {
            return 1+count(root->left, k)+count(root->right, k);
        }
        return count(root->left, k)+count(root->right, k);
    }
    int printKDistantfromLeaf(Node* root, int k) {
    	//Add your code here.
    	return count(root, k);
    }

gfg link - https://www.geeksforgeeks.org/problems/node-at-distance/1
```

---

## 9. Diagonal sum in binary tree

**Q:** Given a binary tree, return an array containing the sum of all nodes lying on each diagonal line (from top-right to bottom-left).

**A:** Pre-order DFS tracking diagonal levels using a map.

**Explanation:**
- The algorithm defines diagonal levels: moving right stays on the same diagonal (`lev`), moving left shifts to the next diagonal (`lev + 1`).
- A `map<int, int>` accumulates the sum of node values for each diagonal level.
- Finally, the map is iterated in order to extract the diagonal sums into the result array.

**Complexity:** Time `O(n log n)` due to map insertions, Space `O(n)` for the map and recursion stack.

```cpp
    void diaSum(Node *root,int lev,map<int,int> &mp){
        if(!root) return;
        if(root->right){
            mp[lev]+=root->right->data;
            diaSum(root->right,lev,mp);
        }
        if(root->left){
            mp[lev+1]+=root->left->data;
            diaSum(root->left,lev+1,mp);
        }
    }
    
    vector<int> diagonalSum(Node* root) {
        vector<int> ans;
        if(!root) return ans;
        map<int,int> mp;
        mp[0]=root->data;
        diaSum(root,0,mp);
        for(auto it : mp) ans.push_back(it.second);
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/diagonal-sum-in-binary-tree/1
```

---

## 10. Left View of Binary Tree

**Q:** Given a binary tree, return an array of the values of the nodes visible when the tree is viewed from the left side.

**A:** Level-order BFS capturing the first node of each level.

**Explanation:**
- A queue is used to process the tree level by level.
- By tracking the size of the queue at the start of the level loop, we can identify the first node processed (`i == 0`).
- The value of this first node is pushed to the result array, and standard left-to-right child enqueuing continues.

**Complexity:** Time `O(n)` to traverse all nodes, Space `O(w)` where `w` is the maximum width of the tree.

```cpp
vector<int> leftView(Node *root){
    vector<int> ans;
    if(!root) return ans;
    queue<Node*> q;
    q.push(root);
    while(!q.empty()){
        int n=q.size();
        for(int i=0;i<n;i++){
            Node* t=q.front();
            q.pop();
            if(i==0) ans.push_back(t->data);
            if(t->left) q.push(t->left);
            if(t->right) q.push(t->right);
        }
    }
    return ans;
}

Link - https://practice.geeksforgeeks.org/problems/left-view-of-binary-tree/1#
```

---

## 11. Make Binary Tree From Linked List

**Q:** Given a singly linked list representing the level-order traversal of a binary tree, construct the binary tree and return its root.

**A:** Level-order BFS node construction matching list traversal.

**Explanation:**
- A queue holds newly created tree nodes that need their children populated.
- The linked list is iterated sequentially; for the front node in the queue, the next list value is attached as the left child and enqueued.
- If more list nodes remain, the subsequent list value is attached as the right child and enqueued, matching the level-order relationship.

**Complexity:** Time `O(n)` to consume the linked list and construct nodes, Space `O(n)` for the BFS queue.

```cpp
void convert(Node *head, TreeNode *&root) {
    root=new TreeNode(head->data);
    head=head->next;
    queue<TreeNode*> q;
    q.push(root);
    while(head){
        TreeNode* cur=q.front();q.pop();
        cur->left=new TreeNode(head->data);
        q.push(cur->left);
        head=head->next;
        if(head){
            cur->right=new TreeNode(head->data);
            q.push(cur->right);
            head=head->next;
        }
    }
}

gfg link - https://www.geeksforgeeks.org/problems/make-binary-tree/1
```

---

## 12. Nodes at given distance in binary tree

**Q:** Given a binary tree, a target value, and an integer `k`, return a sorted array of values of all nodes exactly distance `k` from the target node.

**A:** Two-pass graph approach: map parents while searching for the target, then multi-directional BFS.

**Explanation:**
- The first BFS finds the target node by value and simultaneously builds an `unordered_map` linking nodes to their parents.
- The second BFS starts from the found target, expanding outwards to left children, right children, and parents.
- A visited map prevents backward traversal, and once the distance threshold `k` is hit, the queue contains the answer which is then sorted.

**Complexity:** Time `O(n log n)` primarily dominated by the final sort operation, Space `O(n)` for maps and queues.

```cpp
    Node* makeParent(Node* root,unordered_map<Node*,Node*> &parent,int tar){
        if(!root) return root;
        Node* node=NULL;
        queue<Node*> q;
        q.push(root);
        while(!q.empty()){
            Node* it=q.front();q.pop();
            if(it->data==tar) node=it;
            if(it->left){
                parent[it->left]=it;
                q.push(it->left);
            }
            if(it->right){
                parent[it->right]=it;
                q.push(it->right);
            }
        }
        return node;
    }
    
    vector <int> KDistanceNodes(Node* root, int target , int k){
        vector<int> ans;
        if(!root) return ans;
        unordered_map<Node*,Node*> parent;
        Node* node=makeParent(root,parent,target);
        if(!node) return ans;
        
        unordered_map<Node*,bool> vis;
        queue<Node*> q;
        
        q.push(node);
        vis[node]=true;
        int dis=1;
        
        while(!q.empty() && dis<=k){
            int size=q.size();
            for(int i=0;i<size;i++){
                Node* ptr=q.front();q.pop();
                if(ptr->left && !vis[ptr->left]){
                    q.push(ptr->left);
                    vis[ptr->left]=true;
                }
                if(ptr->right && !vis[ptr->right]){
                    q.push(ptr->right);
                    vis[ptr->right]=true;
                }
                if(parent[ptr] && !vis[parent[ptr]]){
                    q.push(parent[ptr]);
                    vis[parent[ptr]]=true;
                }
            }
            dis++;
        }
        while(!q.empty()){
            ans.push_back(q.front()->data);
            q.pop();
        }
        sort(ans.begin(),ans.end());
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/nodes-at-given-distance-in-binary-tree/1#
```

---

## 13. Paths from root with a specified sum

**Q:** Given a binary tree and a target sum, return all root-to-leaf paths where the sum of the node values equals the target sum.

**A:** Pre-order DFS backtracking to accumulate paths and sum dynamically.

**Explanation:**
- DFS subtracts the current node's value from the running sum and pushes the value to a temporary path vector.
- If the running sum hits zero, the current path is added to the results array.
- After traversing both children, the current node is popped from the temporary path to backtrack.

**Complexity:** Time `O(n)` to visit all nodes, Space `O(h)` for the recursion stack and temporary path buffer.

```cpp
    void solve(Node *r,int s, vector<int> &t, vector<vector<int>> &a){
        if(!r) return;
        s-=r->key;
        t.push_back(r->key);
        if(s==0) a.push_back(t);
        solve(r->left,s,t,a);
        solve(r->right,s,t,a);
        t.pop_back();
    
    }
    
    vector<vector<int>> printPaths(Node *root, int sum) {
        vector<vector<int>> ans;
        vector<int> temp;
        solve(root,sum,temp,ans);
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/paths-from-root-with-a-specified-sum/1
```

---

## 14. Path to Given Node

**Q:** Given the root of a binary tree and an integer `B`, return the path (as an array of values) from the root to the node with value `B`.

**A:** Pre-order DFS with backtracking to find the target node.

**Explanation:**
- The function recursively explores the tree, pushing each visited node's value into the answer vector.
- If the current node matches `B`, it returns true, halting further exploration.
- If a subtree returns true, the success propagates up. If false, the node is popped from the vector to backtrack.

**Complexity:** Time `O(n)` to search the tree, Space `O(h)` for the recursion stack and path array.

```cpp
bool help(TreeNode* A, int B,vector<int> &ans){
    if(!A) return false;
    ans.push_back(A->val);
    if(A->val==B) return true;
    if(help(A->left,B,ans) || help(A->right,B,ans)) return true;
    ans.pop_back();
    return false;
}
vector<int> Solution::solve(TreeNode* A, int B) {
    vector<int> ans;
    if(!A) return ans;
    help(A,B,ans);
    return ans;
}

Link - https://www.interviewbit.com/problems/path-to-given-node/
```

---

## 15. Root to Leaf Paths

**Q:** Given a binary tree, return all paths from the root to every leaf node.

**A:** Pre-order DFS with backtracking to collect leaf paths.

**Explanation:**
- The traversal adds the current node's data to a temporary path vector.
- When a node is identified as a leaf (no left or right children), the collected path is pushed to the global result array.
- The path is popped before returning to backtrack and explore other branches.

**Complexity:** Time `O(n)` to visit all nodes, Space `O(h)` for recursion depth and the temporary vector.

```cpp
void help(Node* A,vector<int> &ans,vector<vector<int>> &res){
    if(!A) return;
    ans.push_back(A->data);
    if(!A->left && !A->right){ 
        res.push_back(ans);
        ans.pop_back();
        return;
    }
    help(A->left,ans,res);
    help(A->right,ans,res);
    ans.pop_back();
}
vector<vector<int>> Paths(Node* root){
    vector<int> ans;
    vector<vector<int>> res;
    if(!root) return res;
    help(root,ans,res);
    return res;
}

Link - https://practice.geeksforgeeks.org/problems/root-to-leaf-paths/1#
```

---

## 16. Root to leaf path sum

**Q:** Given a binary tree and a sum `S`, determine if there is a root-to-leaf path where the node values sum up to exactly `S`.

**A:** Pre-order DFS reducing the target sum along the path.

**Explanation:**
- As the recursion moves down, the current node's value is subtracted from `S`.
- When a leaf node is reached, it checks if the remaining `S` equals the leaf's value.
- If either the left or right subtree finds a valid path, the true result bubbles up.

**Complexity:** Time `O(n)` visiting nodes until the path is found, Space `O(h)` for the recursion stack depth.

```cpp
    bool hasPathSum(Node *root, int S) {
        if(!root) return 0;
        if(!root->left && !root->right) return (S==root->data);
        return(hasPathSum(root->left, S-root->data) || hasPathSum(root->right,S-root->data));
    }
    
    Link - https://practice.geeksforgeeks.org/problems/root-to-leaf-path-sum/1#
```

---

## 17. Sum Tree

**Q:** Given a binary tree, check if it is a Sum Tree (where every node's value equals the sum of values in its left and right subtrees, except leaves).

**A:** Pre-order DFS verifying the property with a helper sum function.

**Explanation:**
- For every non-leaf node, the left and right subtrees are independently summed using an `O(n)` helper function.
- If the node's value does not match the computed sum, it returns false.
- It then recursively asserts the Sum Tree property for both the left and right children.

**Complexity:** Time `O(n^2)` due to redundant subtree summations at every level, Space `O(h)` for the recursion stack.

```cpp
    int sum(Node* root){
        if(!root) return 0;
        return root->data+sum(root->left)+sum(root->right);
    }
    bool isSumTree(Node* root) {
        if(!root) return true;
        if(!root->left && !root->right) return true;
        int l = sum(root->left);
        int r = sum(root->right);
        if(root->data != (l+r)) return false;
        return isSumTree(root->left) && isSumTree(root->right);
    }

gfg link - https://www.geeksforgeeks.org/problems/sum-tree/1
```

---

## 18. Top View of Binary Tree

**Q:** Given a binary tree, return the top view of it (the first node visible at each horizontal distance).

**A:** Level-order BFS using a map to lock in the first seen value per horizontal distance.

**Explanation:**
- BFS traverses the tree layer by layer, tracking horizontal distance (`h`).
- A `map<int, int>` records nodes. Because it's level-order, the first node encountered at any `h` is the top-most; the map only writes if `h` is not yet present.
- Iterating over the map extracts the values in strict left-to-right spatial order.

**Complexity:** Time `O(n log n)` due to map operations, Space `O(n)` for the queue and map.

```cpp
    vector<int> topView(Node *root){
        vector<int> ans;
        if(!root) return ans;
        map<int,int> m;
        queue<pair<Node*,int>> q;
        q.push({root,0});
        while(!q.empty()){
            Node* t=q.front().first;
            int h=q.front().second;
            q.pop();
            if(m.find(h)==m.end()) m[h]=t->data;
            if(t->left) q.push({t->left,h-1});
            if(t->right) q.push({t->right,h+1});
        }
        
        for(auto it : m){
            ans.push_back(it.second);
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/top-view-of-binary-tree/1/?page=2&category[]=Tree&curated[]=1&curated[]=7&sortBy=submissions#
```

---

## 19. Vertical sum

**Q:** Given a binary tree, return the sum of all nodes lying on the same vertical line, ordered from left to right.

**A:** Pre-order DFS using a map to aggregate sums by horizontal distance.

**Explanation:**
- A DFS traverses the tree tracking the horizontal distance `vl` (subtract 1 for left, add 1 for right).
- A `map<int, int>` accumulates the node values into the bucket for their respective `vl`.
- Since `std::map` keeps keys sorted, iterating it at the end produces the left-to-right ordered sums.

**Complexity:** Time `O(n log n)` for map insertions, Space `O(n)` for the map and recursion stack.

```cpp
    void solve(int vl,Node *root,map<int,int> &mp){
        if(!root) return;
        mp[vl]+=root->data;
        if(root->left) solve(vl-1,root->left,mp);
        if(root->right) solve(vl+1,root->right,mp);
    }
  
    vector <int> verticalSum(Node *root) {
        vector<int> ans;
        map<int,int> mp;
        solve(0,root,mp);
        for(auto it : mp){
            ans.push_back(it.second);
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/vertical-sum/1
```

---

## 20. Vertical Width of a Binary Tree

**Q:** Given a binary tree, find its vertical width (the total number of distinct vertical lines present).

**A:** Pre-order DFS tracking the minimum and maximum horizontal bounds.

**Explanation:**
- A DFS traverses the tree, maintaining a horizontal position integer `p` (decrement on left, increment on right).
- Global variables `mn` and `mx` update continuously to track the furthest left and right extents reached.
- The total vertical width is calculated as `mx - mn + 1`.

**Complexity:** Time `O(n)` to traverse all nodes, Space `O(h)` for the recursion stack.

```cpp
    int mn=0,mx=0;
    void inorder(Node* root,int p){
        if(!root) return;
        mn=min(mn,p);
        mx=max(mx,p);
        inorder(root->left,p-1);
        inorder(root->right,p+1);
    }
    
    int verticalWidth(Node* root) {
        if(!root) return 0;
        inorder(root,0);
        return mx-mn+1;
    }

gfg link - https://www.geeksforgeeks.org/problems/vertical-width-of-a-binary-tree/1
```

---

