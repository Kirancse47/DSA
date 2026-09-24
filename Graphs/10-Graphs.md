# Graphs — Q&A

> Source: `Kirancse47/DSA` -> `Graphs/` — 70 problems.
> Every answer carries the complete solution code exactly as it appears in the repo.

Ordering: alphabetical, matching the source directory. Use the cheat sheet below to
jump by algorithm family.

## Cheat sheet

### Representations
Adjacency list `vector<int> adj[V]` — `O(V+E)` space, the default.
Adjacency matrix `vector<vector<int>>` — `O(V^2)`, only for dense graphs / Floyd-Warshall.
Grid as implicit graph — every cell is a vertex, `V = r*c`, `E = 4*r*c`, so `O(V+E) = O(r*c)`.

### Algorithm table

| Algorithm | Time | Space | Use it when |
|---|---|---|---|
| BFS | `O(V+E)` | `O(V)` | shortest path in an UNWEIGHTED graph, level-order, multi-source spread |
| DFS | `O(V+E)` | `O(V)` | connectivity, components, cycle detection, topological order, backtracking |
| Cycle detect (undirected) | `O(V+E)` | `O(V)` | DFS with a parent check, or BFS, or DSU (union of two already-joined nodes) |
| Cycle detect (directed) | `O(V+E)` | `O(V)` | DFS with a recursion-stack array, or Kahn's algorithm failing to emit V nodes |
| Topological sort (Kahn, BFS) | `O(V+E)` | `O(V)` | DAG ordering; also detects a cycle when the output is shorter than V |
| Topological sort (DFS) | `O(V+E)` | `O(V)` | push to a stack on exit, then reverse |
| Dijkstra (binary heap) | `O(E log V)` | `O(V)` | shortest path, NON-NEGATIVE weights |
| 0-1 BFS (deque) | `O(V+E)` | `O(V)` | weights are only 0 and 1; push_front for 0, push_back for 1 |
| Bellman-Ford | `O(V*E)` | `O(V)` | negative weights allowed; detects a negative cycle on the V-th pass |
| Floyd-Warshall | `O(V^3)` | `O(V^2)` | all-pairs shortest path, small V |
| Prim (heap) | `O(E log V)` | `O(V)` | MST, dense-ish graphs |
| Kruskal | `O(E log E)` | `O(V)` | MST, sort edges then union with DSU |
| DSU (path compression + union by rank/size) | near `O(1)` amortised | `O(V)` | connectivity queries, Kruskal, offline merging |
| Tarjan bridges / articulation points | `O(V+E)` | `O(V)` | critical edges/nodes; uses `tin[]` discovery and `low[]` back-edge reach |
| Kosaraju SCC | `O(V+E)` | `O(V)` | strongly connected components; DFS order, transpose, DFS again |
| Bipartite check | `O(V+E)` | `O(V)` | 2-colouring via BFS/DFS; fails iff an odd cycle exists |

### Which algorithm do I reach for?

- Unweighted shortest path -> BFS.
- Weights all non-negative -> Dijkstra.
- Any negative weight, or need negative-cycle detection -> Bellman-Ford.
- Need every pair, V is small -> Floyd-Warshall.
- Weights are only 0/1 -> 0-1 BFS with a deque.
- "Is it possible to order these tasks / are there prerequisites" -> topological sort.
- "Connect everything at minimum cost" -> MST (Kruskal if edge list, Prim if adjacency).
- "Are these two in the same group", repeated queries -> DSU.
- "Which edge/node, if removed, disconnects the graph" -> Tarjan.
- Grid flood fill, islands, rotting oranges, nearest-cell distance -> BFS from all sources at once.

### Tarjan in one line
`low[u] = min(low[u], low[v])` for tree edges and `min(low[u], tin[v])` for back edges;
edge `(u,v)` is a bridge iff `low[v] > tin[u]`.

---

## 1192. Critical Connections in a Network(Bridges In a Graph)

**Q:** Given `n` nodes and an undirected `connections` edge list, find all critical connections (bridges) whose removal disconnects the graph, returned as a list of edges.

**A:** Tarjan's Bridge-finding Algorithm using DFS to track discovery and lowest reachable times.

**Explanation:**
- The graph is built as an adjacency list from the input `connections`.
- A DFS maintains `tin` (insertion/discovery time) and `low` (lowest discovery time reachable from the current node) arrays.
- During DFS, if a neighbor is already visited and is not the parent, `low[node]` is updated with the neighbor's insertion time.
- If a neighbor's `low` time strictly exceeds the current node's `tin` time after DFS returns, it means there is no back-edge and the edge between them is a critical connection.

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
    void dfs(int node,int parent,vector<int> con[],vector<int> &vis,vector<int> &tin,vector<int> &low,int &timer,vector<vector<int>> &ans){
        vis[node]=1;
        tin[node]=low[node]=timer++;
        
        for(auto it : con[node]){
            if(it==parent) continue;
            if(!vis[it]){
                dfs(it,node,con,vis,tin,low,timer,ans);
                low[node]=min(low[node],low[it]);
                if(low[it]>tin[node]) ans.push_back({node,it});
            }else{
                low[node]=min(low[node],tin[it]);
            }
        }
    }
    
    vector<vector<int>> criticalConnections(int n, vector<vector<int>>& connections) {
        vector<int> con[n];
        for(auto it : connections){
            con[it[0]].push_back(it[1]);
            con[it[1]].push_back(it[0]);
        }
        vector<vector<int>> ans;
        vector<int> vis(n,0),tin(n),low(n);
        int timer=0;
        for(int i=0;i<n;i++){
            if(!vis[i]){
                dfs(i,-1,con,vis,tin,low,timer,ans);
            }
        }
        return ans;
    }
```

---

## 1219. Path with Maximum Gold

**Q:** Given an `r` by `c` grid where cells contain gold amounts (0 means impassable), return the maximum amount of gold you can collect starting from any valid cell without revisiting cells in a single path.

**A:** Backtracking DFS exploring all 4-directional valid paths from every non-zero starting cell.

**Explanation:**
- For every non-zero cell in the grid, a backtracking DFS explores all available adjacent cells containing gold.
- The current cell's value is temporarily overwritten to 0 before recursing, acting as the visited state tracker for the current path.
- The maximum gold collected is updated continuously during the traversal.
- After all valid DFS branches from a cell return, its original gold value is restored to allow other independent paths to visit it.

**Complexity:** Time `O(r*c*3^(r*c))`, Space `O(r*c)`

```cpp
    void solve(int gold,int i,int j,vector<vector<int>>& grid,int &ans){
        if (i<0 || i>=grid.size() || j<0 || j>=grid[0].size() || grid[i][j]==0) return;
        int tmp=grid[i][j];
        gold+=tmp;
        ans=max(ans,gold);
        grid[i][j]=0;
        int dx[]={0,1,0,-1};
        int dy[]={1,0,-1,0};
        for(int k=0;k<4;k++){
            int x=i+dx[k];
            int y=j+dy[k];
            solve(gold,x,y,grid,ans);
        }
        grid[i][j]=tmp;
    }

    int getMaximumGold(vector<vector<int>>& grid) {
        int n=grid.size(),m=grid[0].size();
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j]){
                    solve(0,i,j,grid,ans);
                }
            }
        }
        return ans;
    }
```

---

## 1267. Count Servers that Communicate

**Q:** Given an `r` by `c` grid representing servers, count the number of servers that can communicate with at least one other server in the same row or column.

**A:** Two-pass frequency counting on rows and columns.

**Explanation:**
- An initial pass computes the server frequencies for each row and column by accumulating the 1s into `Rows` and `Col` arrays.
- A second pass iterates through the grid again, inspecting every cell.
- If a server (value 1) is found, it can communicate if either its row count or column count strictly exceeds 1.
- No graph traversal is needed because row/column counts inherently capture the implicit bipartite communication edges.

**Complexity:** Time `O(r*c)`, Space `O(r+c)`

```cpp
    int countServers(vector<vector<int>>& grid) {
        vector<int> Rows(grid.size());
        vector<int> Col(grid[0].size());
        
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[0].size(); j++) {
                Rows[i] += grid[i][j];
                Col[j] += grid[i][j];
            }
        }
        
        int ans = 0;
        for (int i = 0; i < grid.size(); i++) {
            for (int j = 0; j < grid[0].size(); j++) {
                if (grid[i][j] == 1 && (Rows[i] > 1 || Col[j] > 1)) {
                    ans++;
                }
            }
        }
        return ans;
    }
```

---

## 127. Word Ladder

**Q:** Given a `startWord`, `targetWord`, and a `wordList`, find the shortest transformation sequence length from start to target changing one letter at a time, where every intermediate word must be in the list.

**A:** BFS on implicitly connected words by changing one character at a time.

**Explanation:**
- The `wordList` is inserted into an unordered_set `wordSet` for fast lookup and to track unvisited words.
- A queue maintains pairs of `(current_word, current_ladder_length)` for the BFS level-order traversal.
- For each popped word, all 26 possible letters are substituted at each character position to generate neighbors, avoiding expensive character-by-character array comparisons.
- If a generated neighbor matches `targetWord`, the answer is returned; otherwise, if it exists in `wordSet`, it is pushed to the queue and erased from the set to prevent revisits.

**Complexity:** Time `O(N * L * 26)` where `N` is wordList length and `L` is word length, Space `O(N * L)`

```cpp
    int ladderLength(string startWord, string targetWord, vector<string>& wordList) {
        unordered_set<string> wordSet;
        for(auto it : wordList) wordSet.insert(it);
        queue<pair<string,int>> q;
        q.push({startWord,1});
        while(!q.empty()){
            string word=q.front().first;
            int ladder=q.front().second;
            q.pop();
            if(word==targetWord) return ladder;
            wordSet.erase(word);
            for(int i=0;i<word.size();i++){
                char c=word[i];
                for(char j='a';j<='z';j++){
                    if(c==j) continue;
                    word[i]=j;
                    if(wordSet.find(word)!=wordSet.end()){
                        q.push({word,ladder+1});
                    }
                }
                word[i]=c;
            }
        }
        return 0;
    }
```

---

## 130. Surrounded Regions

**Q:** Given an `r` by `c` matrix of 'X' and 'O', capture all 'O' regions completely surrounded by 'X' by flipping them to 'X'.

**A:** Multi-source BFS from the grid boundaries to mark unsurrounded 'O's.

**Explanation:**
- A queue is initialized with all border 'O' cells, which by definition cannot be surrounded.
- Boundary 'O' cells are temporarily marked as 'Y' to indicate they are safe and already visited.
- The BFS propagates inwards 4-directionally from these boundary nodes, marking any connected 'O' cells as 'Y'.
- Finally, a nested loop flips all remaining 'O's (the surrounded ones) to 'X', and restores the safe 'Y's back to 'O'.

**Complexity:** Time `O(r*c)`, Space `O(r*c)`

```cpp
 void solve(vector<vector<char>>& mat) {
        int n=mat.size(),m=mat[0].size();
        queue<pair<int,int>> q;
        for(int i=0;i<m;i++){
            if(mat[0][i]=='O'){
                q.push({0,i});
                mat[0][i]='Y';
            }
            if(mat[n-1][i]=='O'){
                q.push({n-1,i});
                mat[n-1][i]='Y';
            }
        }
        for(int i=0;i<n;i++){
            if(mat[i][0]=='O'){
                q.push({i,0});
                mat[i][0]='Y';
            }
            if(mat[i][m-1]=='O'){
                q.push({i,m-1});
                mat[i][m-1]='Y';
            }
        }
        
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0,};
        while(!q.empty()){
            int i=q.front().first;
            int j=q.front().second;
            q.pop();
            for(int k=0;k<4;k++){
                int x=i+dx[k];
                int y=j+dy[k];
                if(x>=0 && x<n && y>=0 && y<m && mat[x][y]=='O'){
                    mat[x][y]='Y';
                    q.push({x,y});
                }
            }
            
        }
        
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(mat[i][j]=='O') mat[i][j]='X';
            }
        }
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(mat[i][j]=='Y') mat[i][j]='O';
            }
        }
    }
```

---

## 1319. Number of Operations to Make Network Connected

**Q:** Given `n` nodes and an edge list `con` representing ethernet cables, return the minimum number of cables to move to connect all nodes, or -1 if impossible.

**A:** Disjoint Set Union (DSU) to count redundant edges and connected components.

**Explanation:**
- The graph is processed by applying Union operations on all provided edges while tracking disjoint sets using `parent` and `rank` arrays.
- If two nodes belong to the same parent set, the edge between them is redundant and increments the `disEdg` count.
- The total number of disconnected components is counted by tallying nodes where `parent[i] == i`.
- Connecting `comp` components requires `comp - 1` edges; if `reqEdg > disEdg`, it returns -1, otherwise it returns `reqEdg`.

**Complexity:** Time `O(E * alpha(V))`, Space `O(V)`

```cpp
    int findParent(int node,vector<int> &parent){
        if(node==parent[node]) return node;
        return parent[node]=findParent(parent[node],parent);
    }
    void Union(int x,int y,vector<int> &parent,vector<int> &rank){
        x=findParent(x,parent);
        y=findParent(y,parent);
        if(rank[x]>rank[y]){
            parent[y]=x;
        }else if(rank[x]<rank[y]){
            parent[x]=y;
        }else{
            parent[y]=x;
            rank[x]++;
        }
    }
    
    int makeConnected(int n, vector<vector<int>>& con) {
        if(n-1>con.size()) return -1;
        vector<int> parent(n),rank(n,0);
        for(int i=0;i<n;i++){
            parent[i]=i;
        }
        int disEdg=0;
        for(auto it : con){
            if(findParent(it[0],parent)!=findParent(it[1],parent)){
                Union(it[0],it[1],parent,rank);
            }else disEdg++;
        }
        int comp=0;
        for(int i=0;i<n;i++){
            if(parent[i]==i) comp++;
        }
        int reqEdg=comp-1;
        if(reqEdg>disEdg) return -1;
        return reqEdg;
    }
```

---

## 133. Clone Graph

**Q:** Given a reference to a `Node` in a connected undirected graph, return a deep copy (clone) of the entire graph.

**A:** DFS recursive cloning with an unordered_map to track already cloned nodes.

**Explanation:**
- An `unordered_map<Node*, Node*>` state array stores the mapping from the original node pointer to the newly allocated cloned node pointer.
- Upon visiting a node, a new clone is instantiated and immediately saved in the map before recursively calling neighbors.
- During neighborhood traversal, if an original neighbor already exists in the map, its cloned pointer is directly pushed into the clone's neighbor list to prevent infinite cycles.
- The DFS terminates dynamically once all reachable connected nodes and back-edges are fully copied.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
    Node* dfs(Node *node,unordered_map<Node*,Node*> &mp){
        Node *clone=new Node(node->val);
        mp[node]=clone;
        for(auto it : node->neighbors){
            if(mp.find(it)!=mp.end()) clone->neighbors.push_back(mp[it]);
            else clone->neighbors.push_back(dfs(it,mp));
        }
        return clone;
    }
    
    Node* cloneGraph(Node* node) {
        if(!node) return node;
        if(node->neighbors.size()==0){
            return new Node(node->val);
        }
        unordered_map<Node*,Node*> mp;
        return dfs(node,mp);
    }
```

---

## 1514. Path with Maximum Probability

**Q:** Given `n` nodes, an `edges` list with corresponding success probabilities `succProb`, return the maximum probability of reaching `end_node` from `start_node`.

**A:** Dijkstra's algorithm modified to maximize probabilities rather than minimizing distances.

**Explanation:**
- The graph is built as an adjacency list mapping nodes to `(neighbor, probability)` pairs.
- A max-heap `priority_queue` stores `(current_probability, node)`, initializing with `(1.0, start_node)`.
- The `prob` array tracks the highest seen probability to reach each node, pruning paths that yield a lesser or equal probability.
- Since edge weights are probabilities (0 <= p <= 1), multiplying probabilities along a path strictly decreases or maintains the value, fulfilling the monotonic requirement for Dijkstra.

**Complexity:** Time `O(E \log V)`, Space `O(V+E)`

```cpp
    double maxProbability(int n, vector<vector<int>>& edges, vector<double>& succProb, int start_node, int end_node) {
        vector<pair<int,double>> adj[n];
        vector<double> prob(n,0);
        for(int i=0;i<edges.size();i++){
            adj[edges[i][0]].push_back({edges[i][1],succProb[i]});
            adj[edges[i][1]].push_back({edges[i][0],succProb[i]});
        }
        priority_queue<pair<double,int>> pq;
        pq.push({1.0,start_node});
        prob[start_node]=1.0;
        while(!pq.empty()){
            int node=pq.top().second;
            double p=pq.top().first;
            pq.pop();
            for(auto it : adj[node]){
                if(prob[it.first]<(p*it.second)){
                    prob[it.first]=p*it.second;
                    pq.push({p*it.second,it.first});
                }
            }
        }
        return prob[end_node];
    }
```

