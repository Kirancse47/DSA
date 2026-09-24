# Recursion & Backtracking — Q&A

> Source: `Kirancse47/DSA` -> `Recursion/` — 26 problems (real count)

Ordering: Alphabetical by filename.

## Cheat sheet
```cpp
void backtrack(int index, State& state, vector<Result>& ans) {
    if (is_goal(state)) {
        ans.push_back(state.clone());
        return;
    }
    for (auto choice : get_choices(index, state)) {
        if (is_valid(choice)) {
            state.push(choice);
            backtrack(next_index(index), state, ans);
            state.pop(); // un-choose
        }
    }
}
```
* **Take/Not-take on an index:** Binary decisions per element (e.g., *78. Subsets*, *Subset Sums*).
* **For-loop over choices:** Exploring variable-width paths at each step (e.g., *131. Palindrome Partitioning*, *39. Combination Sum*).
* **Permutations (swapping vs used[]):** Swap elements in-place vs. tracking visited items (e.g., *46. Permutations*, *Largest number in K swaps*).
* **Duplicate-skipping:** Sorting first, then `if(i>start && a[i]==a[i-1]) continue` to avoid duplicate branches (e.g., *40. Combination Sum II*, *90. Subsets II*).
* **Grid backtracking:** Exploring neighbors via `dx`/`dy` and marking matrix cells as visited (e.g., *Rat in a Maze Problem - I*, *79. Word Search*).
* **Constraint checking:** Validating placement rules (e.g., *51. N-Queens*, *37. Sudoku Solver*, *M-Coloring Problem*).
* **Complexity:** Time is roughly `O((Branching Factor)^(Depth) * (Work Per Leaf))` (e.g., `O(2^n * n)` for subsets, `O(n! * n)` for permutations). Space is `O(Depth)` for recursion stack.

---

## 1. 131. Palindrome Partitioning

**Q:** Given a string, partition it such that every substring is a palindrome, returning all possible partitions.

**A:** For-loop over choices (partition lengths).

**Explanation:**
- Decides the length of the current substring starting at `ind`.
- Pushes the substring to `path` if it is a palindrome, recursively finds subsequent partitions, and pops.
- Base case is reaching the end of the string (`ind == s.size()`), appending the path to results.

**Complexity:** Time: `O(2^n * n)` for branch expansion and string copying. Space: `O(n)` stack depth and path list.

```cpp
    bool isPalindrome(string s,int i,int j){
        if(i>=j) return true;
        while(i<j){
            if(s[i]!=s[j]){
                return false;
            }
            i++;j--;
        }
        return true;
    }

    void pp(string s,int ind,vector<string>&path, vector<vector<string>> &res){
        if(ind==s.size()){
            res.push_back(path);
            return;
        }
        for(int k=ind;k<s.size();k++){
            if(isPalindrome(s,ind,k)){
                path.push_back(s.substr(ind,k-ind+1));
                pp(s,k+1,path,res);
                path.pop_back();
            }
        }
    }

    vector<vector<string>> partition(string s) {
        vector<string> path;
        vector<vector<string>> res;
        pp(s,0,path,res);
        return res;
    }
```

---

## 2. 140. Word Break II

**Q:** Given a string and a dictionary, add spaces to construct sentences where each word is valid, returning all possible sentences.

**A:** For-loop over choices (valid dictionary prefixes).

**Explanation:**
- Appends characters sequentially and checks if the current prefix exists in the dictionary.
- If a valid word is found, appends it along with a space to the current sentence and recurses on the rest of the string.
- Base case triggers when the string is entirely consumed, pushing the accumulated sentence to answers.

**Complexity:** Time: `O(2^n * n)` in the worst case with dense dictionaries. Space: `O(n)` max recursion depth.

```cpp
     void solve(int pos,string res,string &s,unordered_set<string>&dict,vector<string> &ans){
        if(pos==s.size()){
            ans.push_back(res);
        }
        res.push_back(' ');
        string t="";
        for(int i=pos;i<s.size();i++){
            t.push_back(s[i]);
            if(dict.find(t)!=dict.end()){
                solve(i+1,res+t,s,dict,ans);
            }
        }
    }

    vector<string> wordBreak(string s, vector<string>& wordDict) {
        vector<string> ans;
        unordered_set<string> dict;
        for(auto it : wordDict) dict.insert(it);

        string t="";
        for(int i=0;i<s.size();i++){
            t.push_back(s[i]);
            if(dict.find(t)!=dict.end()){
                solve(i+1,t,s,dict,ans);
            }
        }
        return ans;
    }
```

---

## 3. 1863. Sum of All Subset XOR Totals

