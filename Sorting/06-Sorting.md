# Sorting — Q&A
> Source: `Kirancse47/DSA` -> `Sorting/` — 11 problems

| Algorithm      | Best Time  | Avg Time   | Worst Time | Space    | Stable | In-Place |
|----------------|------------|------------|------------|----------|--------|----------|
| Bubble Sort    | `O(n)`     | `O(n^2)`   | `O(n^2)`   | `O(1)`   | Yes    | Yes      |
| Selection Sort | `O(n^2)`   | `O(n^2)`   | `O(n^2)`   | `O(1)`   | No     | Yes      |
| Insertion Sort | `O(n)`     | `O(n^2)`   | `O(n^2)`   | `O(1)`   | Yes    | Yes      |
| Merge Sort     | `O(n log n)`| `O(n log n)`| `O(n log n)`| `O(n)`   | Yes    | No       |
| Quick Sort     | `O(n log n)`| `O(n log n)`| `O(n^2)`   | `O(log n)`| No     | Yes      |
| Heap Sort      | `O(n log n)`| `O(n log n)`| `O(n log n)`| `O(1)`   | No     | Yes      |

`std::sort` is introsort (quicksort with heapsort fallback and insertion sort for small partitions) and is not stable, while `std::stable_sort` uses merge sort. Comparators must provide a strict weak ordering (return `false` for equal elements).

## 1. 274. H-Index

**Q:** Calculate the h-index of a researcher given an array of citations, where h is the maximum number such that the researcher has at least h papers with h or more citations.

**A:** Sort the citations and find the largest h by comparing values with their suffix lengths.

**Explanation:**
- Sort the citation array in ascending order.
- Iterate through the sorted array; the number of papers with at least `cit[i]` citations is `n - i`.
- If `cit[i]` is greater than or equal to `n - i`, then `n - i` is a valid h-index.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
    int hIndex(vector<int>& cit) {
        int n=cit.size();
        sort(cit.begin(),cit.end());
        for(int i=0;i<n;i++){
            int h=n-i;
            if(cit[i]>=h) return h;
        }
        return 0;
    }
```

---

## 2. 315. Count of Smaller Numbers After Self

**Q:** Return an array where the i-th element is the count of numbers smaller than `nums[i]` to its right.

**A:** Use merge sort to count inversions for each element during the merge step.

**Explanation:**
- Attach the original indices to the array elements so we can update their counts.
- During the merge step of merge sort, if an element from the left half is strictly greater than an element from the right half, it is greater than all remaining elements in the right half.
- Tally these counts for each element using the original indices.

**Complexity:** Brute Force: Time `O(n^2)`, Space `O(1)`. Merge Sort: Time `O(n log n)`, Space `O(n)`.

```cpp
// Broute Force...(TLE)
    vector<int> countSmaller(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans(n,0);
        for(int i=n-2;i>=0;i--){
            int count=0;
            for(int j=i+1;j<n;j++){
                if(nums[i]>nums[j]) count++;
            }
            ans[i]=count;
        }
        return ans;
    }
    
// Using MergeSort...
    void merge(int l,int mid,int r,vector<pair<int,int>> &vp,vector<int> &ans){
        vector<pair<int,int>> temp(r-l+1);
        int i=l,j=mid+1,k=0;
        while(i<=mid && j<=r){
            if(vp[i].first<=vp[j].first){
                temp[k++]=vp[j++];
            }else{
                ans[vp[i].second]+=r-j+1;
                temp[k++]=vp[i++];
            }
        }
        while(i<=mid){
            temp[k++]=vp[i++];
        }
        while(j<=r){
            temp[k++]=vp[j++];
        }
        for(int it=l;it<=r;it++){
            vp[it]=temp[it-l];
        }
    }
    
    void mergeSort(int l,int r,vector<pair<int,int>> &vp,vector<int> &ans){
        if(l>=r) return;
        int mid=l+(r-l)/2;
        mergeSort(l,mid,vp,ans);
        mergeSort(mid+1,r,vp,ans);
        merge(l,mid,r,vp,ans);
    }
    
    vector<int> countSmaller(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans(n,0);
        vector<pair<int,int>> vp;
        for(int i=0;i<n;i++) vp.push_back({nums[i],i});
        mergeSort(0,n-1,vp,ans);
        return ans;
    }