---

## 1971. Find if Path Exists in Graph

**Q:** Given `n` vertices, a bidirectional `edges` list, a `source`, and a `destination`, return whether there is a valid path connecting source and destination.

**A:** Standard BFS traversal using a queue to find reachability.

**Explanation:**
- The `edges` list is transformed into a standard bidirectional adjacency list mapping using an `unordered_map<int, vector<int>>`.
- A queue is initialized with the `source` node, and an `unordered_set` tracks visited nodes to prevent cycles.
- While popping from the queue, if the popped node equals the `destination`, a valid path is confirmed and the search returns `true`.
- Unvisited neighbors are enqueued and marked as visited until the queue is exhausted.

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
    bool validPath(int n, vector<vector<int>>& edges, int source, int destination) {
        unordered_map<int, vector<int>> graph;
        for (auto& edge : edges) {
            int u = edge[0];
            int v = edge[1];
            graph[u].push_back(v);
            graph[v].push_back(u);
        }
        queue<int> queue;
        unordered_set<int> visited;
        queue.push(source);
        visited.insert(source);
        while (!queue.empty()) {
            int node = queue.front();
            queue.pop();
            if (node == destination) return true;
            for (int neighbor : graph[node]) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    queue.push(neighbor);
                }
            }
        }
        return false;
    }
```

---

## 1976. Number of Ways to Arrive at Destination

**Q:** Given `n` nodes and an edge list `roads` specifying travel times, return the number of distinct paths from node 0 to node `n-1` that take the absolute minimum amount of time, modulo 10^9+7.

**A:** Dijkstra's Algorithm with an auxiliary array to accumulate path counts.

**Explanation:**
- A min-heap priority queue extracts the shortest travel time first, operating on an adjacency list of `(neighbor, weight)`.
- A `min_cost` array stores the strictly shortest time to each node, and a `count_ways` array tracks the number of ways to achieve this optimal time.
- If a newly computed travel time to a neighbor strictly improves `min_cost`, both `min_cost` and `count_ways` are reset for that neighbor, and it is pushed into the heap.
- If the travel time exactly matches the known `min_cost`, the neighbor's `count_ways` is incremented by the current node's ways, meaning we found another optimal route to the neighbor.

**Complexity:** Time `O(E \log V)`, Space `O(V+E)`

```cpp
class Solution {
public:
    #define ll long long
    #define pll pair<ll,ll>
    #define MOD 1000000007
    int countPaths(int n, vector<vector<int>>& roads) {
        //Make Adjacency List
        vector<vector<pll>> adj(n);
        for(auto& road: roads){
            adj[road[0]].push_back(make_pair(road[1],road[2]));
            adj[road[1]].push_back(make_pair(road[0],road[2]));
        }

        //Apply Dijkstra and keep tracking number of ways to reach a node with min_cost
        priority_queue<pll,vector<pll>,greater<pll>> minheap;
        minheap.push(make_pair(0,0));//{cost,node}: Source is 0

        vector<ll> count_ways(n,0);//Count total ways to reach to a node from start (0)
        count_ways[0] = 1;//There is 1 way to reach to start node
        vector<ll> min_cost(n,LONG_MAX);
        min_cost[0] = 0;
        
        while(!minheap.empty()){
            pll curr = minheap.top();
            minheap.pop();
            ll cost = curr.first;
            ll curr_node = curr.second;
            for(auto& [nbr,weight]: adj[curr_node]){
                if(cost + weight == min_cost[nbr])
                    count_ways[nbr] = (count_ways[nbr] + count_ways[curr_node])%MOD;
                else if(cost + weight < min_cost[nbr]){
                    min_cost[nbr] = cost + weight;
                    count_ways[nbr] = count_ways[curr_node];
                    minheap.push(make_pair(cost + weight, nbr));//Push node only if it minimizes cost
                } 
            }
        }
        return count_ways[n-1];
    }
};
```

---

## 1992. Find All Groups of Farmland

**Q:** Given an `r` by `c` grid `land` of 1s (farmland) and 0s (forest), find the coordinates of the top-left and bottom-right corners of every rectangular group of farmland.

**A:** BFS over unvisited farmland components to compute bounding boxes.

**Explanation:**
- The grid is iterated over, and whenever an unvisited farmland cell (`1`) is encountered, a BFS is launched to traverse the entire connected component.
- The BFS queue stores coordinates, and a `set<pair<int,int>> visited` tracks globally visited cells to avoid recounting fields.
- For each popped cell, it updates variables tracking the minimum and maximum row and column indices seen so far in the component.
- Because the problem guarantees the farmland forms perfect rectangles, these tracked min and max coordinates uniquely define the top-left and bottom-right corners returned.

**Complexity:** Time `O(r*c \log(r*c))`, Space `O(r*c)`

```cpp
    vector<vector<int>> findFarmland(vector<vector<int>>& land) {
        int rows = land.size();
        int cols = land[0].size();
        vector<vector<int>> result;
        vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        set<pair<int, int>> visited;
        
        auto bfs = [&](int start_row, int start_col) -> vector<int> {
            queue<pair<int, int>> q;
            q.push({start_row, start_col});
            visited.insert({start_row, start_col});
            int min_row = start_row, min_col = start_col, max_row = start_row, max_col = start_col;
            
            while (!q.empty()) {
                auto [cur_row, cur_col] = q.front();
                q.pop();
                
                for (auto [dr, dc] : directions) {
                    int new_row = cur_row + dr;
                    int new_col = cur_col + dc;
                    
                    if (new_row >= 0 && new_row < rows && new_col >= 0 && new_col < cols && visited.find({new_row, new_col}) == visited.end() && land[new_row][new_col] == 1) {
                        visited.insert({new_row, new_col});
                        q.push({new_row, new_col});
                        min_row = min(min_row, new_row);
                        min_col = min(min_col, new_col);
                        max_row = max(max_row, new_row);
                        max_col = max(max_col, new_col);
                    }
                }
            }
            
            return {min_row, min_col, max_row, max_col};
        };
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (land[i][j] == 1 && visited.find({i, j}) == visited.end()) {
                    vector<int> farmland = bfs(i, j);
                    result.push_back(farmland);
                }
            }
        }
        return result;
    }
```

---

## 200. Number of Islands

**Q:** Given an `m` by `n` grid of '1's (land) and '0's (water), return the total number of connected land components (islands).

**A:** Iterative BFS or recursive DFS initiated upon finding an unvisited land cell, mutating the grid directly to track visits.

**Explanation:**
- The matrix is traversed cell by cell looking for unvisited '1's; each discovery increments the island count.
- During traversal, connected components are explored 4-directionally and immediately mutated from '1' to '0'.
- This direct mutation prevents infinite loops and avoids the need for a separate visited array.
- The exploration implicitly finishes the island, ensuring the outer loop only triggers traversals for entirely new components.

**Complexity:** BFS Approach: Time `O(m*n)`, Space `O(m*n)`
DFS Approach: Time `O(m*n)`, Space `O(m*n)`

```cpp
// Using BFS...
    int numIslands(vector<vector<char>>& grid) {
        int n=grid.size(),m=grid[0].size();
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j]=='1'){
                    ans++;
                    queue<pair<int,int>> q;
                    q.push({i,j});
                    while(!q.empty()){
                        int len=q.size();
                        for(int z=0;z<len;z++){
                            int i=q.front().first;
                            int j=q.front().second;
                            q.pop();
                            int dx[]={0,1,0,-1};
                            int dy[]={1,0,-1,0};
                            for(int k=0;k<4;k++){
                                int x=i+dx[k];
                                int y=j+dy[k];
                                if(x>=0 && x<n && y>=0 && y<m && grid[x][y]=='1'){
                                    grid[x][y]='0';
                                    q.push({x,y});
                                }
                            }
                        }
                    }
                }
            }
        }
        return ans;
    }
    
// Using DFS...
    void dfs(vector<vector<char>> &grid, int i, int j, int m, int n) {
        if(i>=0 && i<m && j>=0 && j<n && grid[i][j] == '1') {
            grid[i][j] = 0;
            dfs(grid, i-1, j, m, n);
            dfs(grid, i+1, j, m, n);
            dfs(grid, i, j-1, m, n);
            dfs(grid, i, j+1, m, n);
        }
        else return;
    }
    
    int numIslands(vector<vector<char>>& grid) {
        if(grid.size() == 0) return 0;
        int m = grid.size(),n = grid[0].size();
        int ans = 0;
        for(int i=0;i<m;i++) {
            for(int j=0;j<n;j++) {
                if(grid[i][j] == '1') {
                    ans++;
                    dfs(grid, i, j, m, n);
                }
            }
        }
        return ans;
    }
```

---

## 207. Course Schedule

**Q:** Given `numCourses` and a list of `prerequisites` (edges), determine if it is possible to finish all courses (i.e., whether the directed graph has no cycles).

**A:** Kahn's BFS Algorithm for Topological Sort using an indegree array.

**Explanation:**
- A directed adjacency list and an `inDeg` array are built to represent courses and how many prerequisites each course has.
- A queue is initialized with all courses possessing an indegree of 0 (courses with no remaining prerequisites).
- As courses are popped, the count of processed nodes increases, and the indegree of adjacent outgoing nodes is decremented; nodes hitting an indegree of 0 are enqueued.
- If the final processed count equals `numCourses`, the graph is a Directed Acyclic Graph (DAG) and completion is possible.

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
    // Cycle checking using toposort...
    bool isCyclic(int V, unordered_map<int,vector<int>> &adj) {
        vector<int> inDeg(V,0);
        for(int i=0;i<V;i++){
            for(auto it : adj[i]) inDeg[it]++;
        }
        
        queue<int> q;
        for(int i=0;i<V;i++){
            if(inDeg[i]==0) q.push(i);
        }
        
        int cnt=0;
        while(!q.empty()){
            int node=q.front();q.pop();
            cnt++;
            for(auto it : adj[node]){
                inDeg[it]--;
                if(inDeg[it]==0) q.push(it);
            }
        }
        return cnt==V;
    }
    
    bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
        unordered_map<int,vector<int>> mp;
        for(auto it : prerequisites){
            mp[it[1]].push_back(it[0]);
        }
        return isCyclic(numCourses,mp);
    }
```

---

## 210. Course Schedule II

**Q:** Given `numCourses` and a `prerequisites` list, return an array representing a valid ordering of courses to take, or an empty array if a cycle makes it impossible.

**A:** Kahn's BFS Algorithm for Topological Sort appending to a result array.

**Explanation:**
- The graph is built as an adjacency list mapped from prerequisites, maintaining an `inDeg` array counting incoming edges.
- A queue is primed with all nodes that have 0 incoming edges (starting courses).
- Popped nodes are pushed to an `ans` vector while their neighboring nodes have their incoming degree decremented and are pushed to the queue if their degree hits 0.
- Termination yields a valid order if the size of `ans` strictly equals `numCourses`; if lesser, a cycle is present and an empty vector is returned.

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
     vector<int> isCyclic(int V, unordered_map<int,vector<int>> &adj) {
        vector<int> inDeg(V,0);
        for(int i=0;i<V;i++){
            for(auto it : adj[i]) inDeg[it]++;
        }
        queue<int> q;
        for(int i=0;i<V;i++){
            if(inDeg[i]==0) q.push(i);
        }
        vector<int> ans;
        while(!q.empty()){
            int node=q.front();q.pop();
            ans.push_back(node);
            for(auto it : adj[node]){
                inDeg[it]--;
                if(inDeg[it]==0) q.push(it);
            }
        }
        if(ans.size()==V) return ans;
        return {};
    }
    vector<int> findOrder(int numCourses, vector<vector<int>>& prerequisites) {
        unordered_map<int,vector<int>> mp;
        for(auto it : prerequisites){
            mp[it[1]].push_back(it[0]);
        }
        return isCyclic(numCourses,mp);
    }
```

---

## 2192. All Ancestors of a Node in a Directed Acyclic Graph

**Q:** Given `n` nodes and directed `edges`, return a list of lists where the `i`-th list contains all ancestors of the `i`-th node, sorted in ascending order.

**A:** DFS launched individually from every potential ancestor to populate descendant reachable sets.

**Explanation:**
- The problem translates to finding all descendants for each node, which gives the reverse mapping (ancestors for each descendant).
- The directed graph is built normally as an adjacency list.
- A DFS is started iteratively from every node `i` acting as the absolute `parent` for the current traversal.
- For each unvisited node reached during this DFS, `parent` is pushed directly into the descendant's `res` list, guaranteeing inclusion of all distant connections.
- After all DFS runs conclude, each node's ancestor list is explicitly sorted.

**Complexity:** Time `O(V * (V+E) + V^2 \log V)`, Space `O(V^2)`

```cpp
 void dfs(vector<vector<int>>& graph, int parent, int curr, vector<vector<int>>& res, vector<bool>& visit) {
        visit[curr] = true;
        for (int dest : graph[curr]) {
            if (!visit[dest]) {
                res[dest].push_back(parent);
                dfs(graph, parent, dest, res, visit);
            }
        }
    }

    vector<vector<int>> getAncestors(int n, vector<vector<int>>& edges) {
        vector<vector<int>> res(n);
        vector<vector<int>> graph(n);
        
        for (const auto& edge : edges) {
            graph[edge[0]].push_back(edge[1]);
        }
        
        for (int i = 0; i < n; ++i) {
            vector<bool> visit(n, false);
            dfs(graph, i, i, res, visit);
        }
        
        for (int i = 0; i < n; ++i) {
            sort(res[i].begin(), res[i].end());
        }
        
        return res;
    }
```

---

## 2658. Maximum Number of Fish in a Grid

**Q:** Given an `r` by `c` grid representing a lake where cells have fish (or 0 for land), find the maximum number of fish a fisher can catch by starting at any water cell and moving 4-directionally to connected water cells.

**A:** DFS computing the component sum across a matrix, directly modifying the grid to mark visited.

**Explanation:**
- The grid is scanned to find any cell containing fish (> 0) to act as a source node.
- A recursive DFS is spawned that accumulates the fish value of the cell into a tracking variable `r` and overwrites the cell value to 0 to prevent loops.
- The DFS pushes outwards into all four adjacent cells, terminating at grid boundaries or land (0) cells.
- The largest accumulated sum across all initiated DFS traversals represents the maximum catch.

**Complexity:** Time `O(r*c)`, Space `O(r*c)`

```cpp
    void dfs(int i,int j,vector<vector<int>> &g,int &r){
        if(i<0 || i>=g.size() || j<0 || j>=g[0].size() || !g[i][j]) return;
        r+=g[i][j];
        g[i][j]=0;
        dfs(i,j+1,g,r);
        dfs(i,j-1,g,r);
        dfs(i+1,j,g,r);
        dfs(i-1,j,g,r);
    }
    int findMaxFish(vector<vector<int>>& grid) {
        int n=grid.size(),m=grid[0].size();
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j]){
                    int res=0;
                    dfs(i,j,grid,res);
                    ans=max(ans,res);
                }
            }
        }
        return ans;
    }
```

---

## 2685. Count the Number of Complete Components

**Q:** Given an undirected graph of `n` nodes and an edge list, count the number of connected components that are strictly complete graphs (i.e. every pair of vertices in the component is connected).

**A:** DFS traversing connected components to count exactly how many vertices and edges reside within each component.

**Explanation:**
- The graph is converted into a standard bidirectional adjacency list.
- An iterative loop over all `n` nodes launches a DFS on any unvisited node, passing references to `node` and `edges` counters.
- The DFS marks nodes as `vis[i] = true`, increments the `node` tally by 1, and sums the size of outgoing adjacency lists into `edges` (each true bidirectional edge gets tallied twice).
- A component is declared "complete" if the total number of outgoing edges strictly equals `node * (node - 1)` (the definition of a complete graph factoring in the double-counting).

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
    void dfs(int i,int &n,int &e,vector<int> adj[],vector<bool> &vis){
        n++;
        vis[i]=true;
        for(auto it : adj[i]){
            e++;
            if(!vis[it]){
                dfs(it,n,e,adj,vis);
            }
        }
    }
    int countCompleteComponents(int n, vector<vector<int>>& edges) {
        vector<int> adj[n];
        for(auto it : edges){
            adj[it[0]].push_back(it[1]);
            adj[it[1]].push_back(it[0]);
        }
        vector<bool> vis(n,false);
        int ans=0;
        for(int i=0;i<n;i++){
            if(!vis[i]){
                int node=0;
                int edges=0;
                dfs(i,node,edges,adj,vis);
                if((node*(node-1))==edges){
                    ans++;
                }
            }
        }
        return ans;
    }
```