**Q:** Calculate the sum of the XOR totals of every subset of a given array.

**A:** Take/Not-take on an index.

**Explanation:**
- At each array element, recurses twice: once by XORing the element into the running total, and once without it.
- Base case adds the current subset's XOR total into a global sum reference when the end of the array is reached.
- No vectors are passed around; state is just an integer `x` reducing overhead.

**Complexity:** Time: `O(2^n)` since every subset is visited exactly once. Space: `O(n)` for recursion depth.

```cpp
    void solve(int idx,int x,int &ans,vector<int> &nums){
        if(idx==nums.size()){
            ans+=x;
            return;
        }
        solve(idx+1,x^nums[idx],ans,nums);
        solve(idx+1,x,ans,nums);
    }

    int subsetXORSum(vector<int>& nums) {
        int ans=0;
        solve(0,0,ans,nums);
        return ans;
    }
```

---

## 4. 216. Combination Sum III

**Q:** Find all valid combinations of `k` numbers that sum up to `n`, using only digits 1 through 9 at most once.

**A:** Take/Not-take on a fixed integer space (1 to 9).

**Explanation:**
- At each number `i`, decides to include it (subtracting from `n` and `k`, pushing to path) or exclude it.
- Base case captures paths where exactly `k` numbers have summed exactly to `n` (`n==0 && k==0`).
- Prunes branches if numbers exceed 9, `k` drops below 0, or `n` becomes unreachable.

**Complexity:** Time: `O(2^9 * k)` worst case bounded by 9 elements. Space: `O(k)` for combination array and stack size.

```cpp
    void solve(int i,int n,int k,vector<int> &res,vector<vector<int>> &ans){
        if(n==0 && k==0){
            ans.push_back(res);
            return;
        }
        if(i<=n && k>0 && i<10){
            res.push_back(i);
            solve(i+1,n-i,k-1,res,ans);
            res.pop_back();
            solve(i+1,n,k,res,ans);
        }
    }

    vector<vector<int>> combinationSum3(int k, int n) {
        vector<vector<int>> ans;
        vector<int> ds;
        solve(1,n,k,ds,ans);
        return ans;
    }
```

---

## 5. 22. Generate Parentheses

**Q:** Given `n` pairs of parentheses, generate all combinations of well-formed parentheses strings.

**A:** Constrained binary choice (open/close bounds).

**Explanation:**
- Tracks remaining `l` (left) and `r` (right) parentheses.
- Recursively branches to add '(' if `l > 0`, and ')' if `r > l` (ensuring it closes an open parenthesis).
- Base case adds the string when `l == 0` and `r == 0`.

**Complexity:** Time: `O(4^n / sqrt(n))` representing the Catalan number of valid combinations. Space: `O(n)` recursion stack depth.

```cpp
    void solve(int l,int r,string res,vector<string> &ans){
        if(l==0 && r==0){
            ans.push_back(res);
            return;
        }
        if(l>0) solve(l-1,r,res+'(',ans);
        if(r>l) solve(l,r-1,res+')',ans);
    }

    vector<string> generateParenthesis(int n) {
        vector<string> ans;
        solve(n,n,"",ans);
        return ans;
    }
```

---

## 6. 2305. Fair Distribution of Cookies

**Q:** Distribute bags of cookies among `k` children to minimize the maximum cookies obtained by any single child.

**A:** For-loop over choices (assigning item to children).

**Explanation:**
- Assigns the current bag of cookies `c[ind]` to each of the `k` children (bins).
- Upon reaching the end of the bags, calculates the maximum cookies a child has, and updates the global minimum.
- Backtracks by subtracting the bag from the child's tally before trying the next child.

**Complexity:** Time: `O(k^n)` checking all combinations of `n` bags into `k` bins. Space: `O(n + k)` for cookies per child and recursion limit.

```cpp
    void solve(int ind,int k,vector<int>& c,vector<int>& b,int &ans){
        if(ind==c.size()){
            int maxcs=INT_MIN;
            for(auto it : b) maxcs=max(maxcs,it);
            ans=min(ans,maxcs);
            return;
        }
        for(int i=0;i<k;i++){
            b[i]+=c[ind];
            solve(ind+1,k,c,b,ans);
            b[i]-=c[ind];
        }
    }

    int distributeCookies(vector<int>& cookies, int k) {
        vector<int> bags(k,0);
        int ans=INT_MAX;
        solve(0,k,cookies,bags,ans);
        return ans;
    }
```

---

## 7. 37. Sudoku Solver

**Q:** Solve a standard 9x9 Sudoku puzzle by filling empty cells with digits 1-9 such that constraints are met.

**A:** Sequential constraint checking with backtracking.