```

---

## 3. 977. Squares of a Sorted Array

**Q:** Given an integer array sorted in non-decreasing order, return an array of the squares of each number sorted in non-decreasing order.

**A:** Use two pointers from the ends of the array to merge the squared values.

**Explanation:**
- Since negative numbers become positive when squared, the largest squares will be at the ends of the sorted array.
- Place two pointers, one at the start and one at the end, and compare their absolute values.
- Write the larger square to the end of the result array and move the respective pointer inward.

**Complexity:** Time `O(n)`, Space `O(n)`.

```cpp
    vector<int> sortedSquares(vector<int>& nums) {
        int n=nums.size();
        vector<int> ans(n);
        int i=0,j=n-1;
        int k=n-1;
        while(i<=j){
            if(abs(nums[i])<abs(nums[j])){
                ans[k--]=nums[j]*nums[j];
                j--;
            }else{
                ans[k--]=nums[i]*nums[i];
                i++;
            }
        }
        return ans;
    }
```

---

## 4. Bubble Sort

**Q:** Implement bubble sort to sort an array of n elements.

**A:** Repeatedly swap adjacent out-of-order elements, bubbling the largest elements to the end.

**Explanation:**
- Use nested loops where the outer loop tracks passes and the inner loop compares adjacent elements.
- Swap elements if the left is greater than the right.
- Optimize by breaking early if no swaps occurred during a full pass, meaning the array is already sorted.

**Complexity:** Time `O(n^2)`, Space `O(1)`.

```cpp
    void bubbleSort(int arr[], int n){
        // Your code here  
        for(int i=0;i<n-1;i++){
            int f=1;
            for(int j=0;j<n-i-1;j++){
                if(arr[j] > arr[j+1]){ 
                    swap(&arr[j],&arr[j+1]);
                    f=0;
                }
            }
            if(f) return;
        }
    }
    
    Link - https://practice.geeksforgeeks.org/problems/bubble-sort/1
```

---

## 5. Find Pair Given Difference

**Q:** Find if there exists a pair of elements in an array whose absolute difference equals a given value x.

**A:** Sort the array and use a two-pointer sliding window to search for the difference.

**Explanation:**
- Sort the array to process values monotonically.
- Use two pointers, `i` and `j` (starting at 0 and 1).
- If the difference is `x` (and pointers are distinct), return 1; if less than `x`, increment `j`; if greater, increment `i`.

**Complexity:** Time `O(n log n)`, Space `O(1)`.

```cpp
    int findPair(int n, int x, vector<int> &arr) {
        sort(arr.begin(),arr.end());
        int i=0,j=1;
        while(i<n && j<n){
            int d=arr[j]-arr[i];
            if(i!=j && d==x) return 1;
            else if(d<x) j++;
            else i++;
        }
        return -1;
    }

gfg link - https://www.geeksforgeeks.org/problems/find-pair-given-difference1559/1
```

---

## 6. Heap Sort

**Q:** Implement heap sort to sort an array of n elements.

**A:** Build a max-heap and repeatedly swap the root with the end of the array, restoring the heap property.

**Explanation:**
- First, build a max-heap by calling `heapify` from the last non-leaf node up to the root.
- In the second phase, swap the maximum element (at index 0) with the last element.
- Reduce the heap size by 1 and `heapify` the root to maintain the max-heap structure, repeating until sorted.

**Complexity:** Time `O(n log n)`, Space `O(1)` auxiliary (recursive stack `O(log n)`).

```cpp
  //Heapify function to maintain heap property.
    void heapify(int arr[], int n, int i)  {
      // Your Code Here
      int large=i;
      int l=2*i+1, r=2*i+2;
      if(l<n && arr[l]>arr[large]){
          large=l;
      }
      if(r<n && arr[r]>arr[large]){
          large=r;
      }
      if(large!=i){
          swap(arr[large],arr[i]);
          heapify(arr,n,large);
      }
    }
  
    //Function to build a Heap from array.
    void buildHeap(int arr[], int n) {
        // Your Code Here
        for(int i=n/2-1;i>=0;i--){
            heapify(arr,n,i);
        }
    }

 //Function to sort an array using Heap Sort.
    void heapSort(int arr[], int n){
        //code here
        buildHeap(arr,n);
        for(int i=n-1;i>0;i--){
            swap(arr[0],arr[i]);
            heapify(arr,i,0);
        }
    }
    
    Link - https://practice.geeksforgeeks.org/problems/heap-sort/1
```

---

## 7. Insertion Sort

**Q:** Implement insertion sort to sort an array of n elements.

**A:** Build the sorted array one element at a time by shifting larger elements to the right.

**Explanation:**
- Iterate from the second element to the end of the array.
- Store the current element as a key and compare it to previous elements in the sorted portion.
- Shift all elements greater than the key one position to the right and insert the key in its correct spot.

**Complexity:** Time `O(n^2)`, Space `O(1)`.

```cpp
    void insert(int arr[], int i){
        int key=arr[i];
        int j=i-1;
        while(j>=0 && arr[j]>key){
            arr[j+1]=arr[j];
            j--;
        }
        arr[j+1]=key;
    }
    void insertionSort(int arr[], int n){
        for(int i=1;i<n;i++){
            insert(arr,i);
        }
    }
    
    Link - https://practice.geeksforgeeks.org/problems/insertion-sort/0