---

## 407. Trapping Rain Water II

**Q:** Given an `m` by `n` grid representing an elevation map, compute the total volume of water it can trap after raining.

**A:** BFS utilizing a min-heap priority queue initialized with the outer boundary cells to simulate a rising water level.

**Explanation:**
- The algorithm pushes all outer boundary cells of the grid into a min-heap, marking them as visited since water will spill out from the lowest boundary first.
- A `level` variable tracks the maximum height the bounding wall has reached so far.
- As the minimum-height cell is popped from the heap, its unvisited 4-directional neighbors are checked.
- If a neighbor's height is strictly lower than the current `level`, water gets trapped equivalent to `level - neighbor_height`; the neighbor is then pushed into the heap and marked visited.

**Complexity:** Time `O(m*n \log(m*n))`, Space `O(m*n)`

```cpp
    int trapRainWater(vector<vector<int>>& heightMap) {
        int m=heightMap.size(),n=heightMap[0].size();
        if(m<3 || n<3) return 0;
        vector<vector<bool>> vis(m,vector<bool>(n,false));
        priority_queue< vector<int>, vector<vector<int>>, greater<vector<int>> > pq;
        for(int i=0;i<m;i++){
            pq.push({heightMap[i][0],i,0});
            pq.push({heightMap[i][n-1],i,n-1});
            vis[i][0]=vis[i][n-1]=true;
        }
        for(int i=0;i<n;i++){
            pq.push({heightMap[0][i],0,i});
            pq.push({heightMap[m-1][i],m-1,i});
            vis[0][i]=vis[m-1][i]=true;
        }
        int water=0,level=0;
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0};
        while(!pq.empty()){
            auto it=pq.top();pq.pop();
            int height=it[0],i=it[1],j=it[2];
            level=max(level,height);
            for(int k=0;k<4;k++){
                int x=i+dx[k];
                int y=j+dy[k];
                if(x>=0 && x<m && y>=0 && y<n && !vis[x][y]){
                    pq.push({heightMap[x][y],x,y});
                    vis[x][y]=true;
                    if(heightMap[x][y]<level) water+=(level-heightMap[x][y]);
                }
            }
        }
        return water;
    }
```

---

## 684. Redundant Connection

**Q:** Given a list of `edges` belonging to a tree that has exactly one additional redundant edge added to create a cycle, find and return that redundant edge.

**A:** Disjoint Set Union (DSU) with Path Compression and Union by Rank iterating sequentially over the edges.

**Explanation:**
- Iterating sequentially, for each edge the algorithm invokes a `findParent` function to check the current root leader of both endpoints.
- A 1D `parent` array represents disjoint sets, while `rank` keeps trees shallow during unifications.
- If two nodes yield different leaders, they are disconnected and the algorithm invokes `unionNode` to merge their sets.
- If two nodes yield the identical leader, it signifies they are already connected via a previous path, making the current edge the redundant one that causes the cycle.

**Complexity:** Time `O(E * alpha(V))`, Space `O(V)`

```cpp
    int findParent(int node,vector<int> &parent){
        if(node==parent[node]) return node;
        return parent[node]=findParent(parent[node],parent);
    }
    void unionNode(int x,int y,vector<int> &parent,vector<int> &rank){
        if(rank[x]>rank[y]) parent[y]=x;
        else if(rank[y]>rank[x]) parent[x]=y;
        else{
            parent[y]=x;
            rank[x]++;
        }
    }
    vector<int> findRedundantConnection(vector<vector<int>>& edges) {
        int n=edges.size();
        vector<int> parent(n+1),rank(n+1,0);
        for(int i=1;i<=n;i++) parent[i]=i;
        for(auto it : edges){
            int x=findParent(it[0],parent);
            int y=findParent(it[1],parent);
            if(x!=y) unionNode(x,y,parent,rank);
            else return it;
        }
        return {};
    }
```

---

## 695. Max Area of Island

**Q:** Given an `r` by `c` grid filled with 0s and 1s, return the maximum area (number of cells) of a 4-directionally connected component of 1s.

**A:** DFS accumulation returning the number of visited nodes in a connected component, directly wiping grid values to track state.

**Explanation:**
- As the grid is traversed, any unvisited 1 acts as a starting node and launches the DFS.
- Upon entry, the current cell value is overwritten to 0 so it won't be revisited.
- A recursive 4-directional loop pushes into valid adjacent 1s, accumulating their returned areas.
- The returned value is `1 + sum(DFS(children))`, and a tracking variable `maxarea` retains the maximum computed value over all components.

**Complexity:** Time `O(r*c)`, Space `O(r*c)`

```cpp
    int dfs(int i,int j,vector<vector<int>>& grid,int r,int c){
        grid[i][j]=0;
        int dx[]={0,1,0,-1};
        int dy[]={1,0,-1,0};
        int area=1;
        for(int k=0;k<4;k++){
            int ni=i+dx[k];
            int nj=j+dy[k];
            if(ni>=0 && ni<r && nj>=0 && nj<c && grid[ni][nj]){
                area+=dfs(ni,nj,grid,r,c);
            }
        }
        return area;
    }
    
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        int r=grid.size(),c=grid[0].size();
        int maxarea=0;
        for(int i=0;i<grid.size();i++){
            for(int j=0;j<grid[0].size();j++){
                if(grid[i][j]){
                    maxarea=max(maxarea,dfs(i,j,grid,r,c));
                }
            }
        }
        return maxarea;
    }
```

---

## 721. Accounts Merge

**Q:** Given a list of `accounts` where the first string is the name and the rest are emails, merge accounts belonging to the same person based on shared emails and return them sorted.

**A:** Disjoint Set Union (DSU) to group account indices linked by common email addresses.

**Explanation:**
- An `unordered_map<string, int> mailNo` links each email explicitly to an initial account ID (0 to n-1).
- The nested loop processes all emails; if an email has already been mapped to an account ID, it unifies the current account ID with the previously mapped ID using `ds.unionNode`.
- A second pass aggregates all unique emails belonging to the same DSU parent node into a `mergedMail` array of vectors.
- Finally, each merged list is sorted alphabetically, the account name is prepended, and empty sets are discarded before returning.

**Complexity:** Time `O(N \log N + E \cdot alpha(N))` where `E` is total emails and `N` is accounts, Space `O(E + N)`

```cpp
class DS{
    vector<int> parent,size;
    public:
    DS(int n){
        parent=vector<int>(n);
        size=vector<int>(n);
        for(int i=0;i<n;i++){
            parent[i]=i;
            size[i]=1;
        }
    }
    int findParent(int node){
        if(node==parent[node]) return node;
        return parent[node]=findParent(parent[node]);
    }
    void unionNode(int u,int v){
        int p_u=findParent(u);
        int p_v=findParent(v);
        if(p_u!=p_v){
            if(size[p_u]>=size[p_v]){
                size[p_u]+=size[p_v];
                parent[p_v]=parent[p_u];
            }else{
                size[p_v]+=size[p_u];
                parent[p_u]=parent[p_v];
            }
        }
    }
};

class Solution {
public:
    vector<vector<string>> accountsMerge(vector<vector<string>>& accounts) {
        int n=accounts.size();
        DS ds(n);
        unordered_map<string,int> mailNo;
        for(int i=0;i<n;i++){
            for(int j=1;j<accounts[i].size();j++){
                string mail=accounts[i][j];
                if(mailNo.find(accounts[i][j])==mailNo.end()){
                    mailNo[mail]=i;
                }else{
                    ds.unionNode(i,mailNo[mail]);
                }
            }
        }

        vector<string> mergedMail[n];
        for(auto it : mailNo){
            string mail=it.first;
            int node=ds.findParent(it.second);
            mergedMail[node].push_back(mail);
        }

        vector<vector<string>> ans;
        for(int i=0;i<n;i++){
            if(mergedMail[i].size()==0) continue;
            vector<string> temp;
            temp.push_back(accounts[i][0]);
            sort(mergedMail[i].begin(),mergedMail[i].end());
            temp.insert(temp.end(),mergedMail[i].begin(),mergedMail[i].end());
            ans.push_back(temp);
        }
        return ans;
    }
};
```

---

## 743. Network Delay Time

**Q:** Given `n` nodes, an edge list `times` of directed edge delays, and a starting node `k`, find the minimum time needed for all nodes to receive the signal (or return -1 if impossible).

**A:** Dijkstra's Algorithm using a min-heap to find the longest shortest-path.

**Explanation:**
- An adjacency list represents edges `[u, v, w]`.
- A min-heap (`priority_queue`) tracks `(current_distance, node)` and expands outward from the starting node `k`, initialized at distance 0.
- `dis` array acts as the state tracker; if a shorter path to an adjacent node `v` is encountered (`dis[u] + w < dis[v]`), it replaces `dis[v]` and is queued.
- After exhaustion, the overall result is the maximum value in the `dis` array. If any node has `INT_MAX`, it signifies an unreachable component resulting in -1.

**Complexity:** Time `O(E \log V)`, Space `O(V+E)`

```cpp
// Using Dijasktra....
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<int>dis(n+1,INT_MAX);
        priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>>pq;
        vector<vector<int>>adj[n+1];
        for(int j=0;j<times.size();j++){
                int u=times[j][0];
                int v=times[j][1];
                int w=times[j][2];
                adj[u].push_back({v,w});
         }
         dis[k]=0;
         pq.push({0,k});
         while(!pq.empty()){
             int u=pq.top().second;
             pq.pop();
             for(auto vec:adj[u]){
                 int v=vec[0];
                 int w=vec[1];
                 if(dis[u]+w<dis[v]){
                    pq.push({dis[u]+w,v});
                    dis[v]=w+dis[u];
                 }
             }
         }
        
        int ans=0;
        for(int i=1;i<=n;i++){
            if(dis[i]==INT_MAX) return -1;
            ans=max(ans,dis[i]);
        }
        return ans;
     }
```

---

## 785. Is Graph Bipartite?

**Q:** Given an undirected adjacency list `graph`, verify whether the graph is bipartite (nodes can be partitioned into two independent sets with no intra-set edges).

**A:** Alternating two-color graph coloring utilizing BFS or DFS traversal on unvisited components.

**Explanation:**
- A `col` array is instantiated with -1 representing uncolored/unvisited nodes, while valid colors are 0 and 1.
- An outer loop sweeps across all `n` nodes because the graph might be disconnected, initiating BFS or DFS on any -1 node.
- During traversal, unvisited neighbors receive `1 - current_color` (the opposite color) and are pushed to the queue or stack.
- If a visited neighbor possesses the identical color as the current node, the bipartite property is violated and `false` is immediately propagated upwards.

**Complexity:** BFS Approach: Time `O(V+E)`, Space `O(V)`
DFS Approach: Time `O(V+E)`, Space `O(V)`

```cpp
// Using BFS...
    bool Bipartite(vector<vector<int>>& graph, vector<int>&col, int s){
        queue<int>q;
        col[s]=1;
        q.push(s);
        while(!q.empty()){
            int node=q.front();q.pop();
            for(auto it : graph[node]){
                if(col[it]==-1){
                    col[it]=1-col[node];
                    q.push(it);
                }else if(col[it]==col[node]) return false;
            }
        }
        return true;
    }
    bool isBipartite(vector<vector<int>>& graph) {
        int n=graph.size();
        vector<int> col(n,-1);
        for(int i=0;i<n;i++){
            if(col[i]==-1){
                if(!Bipartite(graph,col,i)) return false;
            }
        }
        return true;
    }
    
// Using DFS...
    bool Bipartite(vector<vector<int>>& graph, vector<int>&col, int s){
        for(auto it : graph[s]){
            if(col[it]==-1){
                col[it]=1-col[s];
                if(!Bipartite(graph,col,it)) return false; 
            }else if(col[it]==col[s]) return false;
        }
        return true;
    }
    bool isBipartite(vector<vector<int>>& graph) {
        int n=graph.size();
        vector<int> col(n,-1);
        for(int i=0;i<n;i++){
            if(col[i]==-1){
                col[i]=1;
                if(!Bipartite(graph,col,i)) return false;
            }
        }
        return true;
    }

```

---

## 827. Making A Large Island

**Q:** Given an `n` by `n` grid of 1s (land) and 0s (water), flip at most one 0 to 1 and return the size of the largest resulting island.

**A:** DFS painting island IDs followed by checking hypothetical unions over all water cells.

**Explanation:**
- The first pass finds all disjoint 1 components, replacing them with a unique ID `id` (>1) via DFS while an `unordered_map<int,int> mp` tallies the total size of each assigned `id`.
- The second pass loops exclusively over all original 0 cells to simulate flipping them.
- For a chosen 0, the total hypothetical island size begins at 1 and increases by summing the areas of 4-directionally adjacent unique IDs, employing an `unordered_set` to rigorously prevent double-counting.
- The global maximum observed during these flip simulations dictates the answer (falling back to `n*n` if no 0 existed).

**Complexity:** Time `O(n^2)`, Space `O(n^2)`

```cpp
    int dx[4]={0,0,1,-1};
    int dy[4]={1,-1,0,0};
    unordered_map<int,int> mp;
    int id=2;

    void dfs(int i,int j,vector<vector<int>> &grid){
        int n=grid.size();
        grid[i][j]=id;
        mp[id]++;
        for(int k=0;k<4;k++){
            int x=i+dx[k];
            int y=j+dy[k];
            if(x>=0 && x<n && y>=0 && y<n && grid[x][y]==1){
                dfs(x,y,grid);
            }
        }
    } 

    int largestIsland(vector<vector<int>>& grid) {
        int n=grid.size(),cnt0=0;
        if(n==0 || n==1) return 1;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(grid[i][j]==1){
                    dfs(i,j,grid);
                    id++;
                }else if(grid[i][j]==0) cnt0++;
            }
        }
        if(cnt0==0 || cnt0==1) return n*n;
        int ans=1;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(grid[i][j]==0){
                    int res=1;
                    unordered_set<int> s;
                    for(int k=0;k<4;k++){
                        int x=i+dx[k];
                        int y=j+dy[k];
                        if(x>=0 && x<n && y>=0 && y<n){
                            int gid=grid[x][y];
                            if(gid!=0 && s.find(gid)==s.end()){
                                res+=mp[gid];
                                s.insert(gid);
                            }
                        }
                    }
                    ans=max(ans,res);
                }
            }
        }
        return ans;
    }
```

---

## 886. Possible Bipartition

**Q:** Given `n` people and an undirected list `dislikes`, determine if it is possible to split them into two groups such that no two people in the same group dislike each other.

**A:** BFS Bipartite Graph Coloring algorithm using a group assignment array.

**Explanation:**
- The conflict graph is constructed where edges map to bidirectional dislikes.
- `grp` tracks the group assignment (-1 unassigned, 0 and 1 assigned).
- The outer loop triggers a BFS starting at every unassigned node to guarantee unconnected subgroups are fully mapped.
- When traversing, connected neighbors inherently get assigned `1 - current_group` to guarantee separation, returning `false` if conflict arises (neighbor holds the exact same group).

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
    bool possibleBipartition(int n, vector<vector<int>>& dislikes) {
        vector<int> adj[n+1];
        for(auto it : dislikes){
            adj[it[0]].push_back(it[1]);
            adj[it[1]].push_back(it[0]);
        }
        vector<int> grp(n+1,-1);
        for(int i=1;i<=n;i++){
            if(grp[i]==-1){
                queue<int> q;
                q.push(i);
                grp[i]=0;
                while(!q.empty()){
                    int node=q.front();q.pop();
                    for(auto it : adj[node]){
                        if(grp[it]==-1){
                            q.push(it);
                            grp[it]=1-grp[node];
                        }else if(grp[it]==grp[node]) return false;
                    }
                }
            }
        }
        return true;
    }