**Explanation:**
- Iterates over the entire grid seeking the first empty cell (`'.'`).
- Attempts digits '1' through '9' by verifying row, column, and 3x3 block validity.
- If a valid digit leads to a successful overall solution, returns true; otherwise it un-chooses by replacing with `'.'`. 

**Complexity:** Time: `O(9^(empty_cells))` worst-case branching. Space: `O(1)` as grid is bounded 9x9, or `O(81)` recursion depth.

```cpp
    bool isValid(vector<vector<char>>& board,int row,int col, char c){
        for(int i=0;i<board.size();i++){
            if(board[i][col]==c) return false;
            if(board[row][i]==c) return false;
            if(board[3*(row/3)+i/3][3*(col/3)+i%3]==c) return false;
        }
        return true;
    }

    bool solve(vector<vector<char>>& board){
        for(int i=0;i<board.size();i++){
            for(int j=0;j<board[0].size();j++){
                if(board[i][j]=='.'){
                    for(char c='1';c<='1'+board.size()-1;c++){
                        if(isValid(board,i,j,c)){
                            board[i][j]=c;

                            if(solve(board)){
                                return true;
                            }else{
                                board[i][j]='.';
                            }
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    void solveSudoku(vector<vector<char>>& board) {
        solve(board);
    }
```

---

## 8. 386. Lexicographical Numbers

**Q:** Generate all integers from 1 to `n` in strict lexicographical order.

**A:** DFS on a 10-ary tree (numeral appending).

**Explanation:**
- Treats each number as a tree node, branching out by appending digits 0-9 to the end of the current number.
- Adds the current number to the list, then recursively explores children (`cur * 10 + i`).
- Prunes immediately if the generated number exceeds the limit `n`.

**Complexity:** Time: `O(n)` processing each number exactly once. Space: `O(log10(n))` stack depth for the digit length.

```cpp
    void dfs(int cur,int &limit,vector<int> &ans){
        if(cur>limit) return;
        ans.push_back(cur);
        for(int i=0;i<=9;i++){
            dfs(cur*10+i,limit,ans);
        }
    }
    vector<int> lexicalOrder(int n) {
        vector<int> ans;
        for(int i=1;i<=9;i++){
            dfs(i,n,ans);
        }
        return ans;
    }
```

---

## 9. 39. Combination Sum

**Q:** Find all unique combinations of candidates that sum to a target, where a candidate can be used an unlimited number of times.

**A:** Take/Not-take with repetition allowed.

**Explanation:**
- At each index, decides to stay at the same index and subtract `c[i]` from target (using it again), or move to the next index.
- Pushes the element to path if used and pops upon return.
- Base case collects the combination when the target precisely hits 0.

**Complexity:** Time: `O(2^t * k)` loose bound where `t` is target/min_val. Space: `O(t)` max depth of recursion stack.

```cpp
void solve(vector<int>& c,int i,int s,vector<int> &ds,vector<vector<int>> &ans){
        if(s==0){
            ans.push_back(ds);
            return;
        }
        if(i==c.size()){
            return;
        }
        if(c[i]<=s){
            ds.push_back(c[i]);
            solve(c,i,s-c[i],ds,ans);
            ds.pop_back();
        }
        solve(c,i+1,s,ds,ans);
    }
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<int> ds;
        vector<vector<int>> ans;
        solve(candidates,0,target,ds,ans);
        return ans;
    }
```

---

## 10. 40. Combination Sum II

**Q:** Find all unique combinations that sum to a target, where each candidate may only be used once.

**A:** For-loop over choices with duplicate skipping.

**Explanation:**
- Iterates across remaining candidates, skipping those that are identical to the previous sibling to prevent duplicate subsets.
- Recurses with `j+1` (element used once) and a reduced target.
- Prunes the loop immediately if the candidate exceeds the remaining target (relies on sorted input).

**Complexity:** Time: `O(2^n * k)` in the worst case, vastly pruned by sorting. Space: `O(n)` depth for recursion stack and active path.

```cpp
  void solve(vector<int>& c,int i,int s,vector<int> &ds,vector<vector<int>> &ans){
        if(s==0){
            ans.push_back(ds);
            return;
        }
        for(int j=i;j<c.size();j++){
            if(j>i && c[j]==c[j-1]) continue;
            if(c[j]>s) break;
            ds.push_back(c[j]);
            solve(c,j+1,s-c[j],ds,ans);
            ds.pop_back();
        }
    }

    vector<vector<int>> combinationSum2(vector<int>& candidates, int target) {
        vector<int> ds;
        vector<vector<int>> ans;
        sort(candidates.begin(),candidates.end());
        solve(candidates,0,target,ds,ans);
        return ans;
    }
```