```

---

## 8. Merge Sort

**Q:** Implement merge sort to sort an array.

**A:** Recursively divide the array in half and then merge the sorted halves.

**Explanation:**
- Recursively split the array into two halves until the base case (size 1) is reached.
- In the `merge` step, copy elements to temporary left and right arrays.
- Use two pointers to compare elements from the temporary arrays and write them back in sorted order.

**Complexity:** Time `O(n log n)`, Space `O(n)`.

```cpp
    void merge(int p,int q,int r,vector<int> &arr){
        int n1=q-p+1,n2=r-q;
        vector<int> left(n1),right(n2);
        for(int i=0;i<n1;i++) left[i]=arr[p+i];
        for(int i=0;i<n2;i++) right[i]=arr[q+i+1];
        
        int i=0,j=0,k=p;
        while(i<n1 && j<n2){
            if(left[i]<=right[j]) arr[k++]=left[i++];
            else arr[k++]=right[j++];
        }
        while(i<n1) arr[k++]=left[i++];
        while(j<n2) arr[k++]=right[j++];
    }
  
    void mergeSort(vector<int>& arr, int l, int r) {
        if(l<r){
            int m=l+(r-l)/2;
            mergeSort(arr,l,m);
            mergeSort(arr,m+1,r);
            merge(l,m,r,arr);
        }
    }

gfg - https://www.geeksforgeeks.org/problems/merge-sort/1
```

---

## 9. Missing element of AP

**Q:** Find the missing element in an arithmetic progression of length n.

**A:** Use binary search to locate the missing term by comparing expected values with actual values.

**Explanation:**
- Calculate the common difference using the first and last elements: `(arr[n-1] - arr[0]) / n`.
- Apply binary search: if `arr[mid]` matches its expected value based on the common difference, the missing element is to the right.
- If it does not match, the missing element is to the left or at `mid`.

**Complexity:** Time `O(log n)`, Space `O(1)`.

```cpp
    int findMissing(int arr[], int n){
        int diff=(arr[n-1]-arr[0])/n;
        int low=0,high=n-1;
        while(low<=high){
            int mid=low+(high-low)/2;
            if((arr[mid]-arr[0])/diff==mid){
                low=mid+1;
            }else high=mid-1;
        }
        return arr[high]+diff;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/missing-element-of-ap2228/1
```

---

## 10. Quick Sort

**Q:** Implement quick sort to sort an array of n elements.

**A:** Recursively partition the array around a pivot element.

**Explanation:**
- Choose the last element as the pivot.
- Iterate through the segment, swapping elements smaller than the pivot to the left side.
- Swap the pivot into its correct final position and recursively apply the algorithm to the left and right subarrays.

**Complexity:** Time `O(n log n)` average, `O(n^2)` worst; Space `O(log n)` recursive stack.

```cpp
    int partition (int arr[], int low, int high){
       // Your code here
       int pivot=arr[high];
       int i=low-1;
       for(int j=low;j<high;j++){
           if(arr[j]<pivot){
               i++;
               swap(arr[i],arr[j]);
           }
       }
       swap(arr[i+1],arr[high]);
       return i+1;
    }
    void quickSort(int arr[], int low, int high){
        // code here
        if(low>=high) return;
        int ind=partition(arr,low,high);
        quickSort(arr,low,ind-1);
        quickSort(arr,ind+1,high);
    }
    
    Link - https://practice.geeksforgeeks.org/problems/quick-sort/1
```

---

## 11. Sort an array according to the other

**Q:** Sort array A1 such that the relative ordering of elements matches array A2, and append remaining elements in sorted order.

**A:** Count frequencies in A1 and output them in the order specified by A2.

**Explanation:**
- Use an ordered map to store the frequencies of elements in A1, which naturally sorts the remaining keys.
- Iterate through A2 and append elements to the result array according to their frequency in A1, decrementing the map counts.
- Iterate through the remaining keys in the map and append them to the result array.

**Complexity:** Time `O(n log n + m log n)`, Space `O(n)`.

```cpp
    vector<int> sortA1ByA2(vector<int> A1, int N, vector<int> A2, int M) {
        //Your code here
        vector<int> ans;
        map<int,int> mp;
        for(auto it : A1) mp[it]++;
        for(auto it : A2){
            while(mp[it]>0){
                ans.push_back(it);
                mp[it]--;
            }
        }
        for(auto it : mp){
            while(it.second>0){
                ans.push_back(it.first);
                it.second--;
            }
        }
        return ans;
    }
    
    Link - https://practice.geeksforgeeks.org/problems/relative-sorting4323/1
```