```

---

## 947. Most Stones Removed with Same Row or Column

**Q:** Given an array of `stones` coordinates, return the maximum number of stones that can be removed where a removal requires another unremoved stone to share the same row or column.

**A:** Disjoint Set Union (DSU) to coalesce coordinates where nodes represent distinct rows and columns.

**Explanation:**
- The graph considers rows and columns to be distinct vertices; an offset (`maxr+1`) prevents collisions between matching row and column integer IDs.
- For each stone, the algorithm connects (unions) its corresponding row coordinate and shifted column coordinate within the DSU structure.
- All involved coordinates are stored in an `unordered_set s` for later parent evaluation.
- The total distinct connected components (`grp`) is computed by checking if `ds.findP(it) == it`. The solution is simply the number of stones minus the number of separated connected components.

**Complexity:** Time `O(N * alpha(\max(R, C)))` where `N` is stones and `R`, `C` are max dimensions, Space `O(\max(R, C))`

```cpp
class DS{
    vector<int> parent,size;
    public:
    DS(int n){
        size.resize(n+1,1);
        parent.resize(n+1,0);
        for(int i=0;i<=n;i++) parent[i]=i;
    }
    int findP(int node){
        if(parent[node]==node) return node;
        return parent[node]=findP(parent[node]);
    }
    void unionN(int u,int v){
        int p_u=findP(u);
        int p_v=findP(v);
        if(p_u==p_v) return;
        if(size[p_u]>size[p_v]){
            parent[p_v]=p_u;
            size[p_u]+=size[p_v];
        }else{
            parent[p_u]=p_v;
            size[p_v]+=size[p_u];
        }
    }

};
class Solution {
public:
    int removeStones(vector<vector<int>>& stones) {
        int maxr=0,maxc=0;
        for(auto it : stones){
            maxr=max(maxr,it[0]);
            maxc=max(maxc,it[1]);
        }
        DS ds(maxr+maxc+1);
        unordered_set<int> s;
        for(auto it : stones){
            ds.unionN(it[0],it[1]+maxr+1);
            s.insert(it[0]);
            s.insert(it[1]+maxr+1);
        }
        int grp=0;
        for(auto it : s){
            if(ds.findP(it)==it) grp++;
        }
        return stones.size()-grp;
    }
};
```

---

## 994. Rotting Oranges

**Q:** Given an `r` by `c` grid where cells are 0 (empty), 1 (fresh), or 2 (rotten), find the minimum minutes until no fresh orange remains, or return -1 if impossible.

**A:** Multi-source level-order BFS tracking elapsed minutes while rotting adjacent oranges.

**Explanation:**
- An initial grid sweep queues all originally rotten oranges (`2`) and tallies all strictly `freash` oranges (`1`).
- The BFS progresses by level (minutes) by popping all current elements `q.size()` per iteration before incrementing `ans`.
- Unvisited fresh oranges matching boundary conditions are flipped directly to `2`, enqueued, and decrement the `freash` counter.
- The loop stops when the queue is exhausted, and if the `freash` counter remains above 0, it means some oranges were unreachable returning -1, else it returns `ans - 1`.

**Complexity:** Time `O(r*c)`, Space `O(r*c)`

```cpp
    int orangesRotting(vector<vector<int>>& grid) {
        int n=grid.size(),m=grid[0].size();
        queue<pair<int,int>> q;
        int freash=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j]==2) q.push({i,j});
                else if(grid[i][j]==1) freash++; 
            }
        }
        if(freash==0) return 0;
        int ans=0;
        while(!q.empty()){
            int len=q.size();
            for(int z=0;z<len;z++){
                int i=q.front().first;
                int j=q.front().second;
                q.pop();
                int dx[]={0,1,0,-1};
                int dy[]={1,0,-1,0};
                for(int k=0;k<4;k++){
                    int x=i+dx[k];
                    int y=j+dy[k];
                    if(x>=0 && x<n && y>=0 && y<m && grid[x][y]==1){
                        grid[x][y]=2;
                        q.push({x,y});
                        freash--;
                    }
                }
            }
            ans++;
        }
        if(freash>0) return -1;
        return ans-1;
    }
```

---

## 1. Alien Dictionary

**Q:** Given a sorted `dict` of strings representing words in an alien language with `K` distinct characters, return a valid string characterizing the alphabetical order.

**A:** Kahn's BFS Algorithm for Topological Sort on a directed character graph built from word pair mismatches.

**Explanation:**
- Sequential pairs of words evaluate characters up to their matching lengths; the first mismatch uncovers a definitive directed edge (`dict[i][j]` -> `dict[i+1][k]`) and breaks the inner loop.
- The `inDeg` array is built for the directed dependency relationships across the `K` letters.
- The BFS pops nodes with an indegree of 0 into the `ans` string while satisfying outgoing dependencies.
- This topological sort fundamentally serializes partial graph orders guaranteeing compliance with alien dictionary constraints.

**Complexity:** Time `O(C + V+E)` where `C` is sum of string lengths, Space `O(V+E)`

```cpp
    string topoSort(int V, unordered_map<char,vector<char>> &adj) {
	    string ans;
	    vector<int> inDeg(V,0);
	    for(int i=0;i<V;i++){
	        for(auto it : adj['a'+i]){
	            inDeg[it-'a']++;
	        }
	    }
	    
	    queue<char> q;
	    for(int i=0;i<V;i++){
	        if(inDeg[i]==0) q.push(i+'a');
	    }
	    
	    while(!q.empty()){
	        auto node=q.front();q.pop();
	        ans.push_back(node);
	        for(auto it : adj[node]){
	            inDeg[it-'a']--;
	            if(inDeg[it-'a']==0) q.push(it);
	        }
	    }
	    return ans;
	}
   string findOrder(string dict[], int N, int K) {
        unordered_map<char,vector<char>> mp;
        for(int i=0;i<N-1;i++){
            int j=0,k=0;
            while(j<dict[i].size() && k<dict[i+1].size()){
                if(dict[i][j]!=dict[i+1][k]){
                    mp[dict[i][j]].push_back(dict[i+1][k]);
                    break;
                }
                j++;k++;
            }
        }
        return topoSort(K,mp);
    }
    
    Link - https://practice.geeksforgeeks.org/problems/alien-dictionary/1/
```

---

## 2. Articulation Point - I

**Q:** Given `V` vertices and an undirected `adj` list, determine and return all articulation points (nodes that strictly increase the number of connected components if removed).

**A:** Tarjan's Articulation Point Algorithm using a single DFS updating discovery and lowest-reachable times.

**Explanation:**
- The standard recursive DFS is enhanced using `tin` and `low` arrays mimicking Tarjan's structure for bridges.
- During recursive descent into an unvisited neighbor, a specific test `low[it] >= tin[node] && parent != -1` triggers if a child has no path ascending above `node`, flagging it an articulation point in the `ap` array.
- The `timer` ensures monotonically increasing assignment of bounds avoiding false positive triggers.
- An edge case is handled explicitly: if the DFS initial node (the root) has strictly greater than one independent `child` branches, it inherently serves as an articulation point.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
    void dfs(int node,int parent,vector<int> adj[],vector<int> &vis,vector<int> &tin,vector<int> &low,int &timer,vector<int> &ap){
        vis[node]=1;
        tin[node]=low[node]=timer++;
        int child=0;
        for(auto it : adj[node]){
            if(it==parent) continue;
            if(!vis[it]){
                dfs(it,node,adj,vis,tin,low,timer,ap);
                low[node]=min(low[node],low[it]);
                if(low[it]>=tin[node] && parent!=-1) ap[node]=1;
                child++;
            }else{
                low[node]=min(low[node],tin[it]);
            }
        }
        if(parent==-1 && child>1) ap[node]=1;
    }
  
    vector<int> articulationPoints(int V, vector<int>adj[]) {
        vector<int> vis(V,0),tin(V),low(V),ap(V,0),ans;
        int timer=0;
        for(int i=0;i<V;i++){
            if(!vis[i]){
                dfs(i,-1,adj,vis,tin,low,timer,ap);
            }
        }
        for(int i=0;i<V;i++){
            if(ap[i]) ans.push_back(i);
        }
        if(ans.size()==0) return {-1};
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/articulation-point-1/0
```

---

## 3. BFS of graph

**Q:** Given `V` vertices and an adjacency list `adj` of a directed graph (starting from 0), return a list containing the BFS traversal from the 0-th vertex.

**A:** Standard iterative BFS employing a queue and a visited array.

**Explanation:**
- The graph defines connectivity directly through `adj`.
- The queue initializes tracking starting purely from vertex 0, immediately marked as `vis[0]=1` avoiding early cycles.
- While dequeuing sequentially into `ans`, any valid and unvisited neighbor vertices are enqueued and strictly marked `vis=1` to enforce short reachability logic.
- Due to the constraints and problem definition, early returning the single disconnected component mapping answers the requirement entirely.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
    vector<int> bfsOfGraph(int V, vector<int> adj[]) {
        vector<int> ans;
        vector<int> vis(V,0);
        for(int i=0;i<V;i++){
            if(!vis[i]){
                queue<int> q;
                q.push(i);
                vis[i]=1;
                while(!q.empty()){
                    int node=q.front();q.pop();
                    ans.push_back(node);
                    for(auto it : adj[node]){
                        if(!vis[it]){
                            vis[it]=1;
                            q.push(it);
                        }
                    }
                }
                return ans;
            }
        }
        
    }
    
    Link - https://practice.geeksforgeeks.org/problems/bfs-traversal-of-graph/1/
```

---

## 4. Bridge edge in a graph

**Q:** Given `V` vertices, an adjacency list `adj`, and two nodes `c` and `d`, return 1 if the specific undirected edge between `c` and `d` is a bridge, and 0 otherwise.

**A:** Tarjan's Algorithm for Bridge Finding explicitly targeting only a specific edge constraint during DFS.

**Explanation:**
- It tracks insertion time `tin` and lowest reachable time `l` alongside `vis`.
- The recursive DFS systematically parses adjacent nodes.
- Upon post-evaluation of child components, if `l[i] > tin[node]` it evaluates if the current edge endpoints strictly match `(c, d)` or `(d, c)`.
- If an exact match is discovered under this strict no-back-edge condition, the function propagates 1 upwards indicating the exact target is a bridge.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
    int dfs(int node,int parent,vector<int>&vis,vector<int>&tin,vector<int>&l,int time,vector<int>adj[],int c,int d){
        vis[node]=1;
        tin[node]=l[node]=time;
        time++;
        for(auto &i:adj[node]){
            if(i==parent) continue;
            if(!vis[i]){
                if(dfs(i,node,vis,tin,l,time,adj,c,d)) return 1;
                l[node]=min(l[node],l[i]);
                if(l[i]>tin[node]){
                    if((node==c and i==d) or(node==d and i==c)) return 1; 
                }
            }
            else{
                l[node]=min(l[node],l[i]);
            }
        }
        return 0;
    }
    int isBridge(int V, vector<int> adj[], int c, int d) {
        vector<int>vis(V,0),tin(V,0),l(V,0);
        for(int i=0;i<V;i++){
            if(dfs(i,-1,vis,tin,l,1,adj,c,d)) return 1;
        }
       return 0;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/bridge-edge-in-graph/1/
```

---

## 5. Cheapest Flights Within K Stops

**Q:** Given `n` cities connected by `flights`, find the cheapest price from `src` to `dst` strictly involving at most `K` stops.

**A:** BFS exploring paths strictly constrained by levels (`stops`) allowing multiple visits if costs are optimized.

**Explanation:**
- The graph maps directed paths `[u, v, cost]` using an adjacency structure.
- The BFS is driven by a standard queue mapping `(stops, (current_node, current_cost))` prioritizing stops iteration over pure distance minimization.
- Distance array `dis` permits queued pushing merely if the incoming cost betters previously known values given the edge respects `step <= K`.
- Modulating purely upon distances and filtering strictly upon `K` stops naturally captures constrained minimizations directly bypassing rigid heap approaches.

**Complexity:** Time `O(K \cdot E)`, Space `O(V+E)`

```cpp
    int CheapestFLight(int n, vector<vector<int>>& flights, int src, int dst, int K)  {
        vector<vector<int>> adj[n];
        for(auto it : flights){
            adj[it[0]].push_back({it[1],it[2]});
        }
        queue<pair<int,pair<int,int>> >q;
        q.push({0,{src,0}});
        vector<int> dis(n,1e9);
        dis[src]=0;
        while(!q.empty()){
            auto ft=q.front();q.pop();
            int step=ft.first;
            int node=ft.second.first;
            int dist=ft.second.second;
            for(auto it : adj[node]){
                int next=it[0];
                int nextd=it[1];
                if(dis[next]>dist+nextd && step<=K){
                    dis[next]=dist+nextd;
                    q.push({step+1,{next,dis[next]}});
                }
            }
        }
        if(dis[dst]!=1e9) return dis[dst];
        return -1;
    }

gfg link - https://www.geeksforgeeks.org/problems/cheapest-flights-within-k-stops/1
```

---

## 6. Check if a given graph is tree or not

**Q:** Given `n` vertices and an undirected edge list `v` size `m`, establish if the structural relationships classify cleanly as a tree.

**A:** Edge tally validation compounded with a comprehensive BFS asserting zero cyclical structures and single-component reachability.

**Explanation:**
- To formally exist as a tree, a graph is mandated to hold precisely `n-1` edges natively skipping complex evaluations entirely if violated.
- A standard BFS `detectCycle` function incorporates parent tracking `q.push({node, parent})` determining potential cyclic references instantly if a visited state is re-encountered excluding the parent.
- If cycle parsing successfully yields `false`, an overarching verification loop verifies the initial BFS strictly mapped `vis[i] == 1` identifying unreachability correctly discarding disjoint collections.
- Merging cycle negation plus monolithic reachability outputs `1` for tree alignment, conversely returning `0`.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
    bool detectCycle(vector<int> &vis, vector<int> adj[]) {
        queue<pair<int,int>> q;
        vis[1] = 1;
        q.push({1, -1});
        while(!q.empty()){
            int n = q.size();
            for(int i=0; i<n; i++){
                auto it = q.front();q.pop();
                for(auto x: adj[it.first]){
                    if(x != it.second){
                        if(vis[x]) return true;
                        else {
                            q.push({x, it.first});
                            vis[x] = 1;
                        }
                    }
                }
            }
        }
        return false;
    }
  
    int isTree(int n, int m, vector<vector<int>> &v) {
        if(n==1 && m==0) return 1;
        if(m!=(n-1)) return 0;
        vector<int> vis(n),adj[n];
        for(int i=0; i<m; i++){
            adj[v[i][0]].push_back(v[i][1]);
            adj[v[i][1]].push_back(v[i][0]);
        }
        if(detectCycle(vis, adj)) return 0;
        for(int i=0; i<n; i++) if(!vis[i]) return 0;
        return 1;
    }

gfg link - https://www.geeksforgeeks.org/problems/is-it-a-tree/1
```

---

## 7. Code Testcase Test Result Test Result 3341. Find Minimum Time to Reach Last Room I

**Q:** Given an `r` by `c` grid `moveTime` indicating when a cell becomes accessible, determine the minimum time to travel from `(0, 0)` to `(r-1, c-1)` moving 4-directionally taking exactly 1 unit of time per move.

**A:** Dijkstra's algorithm applied to a matrix treating temporal constraints as monotonically increasing weights.

**Explanation:**
- Time delays establish weights enforcing minimum access boundaries via `max(moveTime[x][y], curTime) + 1`.
- A min-heap orders BFS-traversal utilizing structurally constrained elements `(currentTime, i, j)`.
- Updates strictly push configurations to the priority queue conditionally whenever `nextTime < dis[x][y]`, mapping shortest paths correctly globally.
- The earliest extracted component arriving at `(n-1, m-1)` intrinsically yields the minimum arrival time, returning it immediately.

**Complexity:** Time `O(r*c \log (r*c))`, Space `O(r*c)`

```cpp
    int minTimeToReach(vector<vector<int>>& moveTime) {
        int n=moveTime.size(),m=moveTime[0].size();
        vector<vector<int>> dis(n,vector<int>(m,INT_MAX));
        priority_queue<vector<int>,vector<vector<int>>,greater<vector<int>>> minH;
        minH.push({0,0,0});
        moveTime[0][0]=0;
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0,};
        while(!minH.empty()){
            auto it = minH.top();minH.pop();
            int curTime=it[0];
            int i=it[1],j=it[2];
            if(i==n-1 && j==m-1) return curTime;
            for(int k=0;k<4;k++){
                int x=i+dx[k];
                int y=j+dy[k];
                if(x>=0 && x<n && y>=0 && y<m){
                    int nextTime=max(moveTime[x][y],curTime)+1;
                    if(dis[x][y]>nextTime) {
                        dis[x][y]=nextTime;
                        minH.push({nextTime,x,y});
                    }
                }
            }
        }
        return -1;
    }