---

## 11. 46. Permutations

**Q:** Generate all possible permutations of an array of distinct integers.

**A:** Permutations via swapping.

**Explanation:**
- Swaps the current index with every subsequent index (including itself) to fix an element in the current position.
- Recurses on `ind+1` to permute the rest of the array.
- Backtracks by swapping the elements back to restore the original array order.

**Complexity:** Time: `O(n! * n)` generating all `n!` permutations and deep-copying them. Space: `O(n)` for recursion stack limit.

```cpp
    void solve(int ind,vector<int>& nums,vector<vector<int>>& ans){
        if(ind==nums.size()){
            ans.push_back(nums);
            return;
        }
        for(int i=ind;i<nums.size();i++){
            swap(nums[i],nums[ind]);
            solve(ind+1,nums,ans);
            swap(nums[i],nums[ind]);
        }
    }

    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> ans;
        solve(0,nums,ans);
        return ans;
    }
```

---

## 12. 51. N-Queens

**Q:** Place `n` queens on an `n x n` chessboard such that no two queens attack each other, returning all solutions.

**A:** Column-by-column constraint backtracking.

**Explanation:**
- Iterates row-by-row in the current column, attempting to place a queen.
- Uses `leftRow`, `lowDia`, and `upDia` arrays as `O(1)` constraint lookups instead of scanning the board.
- Places the queen, marks diagonals/row as occupied, recurses to the next column, and backtracks.

**Complexity:** Time: `O(n!)` placing a queen in decreasing available spots. Space: `O(n)` for auxiliary constraint arrays and board copies.

```cpp
    void solve(int col,vector<string> &board,vector<vector<string>> &ans,vector<int> &leftRow,vector<int> &lowDia,vector<int> &upDia,int n){
        if(col==n){
            ans.push_back(board);
            return;
        }

        for(int row=0;row<n;row++){
            if(!leftRow[row] && !lowDia[row+col] && !upDia[n-1+col-row]){
                board[row][col]='Q';
                leftRow[row]=1;
                lowDia[row+col]=1;
                upDia[n-1+col-row]=1;
                solve(col+1,board,ans,leftRow,lowDia,upDia,n);
                board[row][col]='.';
                leftRow[row]=0;
                lowDia[row+col]=0;
                upDia[n-1+col-row]=0;
            }
        }
    }

    vector<vector<string>> solveNQueens(int n) {
        string s(n,'.');
        vector<vector<string>> ans;
        vector<string> board(n);
        for(auto &it : board){
            it=s;
        }
        vector<int> leftRow(n,0),lowDia(2*n-1,0),upDia(2*n-1,0);
        solve(0,board,ans,leftRow,lowDia,upDia,n);
        return ans;
    }
```

---

## 13. 78. Subsets

**Q:** Generate the power set (all subsets) of an array of unique integers.

**A:** Take/Not-take on an index (also includes a bit-manipulation approach).

**Explanation:**
- Recursive approach evaluates index `i`: appends element and branches, then pops and branches without it.
- Reaches base case at the end of the array, logging the generated list.
- Bit manipulation iterates `0` to `2^n-1`, using bit masks to compose each subset directly.

**Complexity:** Time: `O(2^n * n)` for generating and copying subsets. Space: `O(n)` recursion stack depth for DFS, `O(1)` auxiliary for bitwise.

```cpp
// By Recursion
    void solve(int i,vector<int> &nums,vector<int> &ds,vector<vector<int>> &ans){
        if(i==nums.size()){
            ans.push_back(ds);
            return;
        }
        ds.push_back(nums[i]);
        solve(i+1,nums,ds,ans);
        ds.pop_back();
        solve(i+1,nums,ds,ans);
    }

    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> ans;
        vector<int> ds;
        solve(0,nums,ds,ans);
        return ans;
    }

// By Bit Manupulation..
     vector<vector<int>> subsets(vector<int>& nums) {
        int n=nums.size();
        vector<vector<int>> ans;
        for(int i=0;i<(1<<n);i++){
            vector<int> temp;
            for(int j=0;j<n;j++){
                if(i & (1<<j)) temp.push_back(nums[j]);
            }
            ans.push_back(temp);
        }
        return ans;
    }
```

---

## 14. 79. Word Search

**Q:** Check if a given word can be constructed by traversing adjacent cells in an `m x n` character grid.

**A:** Grid backtracking with temporary modification.

**Explanation:**
- Explores all 4 directions recursively whenever a cell matches the target word character.
- Marks the current cell with `'$'` to prevent circular paths (visited state), restoring the character upon backtracking.
- Early exits gracefully if boundaries are crossed or character doesn't match the required index `cnt`.

**Complexity:** Time: `O(m * n * 4^L)` where `L` is the word length. Space: `O(L)` recursion depth.

```cpp
    bool solve(int i,int j,vector<vector<char>>& board, string word,int cnt){
        int n=board.size(),m=board[0].size();
        if(cnt==word.size()){
            return true;
        }
        if(i<0 || i>=n || j<0 || j>=m || board[i][j]!=word[cnt]){
             return false;
         }
        char temp=board[i][j];
        board[i][j]='$';
        bool found=(solve(i,j+1,board,word,cnt+1) || solve(i+1,j,board,word,cnt+1) || solve(i,j-1,board,word,cnt+1) || solve(i-1,j,board,word,cnt+1));
        board[i][j]=temp;
        return found;
    }

    bool exist(vector<vector<char>>& board, string word) {
        int n=board.size(),m=board[0].size();
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(board[i][j]==word[0] && solve(i,j,board,word,0)){
                    return true;
                }
            }
        }
        return false;
    }
```

---

## 15. 90. Subsets II

**Q:** Return all unique subsets from a collection of integers that may contain duplicates.

**A:** For-loop over choices with duplicate skipping (optimized).

**Explanation:**
- The optimized solution sorts the input array first to group duplicates.
- At each level, iterates candidates; uses `if(i != idx && nums[i] == nums[i-1]) continue;` to safely ignore identical sibling branches.
- Contains an alternative approach utilizing a `std::set` to blindly filter duplications.

**Complexity:** Time: `O(2^n * n)` traversing distinct subsets. Space: `O(n)` auxiliary recursion limit.

```cpp
// Using sets...
    void solve(int i,vector<int> &nums,vector<int> &ds,set<vector<int>> &ans){
        if(i==nums.size()){
            ans.insert(ds);
            return;
        }
        ds.push_back(nums[i]);
        solve(i+1,nums,ds,ans);
        ds.pop_back();
        solve(i+1,nums,ds,ans);

    }
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        set<vector<int>> ans;
        vector<int> ds;
        sort(nums.begin(),nums.end());
        solve(0,nums,ds,ans);
        vector<vector<int>> res;
        for(auto it : ans){
            res.push_back(it);
        }
        return res;
    }

// Optimized...
    void solve(int idx,vector<int> &nums,vector<int> &ds,vector<vector<int>> &ans){
        ans.push_back(ds);
        for(int i=idx;i<nums.size();i++){
            if(i!=idx && nums[i]==nums[i-1]) continue;
            ds.push_back(nums[i]);
            solve(i+1,nums,ds,ans);
            ds.pop_back();
        }

    }
    vector<vector<int>> subsetsWithDup(vector<int>& nums) {
        vector<vector<int>>ans;
        vector<int> ds;
        sort(nums.begin(),nums.end());
        solve(0,nums,ds,ans);
        return ans;
    }
```

---

## 16. Find all possible paths from top to bottom

**Q:** Find all paths from the top-left to the bottom-right of an `n x m` grid, moving only down or right.

**A:** Grid backtracking (down and right only).

**Explanation:**
- Moves either down or right from the current cell, recording the value in a temporary path array.
- Ensures moves remain within bounds `i < n-1` and `j < m-1`.
- Base case is reached at `(n-1, m-1)`, pushing the traced path to the result matrix.

**Complexity:** Time: `O(2^(n+m) * (n+m))` path possibilities and copying. Space: `O(n+m)` max recursive frames.

```cpp
void solve(int i,int j,int n,int m,vector<vector<int>> &g,vector<int> &ds,vector<vector<int>> &ans){
        if(i==n-1 && j==m-1){
            ans.push_back(ds);
            return;
        }
        if(i<n-1){
            ds.push_back(g[i+1][j]);
            solve(i+1,j,n,m,g,ds,ans);
            ds.pop_back();
        }
        if(j<m-1){
            ds.push_back(g[i][j+1]);
            solve(i,j+1,n,m,g,ds,ans);
            ds.pop_back();
        }
    }

    vector<vector<int>> findAllPossiblePaths(int n, int m, vector<vector<int>> &grid){
        vector<vector<int>> ans;
        vector<int> ds;
        ds.push_back(grid[0][0]);
        solve(0,0,n,m,grid,ds,ans);
        return ans;
    }

    Link - https://practice.geeksforgeeks.org/problems/find-all-possible-paths-from-top-to-bottom/1/?page=2&status[]=unsolved&curated[]=7&sortBy=submissions#
```

---

## 17. Josephus problem

**Q:** Determine the winner of a game where every `k`-th person is eliminated from a circle of `n` people.