```

---

## 8. Connecting the graph

**Q:** Given `n` nodes and an array of `edge` relations, determine the minimum redundant edge movements required to entirely link the structural components.

**A:** Disjoint Set Union (DSU) verifying total disconnected graphs against dynamically counted redundant connections.

**Explanation:**
- Incorporates `findParent` and `unionNodes` using rank aggregation ensuring flat structures yielding fast tree lookups.
- During mapping iterations, duplicated root interactions (where `pu == pv`) directly tally into redundant edges `nre` rather than performing union assignments.
- A final node traversal enumerates isolated root elements mapping entirely independent networks assigned to `ncc`.
- Given exactly `ncc - 1` edges uniformly bridge `ncc` networks, a validation strictly returns `ncc - 1` assuming `nre >= ncc - 1` holds true, otherwise answering `-1`.

**Complexity:** Time `O(E * alpha(V))`, Space `O(V)`

```cpp
class DisjointSet{
    public:
        vector<int> parent,size;
        DisjointSet(int n){
            size.resize(n,1);
            parent.resize(n);
            for(int i=0;i<n;i++) parent[i]=i;
        }
        int findParent(int u){
            if(u==parent[u]) return u;
            return parent[u]=findParent(parent[u]);
        }
        void unionNodes(int u,int v){
            int pu=findParent(u);
            int pv=findParent(v);
            if(pu==pv) return;
            if(size[pu]>size[pv]){
                parent[pv]=pu;
                size[pu]+=size[pv];
            }else{
                parent[pu]=pv;
                size[pv]+=size[pu];
            }
        }
};
class Solution {
  public:
    int Solve(int n, vector<vector<int>>& edge) {
        DisjointSet ds(n);
        int nre=0;
        for(auto it : edge){
            int pu=ds.findParent(it[0]);
            int pv=ds.findParent(it[1]);
            if(pu==pv) nre++;
            else ds.unionNodes(pu,pv);
        }
        int ncc=0;
        for(int i=0;i<n;i++){
            if(ds.parent[i]==i) ncc++;
        }
        if(nre>=ncc-1) return ncc-1;
        else return -1;
    }
};

gfg link - https://www.geeksforgeeks.org/problems/connecting-the-graph/1
```

---

## 9. Detect cycle in a directed graph

**Q:** Given `V` vertices and an adjacency list `adj` for a directed graph, return `true` if it contains a cycle, else `false`.

**A:** Topo Sort (BFS) using Kahn's algorithm or DFS with a recursion-stack array.

**Explanation:**
- BFS: Calculate in-degrees for all nodes, push nodes with 0 in-degree to a queue, and pop while decrementing neighbors' in-degrees (pushing them if they hit 0). If the total popped nodes count isn't `V`, a cycle exists.
- DFS: Maintain a `vis` array for overall visited nodes and a `dfsVis` array for nodes in the current recursion stack.
- DFS: If an adjacent node is already marked in `dfsVis`, a back edge is found, meaning a cycle exists.
- DFS: Backtrack by clearing the current node from `dfsVis` before returning from the recursive call.

**Complexity:** BFS/DFS: Time `O(V+E)`, Space `O(V)`

```cpp
// Using BFS(Like Topo Sort)...
    bool isCyclic(int V, vector<int> adj[]) {
        vector<int> inDeg(V,0);
        for(int i=0;i<V;i++){
            for(auto it : adj[i]) inDeg[it]++;
        }
        
        queue<int> q;
        for(int i=0;i<V;i++){
            if(inDeg[i]==0) q.push(i);
        }
        
        int cnt=0;
        while(!q.empty()){
            int node=q.front();q.pop();
            cnt++;
            for(auto it : adj[node]){
                inDeg[it]--;
                if(inDeg[it]==0) q.push(it);
            }
        }
        return !(cnt==V);
    }