**A:** Tail-recursive reduction.

**Explanation:**
- Computes the index to eliminate dynamically via `(idx + k - 1) % size`.
- Erases the target item from a concrete list representing players and shrinks the problem.
- Halts when array size becomes 1.

**Complexity:** Time: `O(n^2)` due to `std::vector::erase` shifting items inside recursion. Space: `O(n)` call stack limits.

```cpp
    void solve(int idx,int k,vector<int>&a,int &ans){
        int size=a.size();
        if(size==1){
            ans=a[0];
            return;
        }
        idx=(idx+k-1)%size;
        a.erase(a.begin()+idx);
        solve(idx,k,a,ans);
    }

    int josephus(int n, int k){
       vector<int> a;
       for(int i=1;i<=n;i++)
        a.push_back(i);
        int ans=-1;
        solve(0,k,a,ans);
        return ans;
    }

    Link - https://practice.geeksforgeeks.org/problems/josephus-problem/1#
           https://leetcode.com/problems/find-the-winner-of-the-circular-game/
```

---

## 18. Largest number in K swaps

**Q:** Find the largest possible numeric string achievable by making at most `k` character swaps.

**A:** Permutations via swapping.

**Explanation:**
- Tracks the maximum character remaining in the substring to limit unnecessary swaps.
- Swaps the current position with any position holding the `maxc` value.
- Compares the generated string against a global maximum reference and recurses with `k-1` before backtracking the swap.

**Complexity:** Time: `O(n! / (n-k)!)` max branching constrained by swaps. Space: `O(n)` stack calls limit.

```cpp
    void helper(string &max, string str, int k, int idx){
        if(k==0) return;
        int n=str.length();
        char maxc = str[idx];

        for(int i=idx+1;i<n;i++){
            if(maxc<str[i]){
                maxc=str[i];
            }
        }

        if(maxc != str[idx]) k-=1;

        for(int i=idx;i<n;i++){
            if(str[i]==maxc){
                swap(str[i],str[idx]);
                if (str.compare(max) > 0) max = str;
                helper(max, str, k, idx+1);
                swap(str[i],str[idx]);
            }
        }
    }

    string findMaximumNum(string str, int k){
       // code here.
       string max=str;
       helper(max, str, k, 0);
       return max;
    }

    Link - https://practice.geeksforgeeks.org/problems/largest-number-in-k-swaps-1587115620/1
```

---

## 19. Longest Possible Route in a Matrix with Hurdles

**Q:** Calculate the longest path from a start to a destination in a grid avoiding obstacles, without revisiting cells.

**A:** Grid backtracking with visited array.

**Explanation:**
- DFS travels in 4 directions verifying boundaries, path legality (no hurdles), and unvisited status.
- Maintains a 2D boolean `vis` matrix, setting elements to `true` on entry and `false` on backtrack.
- Replaces global maximum `mx` anytime the destination `xd, yd` is explicitly hit.

**Complexity:** Time: `O(4^(n*m))` loose bound tracking all paths. Space: `O(n*m)` for visited state grid and call stack.

```cpp
  void dfs(int x,int y,int dis,vector<vector<int>> &matrix,int &n,int &m,vector<vector<bool>> &vis,int &xd,int &yd,int &mx){
       if(x == xd && y == yd){
           mx = max(mx,dis);
           return;
       }

       int dx[] = {-1,0,1,0};
       int dy[] = {0,1,0,-1};

       for(int i = 0; i < 4; i++){
           int nx = x + dx[i];
           int ny = y + dy[i];
           if(nx>=0 && ny>=0 && nx<n && ny<m && !vis[nx][ny] && matrix[nx][ny]){
               vis[nx][ny] = true;
               dfs(nx,ny,dis+1,matrix,n,m,vis,xd,yd,mx);
               vis[nx][ny] = false;
           }
       }
   }

    int longestPath(vector<vector<int>> matrix, int xs, int ys, int xd, int yd){
       if(matrix[xs][ys] == 0 || matrix[xd][yd] == 0) return -1;
       int n = matrix.size(), m = matrix[0].size();
       vector<vector<bool>> vis(n,vector<bool>(m,false));

       int mx = -1;
       vis[xs][ys] = true;
       dfs(xs,ys,0,matrix,n,m,vis,xd,yd,mx);
       return mx;
    }

    Link- https://practice.geeksforgeeks.org/problems/longest-possible-route-in-a-matrix-with-hurdles/1#
```

---

## 20. M-Coloring Problem

**Q:** Determine if a graph can be colored with at most `m` colors such that no two adjacent vertices share a color.

**A:** Node-by-node constraint checking.

**Explanation:**
- Attempts assigning colors 0 to `m-1` sequentially to a single node.
- Verifies validity with `isSafe`, assuring no direct neighbor shares the desired color.
- If safe, recursively attempts the next node; if a dead end, resets color to `-1`.

**Complexity:** Time: `O(m^v)` verifying combinations of `m` colors over `v` vertices. Space: `O(v)` vector of colors and recursion limits.

```cpp
    bool isSafe(int col,int node,vector<int> &color,vector<int> adj[]){
        for(auto it : adj[node]){
            if(color[it]==col) return false;
        }
        return true;
    }
    bool solve(int node,int v,int m,vector<int> &color,vector<int> adj[]){
        if(node==v) return true;
        for(int i=0;i<m;i++){
            if(isSafe(i,node,color,adj)){
                color[node]=i;
                if(solve(node+1,v,m,color,adj)) return true;
                color[node]=-1;
            }
        }
        return false;
    }
    bool graphColoring(int v, vector<vector<int>> &edges, int m) {
        vector<int> adj[v];
        for(auto it : edges){
            adj[it[0]].push_back(it[1]);
            adj[it[1]].push_back(it[0]);
        }
        vector<int> color(v,-1);
        if(solve(0,v,m,color,adj)) return true;
        else return false;
    }
Link - https://practice.geeksforgeeks.org/problems/m-coloring-problem-1587115620/1
```

---

## 21. Partition array to K subsets

**Q:** Determine if an array of integers can be partitioned into `k` non-empty subsets with equal sums.

**A:** For-loop over choices with visited array.

**Explanation:**
- First calculates a required target sum `sum/k`, aborting early if indivisible.
- Recursively builds subsets using a `vis` array; once a subset hits `tar`, it recurses starting back at index 0 for `k-1` subsets.
- Prunes by skipping already visited items or numbers that would push the running sum above target.

**Complexity:** Time: `O(k * 2^n)` since elements are filtered across bins. Space: `O(n)` visited map and DFS stack.

```cpp
    bool solve(int i,int n,int k,int curs,int tar,int a[],vector<bool> &vis){
        if(k==0) return true;
        if(curs==tar) return solve(0,n,k-1,0,tar,a,vis);
        for(int j=i;j<n;j++){
            if(curs+a[j]>tar || vis[j]) continue;
            vis[j]=true;
            if(solve(j+1,n,k,curs+a[j],tar,a,vis)) return true;
            vis[j]=false;
        }
        return false;
    }
    bool isKPartitionPossible(int a[], int n, int k){
        if(k==1) true;
        if(k>n) return false;
        int sum=0;
        for(int i=0;i<n;i++) sum+=a[i];
        if(sum%k!=0) return false;
        vector<bool> vis(n,false);
        return solve(0,n,k,0,sum/k,a,vis);
    }

    Link - https://practice.geeksforgeeks.org/problems/partition-array-to-k-subsets/1
```

---

## 22. Permutation with Spaces

**Q:** Given a string, insert spaces between characters to generate all possible spatial permutations.

**A:** Take/Not-take on character gaps.

**Explanation:**
- Operates on character boundaries, taking a prefix string and processing the next character.
- Branches into two recursive calls: one appending a space plus the character, another appending just the character.
- The base case adds the composed string to an answers array when the source string is exhausted.

**Complexity:** Time: `O(2^n * n)` for permutations branching out string concatenations. Space: `O(n)` function depth.

```cpp
    void solve(string s,string op,vector<string> &ans){
        if(s.size()==0){
            ans.push_back(op);
            return;
        }
        solve(s.substr(1),op+' '+s[0],ans);
        solve(s.substr(1),op+s[0],ans);
    }

    vector<string> permutation(string S){
        vector<string> ans;
        string op="";
        op.push_back(S[0]);
        solve(S.substr(1),op,ans);
        return ans;
    }

    Link - https://practice.geeksforgeeks.org/problems/permutation-with-spaces3627/1
```

---

## 23. Print N-bit binary numbers having more 1s than 0s

**Q:** Generate all N-bit binary strings where any prefix has at least as many 1s as 0s.

**A:** Constrained binary choice.

**Explanation:**
- Recursively constructs the string parameter `op` character-by-character.
- `1` can be added anytime. `0` can only be appended if current `one` count strictly exceeds `zero` count.
- Hits base case at length `n`, appending generated binary number.

**Complexity:** Time: `O(2^n)` visiting binary combinations safely bound by Catalan subsets. Space: `O(n)` string copies in recursion trace.