// Using DFS...
    bool dfs(int node,vector<int> adj[],vector<int> &vis,vector<int> &dfsVis){
        vis[node]=1;
        dfsVis[node]=1;
        for(auto it : adj[node]){
            if(!vis[it]){
                // vis[it]=1;
                // dfsVis[it]=1;
                if(dfs(it,adj,vis,dfsVis)) return true;
            }else if(dfsVis[it]) return true;
        }
        dfsVis[node]=0;  //**Backtrack...
        return false;
    }
    bool isCyclic(int V, vector<int> adj[]) {
        vector<int> vis(V,0);
        vector<int> dfsVis(V,0);
        for(int i=0;i<V;i++){
            if(!vis[i]){
                // vis[i]=1;
                // dfsVis[i]=1;
                if(dfs(i,adj,vis,dfsVis)) return true; 
            }
        }
        return false;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/detect-cycle-in-a-directed-graph/1/
```

---

## 10. Detect cycle in an undirected graph

**Q:** Given `V` vertices and an adjacency list `adj` for an undirected graph, return `true` if a cycle exists, else `false`.

**A:** BFS or DFS tracking the parent (previous) node to distinguish bidirectional edges from real cycles.

**Explanation:**
- BFS: Use a queue of pairs `{node, parent_node}` and a visited array `vis`.
- BFS: For each neighbor, if it's already visited and isn't the `parent_node`, a cycle exists.
- DFS: Pass `pre` (the parent node) in the recursive call along with the current node.
- DFS: If an adjacent node is visited and `it != pre`, a back edge forms a cycle.

**Complexity:** BFS/DFS: Time `O(V+E)`, Space `O(V)`

```cpp
//Using BFS...
  bool checkCycle(int i,vector<int> adj[],vector<int> &vis){
        queue<pair<int,int>> q;
        vis[i]=1;
        q.push({i,-1});
        while(!q.empty()){
            int node=q.front().first;
            int pre=q.front().second;
            q.pop();
            for(auto it : adj[node]){
                if(!vis[it]){
                    vis[it]=1;
                    q.push({it,node});
                }
                else if(pre!=it){
                    return true;
                }
            }
        }
        return false;
    }
    bool isCycle(int V, vector<int> adj[]) {
        vector<int> vis(V,0);
        for(int i=0;i<V;i++){
            if(!vis[i]){
                if(checkCycle(i,adj,vis)){
                    return true;
                }
            }
        }
        return false;
    }

//Using DFS...
    bool checkCycle(int i,int pre,vector<int> adj[],vector<int> &vis){
        vis[i]=1;
        for(auto it : adj[i]){
            if(!vis[it]){
                if(checkCycle(it,i,adj,vis)) return true;;
            }else if(it!=pre) return true;
        }
        return false;
    }
    bool isCycle(int V, vector<int> adj[]) {
        vector<int> vis(V,0);
        for(int i=0;i<V;i++){
            if(!vis[i]){
                if(checkCycle(i,-1,adj,vis)){
                    return true;
                }
            }
        }
        return false;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/detect-cycle-in-an-undirected-graph/1/
```

---

## 11. DFS of Graph

**Q:** Given `V` vertices and an adjacency list `adj`, return a list containing the DFS traversal of the graph starting from vertex 0.

**A:** Recursive DFS using a visited array to track explored nodes.

**Explanation:**
- Maintain a boolean `vis` array so that each node is pushed to the traversal list exactly once.
- Push the current node into the `ans` list and immediately mark it as visited.
- Iterate over the adjacency list of the current node and make a recursive call for any unvisited neighbors.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
    void dfs(int V, vector<int> adj[],vector<bool> &vis,vector<int> &ans){
        vis[V]=true;
        ans.push_back(V);
        for(auto it : adj[V]){
            if(!vis[it]){
                dfs(it,adj,vis,ans);
            }
        }
    }
    vector<int> dfsOfGraph(int V, vector<int> adj[]) {
        vector<int> ans;
        vector<bool> vis(V,false);
        for(int i=0;i<V;i++){
            if(!vis[i]){
                dfs(i,adj,vis,ans);
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/depth-first-traversal-for-a-graph/1/
```

---

## 12. Dijkstra's Algorithm

**Q:** Given `V` vertices, an adjacency list `adj` with edge weights, and a source vertex `S`, find the shortest distance from `S` to all other vertices.

**A:** Dijkstra's Algorithm using a min-heap (priority queue).

**Explanation:**
- Initialize a distance array `ans` with `INT_MAX` for all nodes, except `0` for the source node `S`.
- Use a min `priority_queue` containing pairs of `{distance, node}`, initially pushing `{0, S}`.
- Pop the node with the minimum distance, traverse its neighbors, and if `current_distance + edge_weight < neighbor_distance`, update the neighbor's distance and push `{new_distance, neighbor}` to the queue.

**Complexity:** Time `O(E log V)`, Space `O(V)`

```cpp
    vector <int> dijkstra(int V, vector<vector<int>> adj[], int S){
       priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>> > pq;
        pq.push({0,S});
        vector<int> ans(V,INT_MAX);
        ans[S]=0;
        while(!pq.empty()){
            int dist=pq.top().first;
            int node=pq.top().second;
            pq.pop();
            
            for(auto it : adj[node]){
                int next=it[0];
                int nextd=it[1];
                if(ans[next]>dist+nextd){
                    ans[next]=dist+nextd;
                    pq.push({ans[next],next});
                }
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/implementing-dijkstra-set-1-adjacency-matrix/1#
```

---

## 13. Distance of nearest cell having 1

**Q:** Given an `r` x `c` binary `grid`, return a grid of the same size containing the distance of the nearest `1` for each cell.

**A:** Multi-source Breadth-First Search (BFS) starting from all `1`s.

**Explanation:**
- Initialize an `ans` matrix with `1e9` (infinity) and a queue for BFS.
- Enqueue all cell coordinates `(i, j)` where `grid[i][j] == 1` and set their distance in `ans` to `0`.
- Perform BFS using a 4-directional array (`dx`, `dy`), updating the distance of valid neighbors to `1 + ans[r][c]` if it improves their current minimum distance, and enqueue them.

**Complexity:** Time `O(r*c)`, Space `O(r*c)`

```cpp
vector<vector<int>>nearest(vector<vector<int>>grid){
            int n=grid.size(),m=grid[0].size();
            vector<vector<int>> ans(n,vector<int>(m,1e9));
            queue<pair<int,int>> q;
            for(int i=0;i<n;i++){
                for(int j=0;j<m;j++){
                    if(grid[i][j]==1){ 
                        ans[i][j]=0;
                        q.push({i,j});
                    }
                }
            }
            int dx[]={0,1,0,-1};
      int dy[]={1,0,-1,0};
            while(!q.empty()){
                int r=q.front().first;
                int c=q.front().second;
                q.pop();
                
                for(int k=0;k<4;k++){
                    int x=r+dx[k];
                    int y=c+dy[k];
                    if(x>=0 && y>=0 && x<n && y<m && ans[x][y]>1+ans[r][c]){
                        ans[x][y]=1+ans[r][c];
                        q.push({x,y});
                    }
                }
            }
            return ans;
        }
  
  Link - https://practice.geeksforgeeks.org/problems/distance-of-nearest-cell-having-1-1587115620/1
```

---

## 14. Eventual Safe States

**Q:** Given `V` vertices and a directed graph `adj`, return a sorted list of all eventually safe nodes (nodes that only lead to terminal nodes).

**A:** Kahn's Algorithm (Topological Sort) on the reversed graph.

**Explanation:**
- Reverse all edges of the graph into a new adjacency list `Radj` and compute the out-degrees of nodes in the original graph (which acts as in-degrees for the reversed graph).
- Push all terminal nodes (in-degree 0 in the reversed graph) into a queue.
- Process the queue, adding each popped node to the safe states list. Decrement the in-degree of its neighbors in `Radj` and enqueue them when they reach 0. Finally, sort the answer.

**Complexity:** Time `O(V+E + V log V)` for sorting, Space `O(V+E)`

```cpp
    vector<int> eventualSafeNodes(int V, vector<int> adj[]) {
        vector<int> ans;
        vector<int> Radj[V];
        vector<int> indeg(V,0);
        for(int i=0;i<V;i++){
            for(auto it : adj[i]){
                Radj[it].push_back(i);
                indeg[i]++;
            }
        }
        queue<int> q;
        for(int i=0;i<V;i++){
            if(indeg[i]==0) q.push(i);
        }
        while(!q.empty()){
            int cur=q.front();q.pop();
            ans.push_back(cur);
            for(auto it : Radj[cur]){
                indeg[it]--;
                if(indeg[it]==0) q.push(it);
            }
        }
        sort(ans.begin(),ans.end());
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/eventual-safe-states/1
```

---

## 15. Find the string in grid

**Q:** Given an `r` x `c` 2D `grid` of characters and a `word`, find all coordinates where the word starts and can be formed in any of the 8 directions.

**A:** Iterative directional search from every cell matching the first character.

**Explanation:**
- Iterate over every cell in the grid. If the cell matches `word[0]`, check all 8 directional arrays (`x`, `y`).
- For a chosen direction, loop through the characters of `word` and check if the consecutive cells in that direction match.
- If all characters match without going out of bounds, push the starting coordinate `{i, j}` to the answer and immediately stop checking other directions for this cell.

**Complexity:** Time `O(r*c*8*len)`, Space `O(1)` excluding output

```cpp
          int x[8]={-1,-1,-1,0,0,1,1,1};
    int y[8]={-1,0,1,-1,1,-1,0,1};
    bool search2D(vector<vector<char>> grid, int n,int m,string word, int i, int j){
        if(grid[i][j]!=word[0]) return false;
        int len=word.length();
        for(int dir=0;dir<8;dir++){
            int k,rd=i+x[dir],cd=j+y[dir];
            for(k=1;k<len;k++){
               if(rd>=n||cd>=m||cd<0||rd<0)break;
               if(grid[rd][cd]!=word[k])break;
               rd+=x[dir];
               cd+=y[dir];
            }
            if(k==len) return true;
        }
       return false;
    }
    
    vector<vector<int>>searchWord(vector<vector<char>>grid, string word){
        int n=grid.size(),m=grid[0].size();
        vector<vector<int>>ans;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(search2D(grid,n,m,word,i,j)){
                   ans.push_back({i,j});
                }
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/find-the-string-in-grid0111/1/
```

---

## 16. Find whether path exist

**Q:** Given an `n` x `n` `grid` where `1` is source, `2` is destination, `3` is blank, and `0` is a wall, return `true` if a path exists from source to destination.

**A:** Depth-First Search (DFS) exploring 4 directions from the source cell.

**Explanation:**
- Locate the source cell (value `1`) to initiate the DFS.
- In DFS, mark the current cell as visited by changing its value to `0` (wall) in-place to prevent revisiting.
- Explore 4-directionally; return `true` if any neighbor is `2` (the destination), or if the recursive DFS call on a blank cell (value `3` or `1`) returns `true`.

**Complexity:** Time `O(n^2)`, Space `O(n^2)` for recursion stack

```cpp
    int dx[4]={0,1,0,-1};
    int dy[4]={1,0,-1,0};
    bool solve(int i,int j,int n,vector<vector<int>> &grid){
        grid[i][j]=0;
        for(int k=0;k<4;k++){
            int x=i+dx[k];
            int y=j+dy[k];
            if(x<0 || y<0 || x>=n || y>=n || grid[x][y]==0) continue;
            if(grid[x][y]==2) return true;
            if(solve(x,y,n,grid)) return true;
        }
        return false;
    }
    bool is_Possible(vector<vector<int>>& grid) {
        int n=grid.size();
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(grid[i][j]==1){
                    if(solve(i,j,n,grid)) return true;
                }
            }
        }
        return false;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/find-whether-path-exist5238/1/
```

---

## 17. Flood fill Algorithm

**Q:** Given an `r` x `c` 2D array `image`, start coordinates `sr` and `sc`, and a `newColor`, change the color of the connected component to `newColor`.

**A:** Depth-First Search (DFS) replacing the original color while tracking visited cells.

**Explanation:**
- Save the initial color `ic = image[sr][sc]` to know which cells belong to the connected component.
- Use a `vis` array to track changed cells and avoid infinite recursion.
- In DFS, if the cell is out of bounds, already visited, or its color is not `ic`, return. Otherwise, update its color to `newColor`, mark as visited, and recurse 4-directionally.

**Complexity:** Time `O(r*c)`, Space `O(r*c)` for visited array and recursion stack

```cpp
    int X[4]={1,-1,0,0};
    int Y[4]={0,0,-1,1};
    void flood_helper(vector<vector<int>>& image,int i, int j, int newColor, int ic,int n, int m,vector<vector<bool>>& vis){
        if(i<0 || i>=n || j<0 || j>=m || image[i][j]!=ic || vis[i][j]) return;
        image[i][j]=newColor;
        vis[i][j]=true;
        for(int k=0;k<4;k++){
            flood_helper(image,i+X[k],j+Y[k],newColor,ic,n,m,vis);
        }
    }
    
    vector<vector<int>> floodFill(vector<vector<int>>& image, int sr, int sc, int newColor) {
        int n=image.size();
        int m=image[0].size();
        int ic=image[sr][sc];
        vector<vector<bool>> vis(n,vector<bool>(m,false));
        flood_helper(image,sr,sc,newColor,ic,n,m,vis);
        return image;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/flood-fill-algorithm1856/1/
```

---

## 18. Floyd Warshall

**Q:** Given an `V` x `V` adjacency `matrix` where `-1` denotes no edge, find the shortest path between all pairs of vertices in-place.

**A:** Floyd-Warshall Algorithm using three nested loops.

**Explanation:**
- Iterate through all possible intermediate vertices `k` from `0` to `n-1`.
- For each intermediate vertex `k`, iterate through all source `i` and destination `j` pairs.
- If a path exists from `i` to `k` and `k` to `j`, update `matrix[i][j]` to the minimum of its current value and the path length through `k`.

**Complexity:** Time `O(V^3)`, Space `O(1)` (modifies matrix in-place)

```cpp
void shortest_distance(vector<vector<int>>&matrix){
            int n = matrix.size();
            for(int k=0;k<n;k++){
                for(int i=0;i<n;i++){
                    for(int j=0;j<n;j++){
                        if(matrix[i][k]==-1 or matrix[k][j]==-1)continue;
                        if(matrix[i][j]==-1) matrix[i][j] = matrix[i][k] + matrix[k][j];
                        else matrix[i][j] = min(matrix[i][j],matrix[i][k] + matrix[k][j]);
                    }
                }
            }
        }
  
  Link - https://practice.geeksforgeeks.org/problems/implementing-floyd-warshall2042/1#
```

---

## 19. Given a source and distance, in a 2d array containing blocks which breaks if we reach adjacent to it twice, return true if there exists a path

**Q:** Given an `n` x `m` 2D `grid` where `0` is free space and `1` is a block that breaks after being touched adjacently twice, return `true` if a path exists from `src` to `dst`.

**A:** BFS with an adjacency touch counter for blocks.

**Explanation:**
- Use a queue initialized with the `src` cell, a `visited` matrix for free spaces, and an `adjCount` matrix for tracking touches on blocks.
- Traverse 4-directionally; if a neighbor is `0` and unvisited, mark it visited and enqueue it.
- If a neighbor is `1` (a block), increment its `adjCount`; if the count reaches 2, convert it to a `0` (free space), mark visited, and enqueue it.

**Complexity:** Time `O(n*m)`, Space `O(n*m)`

```cpp

struct Cell {
    int x, y;
};

bool isValid(int x, int y, int n, int m) {
    return x >= 0 && y >= 0 && x < n && y < m;
}

bool existsPath(vector<vector<int>>& grid, pair<int,int> src, pair<int,int> dst) {
    int n = grid.size();
    int m = grid[0].size();

    vector<vector<int>> visited(n, vector<int>(m, 0));
    vector<vector<int>> adjCount(n, vector<int>(m, 0)); // count adjacency touches

    queue<Cell> q;
    q.push({src.first, src.second});
    visited[src.first][src.second] = 1;

    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    while (!q.empty()) {
        auto cur = q.front(); q.pop();

        if (cur.x == dst.first && cur.y == dst.second) {
            return true;
        }

        for (int k = 0; k < 4; k++) {
            int nx = cur.x + dx[k];
            int ny = cur.y + dy[k];

            if (!isValid(nx, ny, n, m)) continue;

            if (grid[nx][ny] == 0 && !visited[nx][ny]) {
                // Normal free cell
                visited[nx][ny] = 1;
                q.push({nx, ny});
            } 
            else if (grid[nx][ny] == 1) {
                // It's a block → increase adjacency count
                adjCount[nx][ny]++;
                if (adjCount[nx][ny] >= 2) {
                    // Block breaks, treat as free
                    grid[nx][ny] = 0;
                    visited[nx][ny] = 1;
                    q.push({nx, ny});
                }
            }
        }
    }
    return false;
}
```

---

## 20. Kruskal's Algo (Minimum Spanning Tree)

**Q:** Given `V` vertices and an adjacency list `adj` for an undirected graph, find the sum of weights of edges in the Minimum Spanning Tree (MST).

**A:** Kruskal's Algorithm using Disjoint Set Union (DSU) with path compression and union by rank.

**Explanation:**
- Convert the adjacency list into a flat list of edges `{weight, u, v}` and sort them in ascending order by weight.
- Initialize DSU with a `parent` array pointing to each node itself and a `rank` array set to 0.
- Iterate through the sorted edges; if the two vertices belong to different sets (checked via `findParent`), add the weight to the MST sum and merge their sets with `Union`.

**Complexity:** Time `O(E log E)`, Space `O(V + E)` for the edges list and DSU structures

```cpp
//Kruskal's Algo...
    int findParent(int node,vector<int> &parent){
        if(node==parent[node]) return node;
        return parent[node]=findParent(parent[node],parent);
    }
    void Union(int x,int y,vector<int> &parent,vector<int> &rank){
        x=findParent(x,parent);
        y=findParent(y,parent);
        if(rank[x]>rank[y]){
            parent[y]=x;
        }else if(rank[x]<rank[y]){
            parent[x]=y;
        }else{
            parent[y]=x;
            rank[x]++;
        }
    }
    int spanningTree(int V, vector<vector<int>> adj[]){
        vector<int> parent(V),rank(V,0);
        for(int i=0;i<V;i++){
            parent[i]=i;
        }
        
        vector<vector<int>> edges;
        for(int i=0;i<V;i++){
           for(auto it : adj[i]){
               edges.push_back({it[1],i,it[0]});
           }
        }
        sort(edges.begin(),edges.end());
        int ans=0;
        for(auto it : edges){
            if(findParent(it[1],parent)!=findParent(it[2],parent)){
                ans+=it[0];
                Union(it[1],it[2],parent,rank);
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/minimum-spanning-tree/1/#
```

---

## 21. Minimum Cost Path

**Q:** Given an `n` x `n` `grid` with cell costs, find the minimum cost to reach the bottom-right cell `(n-1, n-1)` from the top-left cell `(0, 0)`, moving in 4 directions.

**A:** Dijkstra's Algorithm on a 2D grid using a min priority queue.

**Explanation:**
- Use a `cost` matrix initialized to `INT_MAX` and a boolean `vis` matrix to track visited states.
- Initialize the priority queue with `{grid[0][0], {0, 0}}` and set `cost[0][0] = grid[0][0]`.
- Pop the cell with the smallest accumulated cost, mark it visited, and for its 4 valid unvisited neighbors, if `current_cost + grid[neighbor]` improves its best known cost, update `cost` and push it to the queue.

**Complexity:** Time `O(n^2 log(n^2))`, Space `O(n^2)`

```cpp
    int row[4] = {0, -1, 0, 1}, col[4] = {-1, 0, 1, 0};
    int minimumCostPath(vector<vector<int>>& grid){
        int n=grid.size();
        priority_queue<pair<int,pair<int,int>>,vector<pair<int,pair<int,int>>>, greater<pair<int,pair<int,int>>>> pq;
        vector<vector<int>> cost(n,vector<int>(n, INT_MAX));
        vector<vector<bool>> vis(n,vector<bool>(n,false));
        cost[0][0] = grid[0][0];
        pq.push({grid[0][0], {0,0}});
        while(!pq.empty()){
             int val = pq.top().first;
             pair<int,int> it = pq.top().second;
             int x = it.first, y = it.second;
             pq.pop();
             vis[x][y] = true;
             for(int k=0;k<4;k++){
                 int r = x+row[k], c = y+col[k];
                 if(r>=0 && c>=0 && r<n && c<n && !vis[r][c]){
                    if(cost[r][c] > val+grid[r][c]){
                        cost[r][c] = val+grid[r][c];
                        pq.push({cost[r][c], {r,c}});
                    }
                }
            }
        }
        return cost[n-1][n-1];
    }
    
    Link - https://practice.geeksforgeeks.org/problems/minimum-cost-path3833/1
```

---

## 22. Minimum Multiplications to reach End

**Q:** Given an array `arr`, a `start` value, and an `end` value, find the minimum number of multiplications (modulo 100000) required to reach `end` from `start` by repeatedly multiplying with elements from `arr`.

**A:** Breadth-First Search (BFS) on a state space graph representing values modulo 100000.

**Explanation:**
- Use a queue containing `{current_value, steps}` and a `steps` array of size 100000 initialized to `1e9` to track the shortest path to each specific modulo remainder.
- If the popped `current_value` equals `end`, return the steps immediately.
- Otherwise, for each multiplier in `arr`, calculate the next state as `(current_value * multiplier) % 100000`. If this next state hasn't been reached faster before, update its `steps` entry and enqueue it.

**Complexity:** Time `O(100000 * N)` where `N` is the size of `arr`, Space `O(100000)`

```cpp
   int minimumMultiplications(vector<int>& arr, int start, int end) {
        queue<pair<int,int>> q;
        vector<int> steps(100000,1e9);
        q.push({start,0});
        steps[start]=0;
        while(!q.empty()){
            int node=q.front().first;
            int step=q.front().second;
            q.pop();
            if(node==end) return step;
            for(auto it : arr){
                int newn=(node*it)%100000;
                if(steps[newn]>1+step){
                    steps[newn]=1+step;
                    q.push({newn,1+step});
                }
            }
        }
        return -1;
    }
gfg link - https://www.geeksforgeeks.org/problems/minimum-multiplications-to-reach-end/1
```

---

## 23. Negative weight cycle (Bellman Ford Algo)

**Q:** Given `N` vertices and a list of directed edges `adj` containing `{u, v, weight}`, determine if the graph contains a negative weight cycle.

**A:** Bellman-Ford Algorithm.

**Explanation:**
- Initialize a `dist` array of size `N` with infinity (`1e8`) and set `dist[0] = 0` (though reaching a disconnected cycle requires checking from all components or a dummy source).
- Relax all edges exactly `N - 1` times: for each edge `u -> v`, if `dist[u] + weight < dist[v]`, update `dist[v]`.
- Iterate through all edges one final time. If any edge can still be relaxed, it indicates the presence of a negative weight cycle, so return `true`.

**Complexity:** Time `O(N*E)`, Space `O(N)`

```cpp
int isNegativeWeightCycle(int N, vector<vector<int>>adj){
            // Code here
            int inf = 1e8; 
        vector<int> dist(N, inf); 
        dist[0] = 0; 
        for(int i = 1;i<=N-1;i++) {
            for(auto it: adj) {
                if(dist[it[0]] + it[2] < dist[it[1]]) {
                    dist[it[1]] = dist[it[0]] + it[2]; 
                }
            }
        }
    
        int fl = 0; 
        for(auto it: adj) {
            if(dist[it[0]] + it[2] < dist[it[1]]) {
                // cout << -1; 
                fl = 1; 
                break; 
            }
        }
        
        if(!fl) {
            // for(int i = 0;i<N;i++) {
            //     cout << dist[i]<<" ";
            // }
            return false;
        }
        return true;
        }
  
  Link - https://practice.geeksforgeeks.org/problems/negative-weight-cycle3504/1#
```

---

## 24. Number of Distinct Islands

**Q:** Given an `r` x `c` binary `grid`, return the number of distinct islands (connected components of `1`s), where two islands are considered identical if they can be translated to match each other.

**A:** DFS maintaining the relative coordinates of the island's cells, storing them in a `set`.

**Explanation:**
- Iterate over the grid to find an unvisited `1` (start of a new island).
- During the DFS, compute the relative position of the current cell to the island's start cell `(r-r0, c-c0)` and push it into a vector `vec`.
- After the DFS finishes, insert the vector `vec` into a `set`. The set naturally deduplicates identical shapes, and its size gives the answer.

**Complexity:** Time `O(r*c * log(number_of_islands))`, Space `O(r*c)`

```cpp
    void dfs(int r,int c,int r0,int c0,vector<vector<int>>& grid,vector<vector<bool>>& vis,vector<pair<int,int>> &vec){
        int n=grid.size(),m=grid[0].size();
        vis[r][c]=true;
        vec.push_back({r-r0,c-c0});
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0};
        for(int k=0;k<4;k++){
            int x=r+dx[k];
            int y=c+dy[k];
            if(x>=0 && y>=0 && x<n && y<m && !vis[x][y] && grid[x][y]){
                dfs(x,y,r0,c0,grid,vis,vec);
            }
        }
    }
  
    int countDistinctIslands(vector<vector<int>>& grid) {
        int n=grid.size(),m=grid[0].size();
        vector<vector<bool>> vis(n,vector<bool>(m,false));
        set<vector<pair<int,int>>> st;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j] && !vis[i][j]){
                    vector<pair<int,int>> vec;
                    dfs(i,j,i,j,grid,vis,vec);
                    st.insert(vec);
                }
            }
        }
        return st.size();
    }

gfg link - https://www.geeksforgeeks.org/problems/number-of-distinct-islands/1
```

---

## 25. Number Of Enclaves

**Q:** Given an `r` x `c` binary `mat`, find the number of land cells (`1`s) that cannot reach the boundary of the grid.

**A:** Multi-source BFS starting from the boundary land cells to mark all reachable lands.

**Explanation:**
- Enqueue all `1`s situated on the grid borders and immediately convert them to `0` (marking them as visited and safe).
- Perform a BFS to traverse all land cells connected to these boundary cells, converting them to `0` as well.
- Iterate through the matrix once more; any remaining `1`s are fully enclosed land cells (enclaves), so count and return them.

**Complexity:** Time `O(r*c)`, Space `O(r*c)`

```cpp
    int numberOfEnclaves(vector<vector<int>> &mat) {
        int n=mat.size(),m=mat[0].size();
        queue<pair<int,int>> q;
        for(int i=0;i<m;i++){
            if(mat[0][i]==1){
                q.push({0,i});
                mat[0][i]=0;
            }
            if(mat[n-1][i]==1){
                q.push({n-1,i});
                mat[n-1][i]=0;
            }
        }
        for(int i=0;i<n;i++){
            if(mat[i][0]==1){
                q.push({i,0});
                mat[i][0]=0;
            }
            if(mat[i][m-1]==1){
                q.push({i,m-1});
                mat[i][m-1]=0;
            }
        }
        
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0,};
        while(!q.empty()){
            int i=q.front().first;
            int j=q.front().second;
            q.pop();
            for(int k=0;k<4;k++){
                int x=i+dx[k];
                int y=j+dy[k];
                if(x>=0 && x<n && y>=0 && y<m && mat[x][y]==1){
                    mat[x][y]=0;
                    q.push({x,y});
                }
            }
            
        }
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(mat[i][j]==1) ans++;
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/number-of-enclaves/1
```

---

## 26. Number of Good Components

**Q:** Given `V` vertices and an `edges` list, find the number of "good" components, where every vertex in the component is connected to every other vertex in that same component (a clique).

**A:** DFS to extract all nodes in a component followed by a degree verification.

**Explanation:**
- Convert the edge list to an adjacency list.
- Use DFS to find a connected component and collect all its constituent nodes into a `nodes` array.
- A component is "good" if every node within it has an adjacency list size (degree) of exactly `component_size - 1`. If this holds for all nodes in the list, increment the good components count.

**Complexity:** Time `O(V+E)`, Space `O(V+E)`

```cpp
    void dfs(int node,vector<int> &vis,vector<int> adj[],vector<int> &nodes){
        nodes.push_back(node);
        vis[node]=1;
        for(auto it: adj[node]){
            if(!vis[it]){
                dfs(it,vis,adj,nodes);
            }
        }
    }
  
    int findNumberOfGoodComponent(int e, int v, vector<vector<int>> &edges) {
        int ans=0;
        vector<int> vis(v+1,0);
        vector<int> adj[v+1];
        for(auto it: edges){
            adj[it[0]].push_back(it[1]);
            adj[it[1]].push_back(it[0]);
        }
        for(int i=1;i<=v;i++){
            if(!vis[i]){
                vector<int> nodes;
                dfs(i,vis,adj,nodes);
                
                bool flag=1;
                for(int j=0;j<nodes.size();j++){
                    if(adj[nodes[j]].size()<nodes.size()-1){
                        flag=0;
                        break;
                    }
                }
                if(flag){
                    ans++;
                }
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/number-of-good-components--170647/1
```

---

## 27. Number of Provinces

**Q:** Given `V` vertices and an adjacency matrix `adj` of cities where `1` indicates a direct road, find the number of provinces (connected components).

**A:** DFS traversal on each unvisited city.

**Explanation:**
- Convert the adjacency matrix into an adjacency list `a` for more efficient traversal.
- Iterate through all vertices from `0` to `V-1`. If a vertex hasn't been visited in the `vis` array, it's a new province: increment the province count `ans` and launch a DFS.
- The DFS will mark all cities reachable from that vertex as visited, effectively finding the full connected component.

**Complexity:** Time `O(V^2)` for matrix conversion and traversal, Space `O(V)`

```cpp
    void dfs(int node,vector<int> a[],vector<int> &vis){
        vis[node]=1;
        for(auto it : a[node]){
            if(!vis[it]){
               dfs(it,a,vis); 
            }
        }
    }
  
    int numProvinces(vector<vector<int>> adj, int V) {
        vector<int> a[V];
        for(int i=0;i<V;i++){
            for(int j=0;j<V;j++){
                if(adj[i][j]){
                    a[j].push_back(i);
                    a[i].push_back(j);
                    
                }
            }
        }
        
        vector<int> vis(V,0);
        int ans=0;
        for(int i=0;i<V;i++){
            if(!vis[i]){
                ans++;
                dfs(i,a,vis);
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/number-of-provinces/1?utm_source=youtube&utm_medium=collab_striver_ytdescription&utm_campaign=number_of_provinces
```

---

## 28. Path With Minimum Effort

**Q:** Given a `rows` x `columns` grid `heights`, find a path from `(0,0)` to `(rows-1, columns-1)` that minimizes the maximum absolute difference in heights between consecutive cells along the path.

**A:** Modified Dijkstra's Algorithm using a min-heap priority queue.

**Explanation:**
- Maintain a `dis` matrix for the minimum effort required to reach each cell, and initialize a min-heap prioritizing minimum effort paths `{effort, {r, c}}`.
- Pop the cell with the smallest effort; if it's the destination, return the effort immediately.
- For its 4 neighbors, calculate the effort to reach them as `max(abs(heights[r][c] - heights[nr][nc]), dis[r][c])`. If this new effort is strictly less than `dis[nr][nc]`, update the distance matrix and push the neighbor into the priority queue.

**Complexity:** Time `O(rows*columns * log(rows*columns))`, Space `O(rows*columns)`

```cpp
    int MinimumEffort(int rows, int columns, vector<vector<int>> &heights) {
        vector<vector<int>> dis(rows,vector<int>(columns,1e9));
        priority_queue<pair<int,pair<int,int>>, vector<pair<int,pair<int,int>>>, greater<pair<int,pair<int,int>>> > pq;
        pq.push({0,{0,0}});
        dis[0][0]=0;
        int dr[]={0,0,1,-1};
        int dc[]={1,-1,0,0};
        while(!pq.empty()){
            auto tp=pq.top();pq.pop();
            int dist=tp.first;
            int r=tp.second.first;
            int c=tp.second.second;
            if(r==rows-1 && c==columns-1) return dist;
            for(int k=0;k<4;k++){
                int nr=r+dr[k];
                int nc=c+dc[k];
                if(nr>=0 && nr<rows && nc>=0 && nc<columns){
                    int diff=max(abs(heights[r][c]-heights[nr][nc]),dis[r][c]);
                    if(diff<dis[nr][nc]){
                        dis[nr][nc]=diff;
                        pq.push({diff,{nr,nc}});
                    }
                }
            }
        }
        return 0;
    }

gfg link - https://www.geeksforgeeks.org/problems/path-with-minimum-effort/1
```

---

## 29. Prim's Algo (Minimum Spanning Tree)

**Q:** Given `V` vertices and an adjacency list `adj` for an undirected weighted graph, find the sum of weights in the Minimum Spanning Tree (MST).

**A:** Prim's Algorithm using a min-heap priority queue.

**Explanation:**
- Use a `minh` priority queue containing `{weight, node}` initialized with `{0, 0}`.
- Pop the node with the minimum weight; if it is already visited (`vis` is 1), continue to skip it.
- Mark the node as visited, add the weight to the total MST weight `ans`, and push all unvisited adjacent neighbors `{edge_weight, neighbor}` into the queue.

**Complexity:** Time `O(E log V)`, Space `O(V)`

```cpp
// It gives Minimum weight and parent also...
    int spanningTree(int V, vector<vector<int>> adj[]){
        vector<int> key(V,1e9),mst(V,0),parent(V,-1);
        priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> minh;
        minh.push({0,0});
        key[0]=0;
        while(!minh.empty()){
            int node=minh.top().second;minh.pop();
            mst[node]=1;
            for(auto it : adj[node]){
                int next=it[0];
                int val=it[1];
                if(!mst[next] && val<key[next]){
                    parent[next]=node;
                    key[next]=val;
                    minh.push({val,next});
                }
            }
        }
        return accumulate(key.begin(),key.end(),0);
    }
    
// Only Minimum weight...
    int spanningTree(int V, vector<vector<int>> adj[]){
        vector<int> vis(V,0);
        priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> minh;
        minh.push({0,0});
        int ans=0;
        while(!minh.empty()){
            int wt=minh.top().first;
            int node=minh.top().second;
            minh.pop();
            if(vis[node]) continue;
            vis[node]=1;
            ans+=wt;
            for(auto it : adj[node]){
                if(!vis[it[0]]) minh.push({it[1],it[0]});
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/minimum-spanning-tree/1/
```

---

## 30. Replace O's with X's

**Q:** Given an `n` x `m` matrix `mat` containing `'X'` and `'O'`, replace all `'O'`s that are completely surrounded by `'X'`s with `'X'`.

**A:** Multi-source BFS starting from the boundary `'O'` cells to identify and mark all safe (reachable) `'O'`s.

**Explanation:**
- Enqueue all `'O'` cells located on the matrix boundaries and temporarily change them to `'Y'` to indicate they are safe (cannot be surrounded).
- Perform a BFS to find all other `'O'` cells connected to these boundary cells, converting them to `'Y'` as well.
- Iterate through the matrix again: change any remaining enclosed `'O'` cells to `'X'`, and revert the safe `'Y'` cells back to `'O'`.

**Complexity:** Time `O(n*m)`, Space `O(n*m)`

```cpp
    vector<vector<char>> fill(int n, int m, vector<vector<char>> mat){
        queue<pair<int,int>> q;
        for(int i=0;i<m;i++){
            if(mat[0][i]=='O'){
                q.push({0,i});
                mat[0][i]='Y';
            }
            if(mat[n-1][i]=='O'){
                q.push({n-1,i});
                mat[n-1][i]='Y';
            }
        }
        for(int i=0;i<n;i++){
            if(mat[i][0]=='O'){
                q.push({i,0});
                mat[i][0]='Y';
            }
            if(mat[i][m-1]=='O'){
                q.push({i,m-1});
                mat[i][m-1]='Y';
            }
        }
        
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0,};
        while(!q.empty()){
            int i=q.front().first;
            int j=q.front().second;
            q.pop();
            for(int k=0;k<4;k++){
                int x=i+dx[k];
                int y=j+dy[k];
                if(x>=0 && x<n && y>=0 && y<m && mat[x][y]=='O'){
                    mat[x][y]='Y';
                    q.push({x,y});
                }
            }
            
        }
        
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(mat[i][j]=='O') mat[i][j]='X';
            }
        }
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(mat[i][j]=='Y') mat[i][j]='O';
            }
        }
        return mat;
    }

gfg link - https://www.geeksforgeeks.org/problems/replace-os-with-xs0052/1
```

---

## 31. Shortest Distance in a Binary Maze

**Q:** Given an `n` x `m` binary `grid`, find the shortest path length from a `source` cell to a `destination` cell where `1` is a valid path and `0` is a wall.

**A:** Breadth-First Search (BFS) for shortest path on an unweighted grid.

**Explanation:**
- Return `0` immediately if `source == destination`, and return `-1` if the destination cell itself is a wall (`0`).
- Initialize a `dist` array with `1e9` (infinity) and a queue containing the `source` cell, setting `dist[source] = 0`.
- Perform BFS using a 4-directional array. If a neighbor is a `1` and can be reached with a strictly shorter distance, update `dist[neighbor] = 1 + dist[current]`, and push it to the queue.
- If the updated neighbor matches the `destination` coordinates, return the distance immediately to avoid unnecessary work.

**Complexity:** Time `O(n*m)`, Space `O(n*m)`

```cpp
    int shortestPath(vector<vector<int>> &grid, pair<int, int> source,pair<int, int> destination) {
        if(source==destination) return 0;
        if(!grid[destination.first][destination.second]) return -1;
        int n=grid.size(),m=grid[0].size();
        vector<vector<int>> dist(n,vector<int>(m,1e9));
        queue<pair<int,int>> q;
        q.push(source);
        dist[source.first][source.second]=0;
        int dx[]={0,0,1,-1};
        int dy[]={1,-1,0,0};
        while(!q.empty()){
            auto it=q.front();q.pop();
            int i=it.first;
            int j=it.second;
            for(int k=0;k<4;k++){
                int x=i+dx[k];
                int y=j+dy[k];
                if(x>=0 && x<n && y>=0 && y<m && grid[x][y]==1){
                    if(dist[x][y]>1+dist[i][j]){
                        dist[x][y]=1+dist[i][j];
                        if(destination.first==x && destination.second==y) return dist[x][y];
                        q.push({x,y});
                    }
                }
            }
        }
        return -1;
    }

gfg link - https://www.geeksforgeeks.org/problems/shortest-path-in-a-binary-maze-1655453161/1
```

---

## 32. Shortest path in Directed Acyclic Graph

**Q:** Given `N` vertices and `M` directed weighted `edges` in a Directed Acyclic Graph (DAG), find the shortest path from vertex `0` to all other vertices.

**A:** Topological Sort via DFS followed by edge relaxation.

**Explanation:**
- Build an adjacency list and perform a DFS to find the topological ordering of the DAG, pushing nodes onto a stack `s` when their traversal finishes.
- Initialize a distance array `dis` with `1e9` (infinity) and set `dis[0] = 0`.
- Pop nodes from the stack in topological order. For each node, relax its outgoing edges by updating the neighbors' distances: `dis[neighbor] = min(dis[neighbor], dis[node] + weight)`.
- Replace any unreached distances (`1e9`) with `-1` before returning the array.

**Complexity:** Time `O(N+M)`, Space `O(N+M)`

```cpp
    void topo(int i,vector<pair<int,int>> adj[],stack<int> &s,vector<int> &vis){
        vis[i]=1;
        for(auto it : adj[i]){
            if(!vis[it.first]){
                topo(it.first,adj,s,vis);
            }
        }
        s.push(i);
    }
  
    vector<int> shortestPath(int N,int M, vector<vector<int>>& edges){
        vector<pair<int,int>> adj[N];
        for(auto it : edges){
            adj[it[0]].push_back({it[1],it[2]});
        }
        stack<int> s;
        vector<int> vis(N,0);
        for(int i=0;i<N;i++){
            if(!vis[i]){
                topo(i,adj,s,vis);
            }
        }
        vector<int> dis(N,1e9);
        dis[0]=0;
        while(!s.empty()){
            int node=s.top();s.pop();
            for(auto it : adj[node]){
                dis[it.first]=min(dis[it.first],it.second+dis[node]);
            }
        }
        for(int i=0;i<N;i++){
            if(dis[i]==1e9) dis[i]=-1;
        }
        return dis;
    }

gfg link - https://www.geeksforgeeks.org/problems/shortest-path-in-undirected-graph/1
```

---

## 33. Shortest Path in Directed Acyclic Graph

**Q:** Given `n` vertices, a `src` vertex, and an adjacency list `adj` for a Directed Acyclic Graph (DAG), find the shortest distance from `src` to all other vertices.

**A:** Topological Sort via DFS followed by edge relaxation.

**Explanation:**
- Compute the topological ordering of all nodes using DFS and store them in a stack.
- Initialize the distance array `dis` with `1e9` (infinity) and set `dis[src] = 0`.
- Pop nodes from the topological stack sequentially; if the current node has been reached (`dis != 1e9`), update its neighbors' distances if `dis[node] + weight < dis[neighbor]`.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
void topoSort(int node,stack<int> &st,vector<int> &vis,vector<pair<int,int>> adj[]){
    vis[node]=1;
    for(auto it : adj[node]){
        if(!vis[it.first]){
            topoSort(it.first,st,adj);
        }
    }
    st.push(node);
}
vector<int> shortestPath(int src,int n,vector<pair<int,int>> adj[]){
    vector<int> dis(n,1e9);
    vector<int> vis(n,0);
    stack<int> st;
    for(int i=0;i<n;i++){
        if(!vis[i]) topoSort(i,st,vis,adj);
    }
    dis[src]=0;
    while(!st.empty()){
        int node=st.top();st.pop();
        if(dis[node]!=1e9){
            for(auto it : adj[node]){
                if(dis[node]+it.second < dis[it.first]){
                    dis[it.first]=it.second+dis[node];
                }
            }
        }
    }
    return dis;
}

Link - https://www.geeksforgeeks.org/shortest-path-for-directed-acyclic-graphs/
```

---

## 34. Shortest Path in Undirected Graph with Unit Weights

**Q:** Given `n` vertices, a `src` vertex, and an adjacency list `adj` for an undirected graph with unit edge weights, find the shortest distance from `src` to all other vertices.

**A:** Breadth-First Search (BFS) using a simple queue.

**Explanation:**
- Initialize a `dis` array with `INT_MAX` and set `dis[src] = 0`, then push `src` to a queue.
- Pop nodes from the queue and iterate through their neighbors.
- If reaching the neighbor through the current node offers a shorter path (`dis[node] + 1 < dis[neighbor]`), update `dis[neighbor]` and enqueue it.

**Complexity:** Time `O(V+E)`, Space `O(V)`

```cpp
vector<int> shortestPath(int src,int n,vector<int> adj[]){
    vector<int> dis(n,INT_MAX);
    queue<int> q;
    q.push(src);
    dis[src]=0;
    while(!q.empty()){
        int node=q.front();q.pop();
        for(auto it : adj[node]){
            if(dis[node]+1<dis[it]){
                dis[it]=1+dis[node]+1;
                q.push(it);
            }
        }
    }
    return dis;
}
```

---

## 35. Shortest Path in Weighted undirected graph

**Q:** Given `n` vertices and a list of `edges` for a weighted undirected graph, find the shortest path from vertex `1` to vertex `n`, returning the total weight followed by the sequence of nodes.

**A:** Dijkstra's Algorithm with path reconstruction using a `parent` array.

**Explanation:**
- Build the adjacency list and initialize a `dist` array with infinity (`1e9`) and a `parent` array where `parent[i] = i`.
- Run Dijkstra's algorithm from vertex `1`. When a shorter path to a neighbor is found, update its `dist`, set its `parent` to the current node, and push it to the min-heap.
- If vertex `n` is unreachable, return `{-1}`. Otherwise, backtrack from `n` using the `parent` array to build the path, append the total distance, and reverse the sequence for the final result.

**Complexity:** Time `O(E log V)`, Space `O(V+E)`

```cpp
    vector<int> shortestPath(int n, int m, vector<vector<int>>& edges) {
        vector<pair<int,int>> adj[n+1];
        for(auto it : edges){
            adj[it[0]].push_back({it[1],it[2]});
            adj[it[1]].push_back({it[0],it[2]});
        }
        priority_queue< pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>> >pq;
        vector<int> dist(n+1,1e9);
        vector<int> parent(n+1);
        for(int i=1;i<=n;i++) parent[i]=i;
        pq.push({0,1});
        dist[1]=0;
        while(!pq.empty()){
            int node=pq.top().second;
            int dis=pq.top().first;
            pq.pop();
            for(auto it : adj[node]){
                if(it.second+dis<dist[it.first]){
                    dist[it.first]=it.second+dis;
                    parent[it.first]=node;
                    pq.push({dist[it.first],it.first});
                }
            }
        }
        if(dist[n]==1e9) return {-1};
        vector<int> ans;
        int i=n;
        while(parent[i]!=i){
            ans.push_back(i);
            i=parent[i];
        }
        ans.push_back(1);
        ans.push_back(dist[n]);
        reverse(ans.begin(),ans.end());
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/shortest-path-in-weighted-undirected-graph/1
```

---

## 36. Shortest Source to Destination Path

**Q:** Given an `N` x `M` binary grid `A`, find the shortest path from `(0, 0)` to `(X, Y)` where `1` represents a valid path and `0` represents a wall.

**A:** Level-order Breadth-First Search (BFS) on an unweighted grid.

**Explanation:**
- Return `-1` if the starting cell `(0, 0)` is a wall `0`.
- Maintain a queue holding the cell coordinates and mark visited cells by changing them to `0` directly in the grid to save space.
- Process the queue level-by-level, incrementing the distance `ans` after each full level, and return `ans` when the target `(X, Y)` is popped.

**Complexity:** Time `O(N*M)`, Space `O(N*M)`

```cpp
    int shortestDistance(int N, int M, vector<vector<int>> A, int X, int Y) {
        if(A[0][0]==0) return -1;
        queue<pair<int,int>> q;
        q.push({0,0});
        int dx[]={0,1,0,-1};
        int dy[]={1,0,-1,0};
        int ans=0;
        while(!q.empty()){
            int len=q.size();
            for(int i=0;i<len;i++){
                int r=q.front().first;
                int c=q.front().second;
                q.pop();
                if(r==X && c==Y) return ans;
                for(int k=0;k<4;k++){
                    int x=r+dx[k];
                    int y=c+dy[k];
                    if(x>=0 && y>=0 && x<N && y<M && A[x][y]){
                        q.push({x,y});
                        A[x][y]=0;
                    }
                }
            }
            ans++;
        }
        return -1;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/shortest-source-to-destination-path3544/1/
```

---

## 37. Snake and Ladder Problem

**Q:** Given a 1D board of 30 cells and an array `arr` denoting `N` snakes or ladders (pairs of start and end cells), find the minimum dice throws to reach cell 30 from cell 1.

**A:** Level-order Breadth-First Search (BFS) tracking the number of dice throws.

**Explanation:**
- Map the jump configurations (snakes and ladders) into a hash map `mp` for `O(1)` lookup of destinations.
- Use a queue starting with cell `1` and process it level-by-level, simulating 6 dice rolls (from 1 to 6) for each popped cell.
- If a destination cell contains a ladder or a snake, move directly to its final destination. If it hasn't been visited, mark it and enqueue it, returning the level count when cell `30` is reached.

**Complexity:** Time `O(1)` (since the board size is constantly 30), Space `O(1)`

```cpp
    int minThrow(int N, int arr[]){
        unordered_map<int,int> mp;
        for(int i=0;i<2*N-1;i+=2) mp[arr[i]]=arr[i+1];
        vector<bool> vis(31,false);
        queue<int> q;
        q.push(1);
        vis[1]=true;
        int ans=0;
        while(!q.empty()){
            int len=q.size();
            for(int i=0;i<len;i++){
                int node=q.front();q.pop();
                if(node==30) return ans;
                for(int j=1;j<7;j++){
                    int next=node+j;
                    if(next>30) break;
                    if(mp.find(next)!=mp.end()) next=mp[next];
                    if(!vis[next]){
                        vis[next]=true;
                        q.push(next);
                    }
                }
            }
            ans++;
        }
        return -1;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/snake-and-ladder-problem4816/1/
```

---

## 38. Steps by Knight

**Q:** Given an `n` x `n` chessboard, a starting `KnightPos`, and a `TargetPos`, find the minimum steps a Knight takes to reach the target position.

**A:** Level-order Breadth-First Search (BFS) using the 8 possible Knight moves.

**Explanation:**
- Return `0` immediately if the start position matches the target position.
- Start a queue with the Knight's initial position and mark it visited in a `vis` matrix.
- Traverse the queue level-by-level; for each cell, try all 8 Knight jump directions (`dx`, `dy`). If the new cell is within bounds and unvisited, mark it and enqueue it, returning the level count `step` when the target is matched.

**Complexity:** Time `O(n^2)`, Space `O(n^2)`

```cpp
        int minStepToReachTarget(vector<int>&KnightPos,vector<int>&TargetPos,int n){
            int kx=KnightPos[0],ky=KnightPos[1];
            int tx=TargetPos[0],ty=TargetPos[1];
            if(kx==tx && ky==ty) return 0;
        vector<vector<bool>> vis(n+1,vector<bool>(n+1,false));
        queue<pair<int,int>> q;
        q.push({kx,ky});
        vis[kx][ky]=true;
        int step=0;
        while(!q.empty()){
            int sz=q.size();
            for(int s=0;s<sz;s++){
                auto it=q.front();q.pop();
                int i=it.first,j=it.second;
                if(i==tx && j==ty) return step;
                int dx[]={-1,-1,1,1,-2,-2,2,2};
                int dy[]={-2,2,-2,2,-1,1,-1,1};
                for(int k=0;k<8;k++){
                    int x=i+dx[k];
                    int y=j+dy[k];
                    if(x>0 && y>0 && x<=n && y<=n && vis[x][y]==false){
                        vis[x][y]=true;
                        q.push({x,y});
                    }
                }
            }
            step++;
        }
        return -1;
        }
  
  Link - https://practice.geeksforgeeks.org/problems/steps-by-knight5927/1
```

---

## 39. Strongly Connected Components (Kosaraju's Algo)

**Q:** Given `n` vertices and a directed graph represented by an adjacency list `adj`, find the number of Strongly Connected Components (SCCs).

**A:** Kosaraju's Algorithm utilizing two sequential DFS passes and graph transposition.

**Explanation:**
- First pass: Perform DFS on all unvisited nodes to compute finishing times, pushing each node onto a stack `st` once all its descendants have been explored.
- Construct a transposed (reversed edge) graph `transpose` by iterating through all edges in `adj`.
- Second pass: Pop nodes from the stack; if a popped node is unvisited, it marks the discovery of a new SCC. Increment the SCC count and perform a DFS on the transposed graph to mark all nodes within this SCC as visited.

**Complexity:** Time `O(V+E)`, Space `O(V+E)` for the transposed graph and recursion stack

```cpp
        void dfs(int node, stack<int> &st, vector<int> &vis, vector<int> adj[]) {
        vis[node] = 1; 
        for(auto it: adj[node]) {
            if(!vis[it]) {
                dfs(it, st, vis, adj); 
            }
        }
        st.push(node); 
    }
    void revDfs(int node, vector<int> &vis, vector<int> transpose[]) {
        // cout << node << " "; 
        vis[node] = 1; 
        for(auto it: transpose[node]) {
            if(!vis[it]) {
                revDfs(it, vis, transpose); 
            }
        }
    }
    
    int kosaraju(int n, vector<int> adj[]){
        stack<int> st;
        vector<int> vis(n, 0); 
        for(int i=0;i<n;i++) {
            if(!vis[i]) {
                dfs(i, st, vis, adj); 
            }
        } 
    
        vector<int> transpose[n]; 
    
        for(int i=0;i<n;i++) {
            vis[i] = 0; 
            for(auto it: adj[i]) {
                transpose[it].push_back(i); 
            }
        }
    
    
        int ans=0;
        while(!st.empty()) {
            int node = st.top();
            st.pop(); 
            if(!vis[node]) {
               // cout << "SCC: "; 
                revDfs(node, vis, transpose); 
               // cout << endl;
               ans++;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/strongly-connected-components-kosarajus-algo/1/
```

---

## 40. Topological sort

**Q:** Given `V` vertices and a directed acyclic graph represented by an adjacency list `adj`, return an array containing the topological sort of the graph.

**A:** Kahn's Algorithm (BFS) or Depth-First Search (DFS) with a stack.

**Explanation:**
- BFS (Kahn's): Calculate in-degrees for all nodes, initialize a queue with nodes of `0` in-degree. Pop them, add to the answer, and decrement neighbors' in-degrees, queueing them when they hit `0`.
- DFS: Traverse unvisited nodes recursively, pushing a node onto a stack `st` only after completely traversing all of its descendants.
- DFS: Finally, empty the stack into the answer array; since a node was pushed after its dependencies, popping them yields the correct topological order.

**Complexity:** BFS/DFS: Time `O(V+E)`, Space `O(V)`

```cpp
// Using BFS...
        vector<int> topoSort(int V, vector<int> adj[]) {
            vector<int> ans;
            vector<int> inDeg(V,0);
            for(int i=0;i<V;i++){
                for(auto it : adj[i]){
                    inDeg[it]++;
                }
            }
            
            queue<int> q;
            for(int i=0;i<V;i++){
                if(inDeg[i]==0) q.push(i);
            }
            
            while(!q.empty()){
                int node=q.front();q.pop();
                ans.push_back(node);
                for(auto it : adj[node]){
                    inDeg[it]--;
                    if(inDeg[it]==0) q.push(it);
                }
            }
            return ans;
        }

// Using DFS...
    void dfs(int node,vector<int> adj[],stack<int> &st,vector<int> &vis){
        vis[node]=1;
        for(auto it : adj[node]){
            if(!vis[it]){
                dfs(it,adj,st,vis);
            }
        }
        st.push(node);
    }
        vector<int> topoSort(int V, vector<int> adj[]) {
            vector<int> ans;
            vector<int> vis(V,0);
            stack<int>st;
            for(int i=0;i<V;i++){
                if(!vis[i]) dfs(i,adj,st,vis);
            }
            while(!st.empty()){
                ans.push_back(st.top());
                st.pop();
            }
            return ans;
        }

        Links - https://practice.geeksforgeeks.org/problems/topological-sort/1/
```

---

## 41. Unit Area of largest region of 1's

**Q:** Given an `n` x `m` binary `grid`, find the area (number of cells) of the largest connected region of `1`s, considering 8-directional connectivity.

**A:** Breadth-First Search (BFS) or Depth-First Search (DFS) on the unvisited `1`s to measure component size.

**Explanation:**
- Iterate over the grid to find a `1` and initiate a traversal (BFS or DFS). To avoid using a separate visited matrix, mark cells as visited by setting `grid[x][y] = 0`.
- BFS: Use a queue initialized with the starting cell. Count every valid cell popped, explore its 8 directions, and update the global `ans` with the maximum component size found.
- DFS: Recursively sum `1` (for the current cell) plus the area returned by DFS calls on 8-directional valid neighbors, and return the total area to track the maximum.

**Complexity:** BFS/DFS: Time `O(n*m)`, Space `O(n*m)` for the queue or recursion stack

```cpp
    int dx[8]={-1,-1,-1,0,0,1,1,1};
    int dy[8]={-1,0,1,-1,1,-1,0,1};

// BFS...
    void bfs(int i,int j,int n,int m,vector<vector<int>> &grid,int &ans){
        int res=0;
        grid[i][j]=0;
        queue<pair<int,int>> q;
        q.push({i,j});
        while(!q.empty()){
            auto it=q.front();q.pop();
            res++;
            int r=it.first, c=it.second;
            for(int k=0;k<8;k++){
                int x=r+dx[k];
                int y=c+dy[k];
                if(x<n && x>=0 && y<m && y>=0 && grid[x][y]){
                    grid[x][y]=0;
                    q.push({x,y});
                }
            }
        }
        ans=max(ans,res);
    }
    
    int findMaxArea(vector<vector<int>>& grid) {
        // Code here
        int n=grid.size(),m=grid[0].size();
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j]) bfs(i,j,n,m,grid,ans);
            }
        }
        return ans;
    }
    
    
// DFS...
    int dfs(int i,int j,int n,int m,vector<vector<int>> &grid){
        grid[i][j]=0;
        int area=1;
        for(int k=0;k<8;k++){
            int x=i+dx[k];
            int y=j+dy[k];
            if(x<n && x>=0 && y<m && y>=0 && grid[x][y]){
                grid[x][y]=0;
                area+=dfs(x,y,n,m,grid);
            }
        }
        return area;
    }
    
    int findMaxArea(vector<vector<int>>& grid) {
        // Code here
        int n=grid.size(),m=grid[0].size();
        int ans=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<m;j++){
                if(grid[i][j]) ans=max(ans,dfs(i,j,n,m,grid));
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/length-of-largest-region-of-1s-1587115620/1
```

---

## 42. Word Boggle

**Q:** Given a `dictionary` of words and an `n` x `m` `board` of characters, find all distinct words that can be formed by a sequence of adjacent characters (including diagonals).

**A:** Depth-First Search (DFS) with backtracking on the grid for each word.

**Explanation:**
- Iterate through each word in the `dictionary` and try to find a valid starting cell on the `board` matching the first character.
- In DFS, track the current character index `idx`. If it matches the cell, temporarily mark the cell as `'*'` to prevent reusing it within the same path.
- Recursively check all 8 surrounding directions for the next character `idx+1`, backtrack by restoring the cell's original character, and return `true` if any direction leads to the full word.

**Complexity:** Time `O(W * n * m * 8^L)` where `W` is the number of words and `L` is the max word length, Space `O(L)` for the recursion stack

```cpp
    bool dfs(vector<vector<char> >& board, string &s, int i, int j, int n, int m, int idx){
       if(i<0 || i>=n||j<0||j>=m) return false;
       if(s[idx]!= board[i][j]) return false;
       if(idx == s.size()-1) return true;
       
       char temp = board[i][j];
       board[i][j]='*';
       
       bool a = dfs(board,s,i,j+1,n,m,idx+1);
       bool b = dfs(board,s,i,j-1,n,m,idx+1);
       bool c = dfs(board,s,i+1,j,n,m,idx+1);
       bool d = dfs(board,s,i-1,j,n,m,idx+1);
       bool e = dfs(board,s,i+1,j+1,n,m,idx+1);
       bool f = dfs(board,s,i-1,j+1,n,m,idx+1);
       bool g = dfs(board,s,i+1,j-1,n,m,idx+1);
       bool h = dfs(board,s,i-1,j-1,n,m,idx+1);
       
       board[i][j]=temp;
       return a||b||c||e||f||g||h||d;
    }
    
    vector<string> wordBoggle(vector<vector<char> >& board, vector<string>& dictionary) {
       int n= board.size();
       int m = board[0].size();
       vector<string> ans;
       set<string> store;
        for(int i=0;i<dictionary.size();i++){
            string s = dictionary[i];
            int l = s.size();
            for(int j = 0 ; j < n;j++){
                for(int k=0;k<m;k++){
                    if(dfs(board,s,j,k,n,m,0)){
                        store.insert(s);
                    }
                }
            }
        }
        
        for(auto i:store) ans.push_back(i);
        sort(ans.begin(),ans.end());
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/word-boggle4143/1
```

---

## 43. Word Ladder II

**Q:** Given a `beginWord`, an `endWord`, and a `wordList`, find all shortest transformation sequences from `beginWord` to `endWord` where each adjacent word differs by exactly one character.

**A:** Level-order Breadth-First Search (BFS) tracking full path vectors.

**Explanation:**
- Use a queue of paths (vectors of strings) starting with `[beginWord]`. Keep an `unordered_set` `st` of available words for valid transitions.
- Maintain a `tmp` array of words visited during the *current* level. When moving to a new level (path size exceeds `level`), erase all words in `tmp` from `st` to prevent redundant, non-shortest visits while allowing different paths to reach the same word on the same level.
- For the last word in a popped path, try all 1-character variations; if a variation exists in `st`, append it to the path and enqueue the new path. Store valid paths in `ans` if they reach `endWord` and match the shortest length.

**Complexity:** Time `O(N * 26 * L * paths)` where `N` is wordList size, `L` is string length, Space `O(N * paths * L)`

```cpp
vector<vector<string>> findSequences(string beginWord, string endWord, vector<string>& wordList) {
        vector<vector<string>> ans;
        unordered_set<string> st;
        for(auto it : wordList) st.insert(it);
        queue<vector<string>> q;
        q.push({beginWord});
        vector<string> tmp;
        tmp.push_back(beginWord);
        int level=0;
        while(!q.empty()){
            vector<string> vec=q.front();
            q.pop();
            if(vec.size() > level){
                level++;
                for(auto it : tmp){
                    st.erase(it);
                }
                tmp.clear();
            }
            string word=vec.back();
            if(word ==endWord){
                if(ans.size()==0){
                    ans.push_back(vec);
                }else if(ans[0].size()==vec.size()){
                    ans.push_back(vec);
                }
            }
            for(int i=0;i<word.size();i++){
                char c=word[i];
                for(char j='a';j<='z';j++){
                    word[i]=j;
                    if(st.count(word) > 0){
                        vec.push_back(word);
                        q.push(vec);
                        tmp.push_back(word);
                        vec.pop_back();
                    }
                }
                word[i]=c;
            }
        }
        return ans;
    }

gfg link - https://www.geeksforgeeks.org/problems/word-ladder-ii/1?utm_source=youtube&utm_medium=collab_striver_ytdescription&utm_campaign=word-ladder-ii
```

---