```cpp
void solve(int n,int one,int zero,string op,vector<string> &ans){
    if(n==0){
        ans.push_back(op);
        return;
    }
    solve(n-1,one+1,zero,op+'1',ans);
    if(one>zero) solve(n-1,one,zero+1,op+'0',ans);
}

vector<string> NBitBinary(int n){
    vector<string> ans;
    solve(n,0,0,"",ans);
    return ans;
}

gfg link - https://www.geeksforgeeks.org/problems/print-n-bit-binary-numbers-having-more-1s-than-0s0252/1
```

---

## 24. Rat in a Maze Problem - I

**Q:** Find all lexicographically sorted paths for a rat to navigate from top-left to bottom-right in a matrix avoiding blockers.

**A:** Grid backtracking with directional arrays.

**Explanation:**
- Mutates the original grid by flipping `1` to `0` to emulate a visited set, conserving space.
- Branches out in standard `D L R U` order to naturally attempt lexicographical traversal.
- At `(n-1, m-1)`, registers the current string path and un-visits before returning.

**Complexity:** Time: `O(4^(n*m))` exploring non-blocked trails. Space: `O(n*m)` string length on maximum winding path.

```cpp
    vector<string> ans;
    void solve(int i,int j,string path,vector<vector<int>> &maze){
        int n=maze.size(),m=maze[0].size();
        maze[i][j]=0;
        if(i==n-1 && j==m-1) {
            ans.push_back(path);
            maze[i][j]=1;
            return;
        }
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0};
        string dir="RLDU";
        for(int k=0;k<4;k++){
            int x=dx[k]+i;
            int y=dy[k]+j;
            char d=dir[k];
            if(x>=0 && x<n && y>=0 && y<m && maze[x][y]){
                solve(x,y,path+d,maze);
            }
        }
        maze[i][j]=1;
    }
    vector<string> ratInMaze(vector<vector<int>>& maze) {
        if(maze[0][0]==0) return ans;
        solve(0,0,"",maze);
        sort(ans.begin(),ans.end());
        return ans;
    }

    Link - https://practice.geeksforgeeks.org/problems/rat-in-a-maze-problem/1#
```

---

## 25. Subset Sums

**Q:** Find and return the sums of all possible subsets in an array.

**A:** Take/Not-take on an index (accumulator).

**Explanation:**
- Branches recursively picking an element (adds `arr[i-1]` to cumulative sum) or not (leaves sum alone).
- Tracks index decrementally towards 0 as the base case trigger.
- Pushes the final combined `s` without worrying about path allocations.

**Complexity:** Time: `O(2^n)` to cover every power set sum. Space: `O(n)` memory footprint via recursive tree.

```cpp
  void solve(int i,int s,vector<int> &ans,vector<int>&arr){
        if(i==0){
            ans.push_back(s);
            return;
        }
        solve(i-1,s,ans,arr);
        solve(i-1,s+arr[i-1],ans,arr);
    }

    vector<int> subsetSums(vector<int> arr, int N){
        vector<int> ans;
        solve(N,0,ans,arr);
        return ans;
    }

    Link-https://practice.geeksforgeeks.org/problems/subset-sums2234/1#
```

---

## 26. Word Search

**Q:** Check if a given word exists in a matrix by matching contiguous neighbors (alternative formulation).

**A:** Grid backtracking with temporary modification.

**Explanation:**
- Instead of tracking target word character directly by count, matches next char index ahead of the loop.
- Escapes circular DFS by turning active cell character into `'#'`, then recovers it before leaving context.
- Traverses 4 grid neighbors simultaneously through bounded offset combinations.

**Complexity:** Time: `O(m * n * 4^L)` grid span scaled by word length `L`. Space: `O(L)` maximal branch depths limits.

```cpp
int dx[4]={0,0,1,-1};
    int dy[4]={1,-1,0,0};
    bool solve(int idx,int i,int j,vector<vector<char>> &mat,string &word){
        if(idx==word.size()-1) return true;
        int n=mat.size(),m=mat[0].size();
        char tmp=mat[i][j];
        mat[i][j]='#';
        for(int k=0;k<4;k++){
            int x=i+dx[k];
            int y=j+dy[k];
            if(x>=0 && x<n && y>=0 && y<m && mat[x][y]==word[idx+1]){
                if(solve(idx+1,x,y,mat,word)) return true;
            }
        }
        mat[i][j]=tmp; //backtracking
        return false;
    }
    bool isWordExist(vector<vector<char>>& mat, string& word) {
        int n=mat.size(),m=mat[0].size();
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(mat[i][j]==word[0]){
                    if(solve(0,i,j,mat,word)) return true;
                }
            }
        }
        return false;
    }

gfg link - https://www.geeksforgeeks.org/problems/word-search/1
```

---
