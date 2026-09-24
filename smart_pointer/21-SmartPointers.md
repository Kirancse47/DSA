# Smart Pointers & RAII — Q&A
> Source: `Kirancse47/DSA` -> `smart_pointer/` — 6 files

RAII (Resource Acquisition Is Initialization) binds resource lifecycle to object lifetime; acquiring resources in the constructor and releasing them in the destructor guarantees leak-free cleanup via stack unwinding.

| Smart Pointer | Ownership | Copyable | Overhead | Use Case |
|---|---|---|---|---|
| `unique_ptr` | Exclusive | No (Movable) | None | Default choice; strict ownership. |
| `shared_ptr` | Shared | Yes | Ref counter | Multiple owners; destroyed when count is 0. |
| `weak_ptr` | Non-owning | Yes | Ref counter | Observer; breaks `shared_ptr` reference cycles. |

`make_unique` / `make_shared` perform a single allocation for both the object and (for shared) the control block, reducing fragmentation and preventing memory leaks if an exception is thrown during allocation.
Cycles leak memory because ref counts never hit zero; `weak_ptr` observes without owning to break this. Custom deleters allow `unique_ptr` to manage C-style handles (e.g., `FILE*`). `enable_shared_from_this` safely generates a `shared_ptr` to `this`.

---

## 1. 1.smartPrt.cpp

**Q:** How do you write a basic custom smart pointer class?

**A:** RAII wrapper around a raw pointer.

**Explanation:**
- Wraps raw pointer in a class (`MyInt`).
- Deletes pointer in destructor, preventing leaks.
- Overloads `operator*` to mimic pointer dereferencing.

**Cost / notes:** Zero overhead wrapper.

```cpp
// TOPIC: Introduction To Smart Pointers

// NOTES:
// 0. Smart pointer is a class which wraps a raw pointer, to manage the life time of the pointer.
// 1. The most fundamental job of smart pointer is to remove the chances of memory leak.
// 2. It makes sure that the object is deleted if it is not reference any more.

// TYPES:
// 1. unique_ptr : 
//    Allows only one owner of the underlying pointer.

// 2. shared_ptr : 
//    Allows multiple owners of the same pointer (Reference count is maintained).

// 3. weak_ptr : 
//    It is special type of shared_ptr which doesn't count the reference.

#include<iostream>
using namespace std;

class MyInt{
    private:
        int *data;
    public:
        explicit MyInt(int *p=nullptr) { data=p; }
        ~MyInt() { delete data; }
        int& operator * () { return *data; }
};

int main(){
    int* p=new int(10);
    MyInt myint = MyInt(p);
    cout<< *myint <<endl;
    return 0;
}
```

## 2. 2.uniquePtr.cpp

**Q:** How do you safely transfer ownership of a unique pointer?

**A:** Using `std::move`.

**Explanation:**
- `unique_ptr` cannot be copied, enforcing exclusive ownership.
- Use `std::move` to transfer ownership.
- `make_unique` is preferred as it's exception-safe.

**Cost / notes:** Zero overhead compared to raw pointers.

```cpp
// TOPIC: Unique Pointer In C++

// NOTES:
// 0. unique_ptr is a class template.
// 1. unique_ptr is one of the smart pointer provided by c++11 to prevent memory leaks.
// 2. unique_ptr wraps a raw pointer in it, and de-allocates the raw pointer,
//    when unique_ptr object goes out of scope.
// 3. similar to actual pointers we can use -> and * on the object of unique_ptr, 
//    because it is overloaded in unique_ptr class.
// 4. When exception comes then also it will de-allocate the memory hence no memory leak.
// 5. Not only object we can create array of objects of unique_ptr.

// OPERATIONS:
// release, reset, swap, get, get_deleter.

#include<iostream>
using namespace std;

class Foo{
    private:
        int x;
    public:
        explicit Foo(int val){ x=val; }
        int getX() { return x; }
        ~Foo() { cout<<"Foo Destroid"<<endl; }
};

int main(){
    //Type 1
    unique_ptr<Foo> p1(new Foo(10));
    unique_ptr<Foo> p2(new Foo(100));
    //Type 2
    unique_ptr<Foo> p3=make_unique<Foo>(20);  // should use make_unique (this is exception safe)
 
    cout<<p1->getX()<<" "<<(*p2).getX()<<" "<<p3->getX()<<endl;

    //p1=p3; //Fail : this will fail because you can not copy ownership.
    unique_ptr<Foo> p4 = std::move(p1);  //Pass : because moving ownership is allowed.

    Foo* p = p3.get();  //it will give you manage object;

    Foo* p5=p3.release(); //it will release ownership and return the opbject.

    p2.reset(p5);
    cout<<p2->getX();
    return 0;
}
```

## 3. 3.sharedPtr.cpp

**Q:** What happens when you pass a `shared_ptr` by value to multiple threads?

**A:** The reference count is incremented in a thread-safe manner.

**Explanation:**
- Demonstrates multiple owners using `shared_ptr`.
- The control block (reference count) is thread-safe, but the managed object is not.
- Passing by value increments ref count, while passing by reference does not.

**Cost / notes:** Overhead of heap-allocated control block and atomic ref counting.

```cpp
// TOPIC: Shared Pointer In C++

// NOTES:
// 0. shared_ptr is a smart pointer which can share the ownership of object (managed object).
// 1. Several shared_ptr can point to the same object (managed object).
// 2. It keep a reference count to maintain how many shared_ptr are pointing to the same object.
//    and once last shared_ptr goes out of scope then the managed object gets deleted.
// 3. shared_ptr is threads safe and not thread safe. [what is this??]
//    a. control block is thread safe
//    b. managed object is not
// 4. There are three ways shared_ptr will destroyed managed object.
//    a. If the last shared_ptr goes out of scope.
//    b. If you initialize shared_ptr with some other shared_ptr.
//    c. If you reset shared_ptr.
// 5. Reference count doesn't work when we use reference or pointer of shared_ptr.

#include<iostream>
#include<memory>
#include<thread>
using namespace std;

class Foo{
    int x;
    public:
        Foo(int x):x(x){}
        int getX(){ return x; }
        ~Foo(){ cout<<"~Foo"; }
};

void fun(shared_ptr<Foo> sp){
    cout<<"fun: "<<sp.use_count()<<endl;
}

int main(){
    // Foo *f=new Foo(1);
    // cout<<f->getX();
    // delete f;

/*
    shared_ptr<Foo> sp(new Foo(100));
    cout<<sp->getX()<<endl;
    cout<<sp.use_count()<<endl;
    shared_ptr<Foo> sp1=sp; //it will increase the referance count;
    cout<<sp1.use_count()<<endl;
    shared_ptr<Foo> &sp2=sp; //but it will not increase the referance count;
    cout<<sp.use_count()<<endl;
*/

    shared_ptr<Foo> sp(new Foo(50));
    thread t1(fun, sp), t2(fun,sp), t3(fun,sp);
    cout<<"main: "<<sp.use_count()<<endl;
    t1.join();t2.join();t3.join();
    return 0;
}
```

## 4. 4.weakPtr1.cpp

**Q:** How do you access a resource managed by a `shared_ptr` without affecting its lifetime?

**A:** Using `weak_ptr` and locking it.

**Explanation:**
- `weak_ptr` observes a `shared_ptr` without increasing the reference count.
- Must call `.lock()` to obtain a `shared_ptr` before accessing the resource.
- Demonstrates checking if the resource is still valid via `expired()`.

**Cost / notes:** Same control block overhead as `shared_ptr`, plus a weak reference count.

```cpp
// TOPIC: Weak Pointer In C++

// NOTES:
// 0. If we say unique_ptr is for unique ownership and shared_ptr is for shared ownership then
//    weak_ptr is for non-ownership smart pointer.
// 1. It actually reference to an object which is managed by shared_ptr.
// 2. A weak_ptr is created as a copy of shared_ptr.
// 3. We have to convert weak_ptr to shared_ptr in order to use the managed object.
// 4. It is used to remove cyclic dependency between shared_ptr.

#include <iostream>
#include <memory>

int main() {

    auto sharedPtr = std::make_shared<int>(100);
    std::weak_ptr<int> weakPtr(sharedPtr);

    std::cout << "weakPtr.use_count(): " << weakPtr.use_count() << std::endl;
    std::cout << "sharedPtr.use_count(): " << sharedPtr.use_count() << std::endl;
    std::cout << "isWeakPtr.expired(): " << weakPtr.expired() << std::endl;

    if (std::shared_ptr<int> sharedPtr1 = weakPtr.lock()) {
        std::cout << "*sharedPtr: " << *sharedPtr << std::endl;
        std::cout << "sharedPtr1.use_count(): " << sharedPtr1.use_count() << std::endl;
    }
    else {
        std::cout << "Don't get the resource!" << std::endl;
    }

    weakPtr.reset();
    if (std::shared_ptr<int> sharedPtr1 = weakPtr.lock()) {
        std::cout << "*sharedPtr: " << *sharedPtr << std::endl;
        std::cout << "sharedPtr1.use_count(): " << sharedPtr1.use_count() << std::endl;
    }
    else {
        std::cout << "Don't get the resource!" << std::endl;
    }
    return 0;
}
```

## 5. 5.RemoveCycleDepWeakPtr.cpp

**Q:** How do you prevent a memory leak when two objects need to hold references to each other?

**A:** Break the cyclic dependency using `weak_ptr`.

**Explanation:**
- Shows a `Mother` and `Son`/`Daughter` referencing each other.
- If both use `shared_ptr`, the reference count never reaches zero (a cycle).
- Using `weak_ptr` for the back-reference allows proper destruction.

**Cost / notes:** Memory leak avoided; resources properly freed.

```cpp
#include <iostream>
#include <memory>

struct Son;
struct Daughter;

struct Mother{
  ~Mother(){
    std::cout << "Mother gone" << std::endl;
  }
  void setSon(const std::shared_ptr<Son> s ){
    mySon=s;
  }
  void setDaughter(const std::shared_ptr<Daughter> d ){
    myDaughter=d;
  }
//   std::shared_ptr<Son> mySon;
//   std::shared_ptr<Daughter> myDaughter;
// to break the cyclic dependency we need to use week pointer;
  std::weak_ptr<Son> mySon;
  std::weak_ptr<Daughter> myDaughter;

};

struct Son{
  Son(std::shared_ptr<Mother> m):myMother(m){}
  ~Son(){
    std::cout << "Son gone" << std::endl;
  }
  std::shared_ptr<const Mother> myMother;
};

struct Daughter{
  Daughter(std::shared_ptr<Mother> m):myMother(m){}
  ~Daughter(){
    std::cout << "Daughter gone" << std::endl;
  }
  std::shared_ptr<const Mother> myMother;
};

int main(){
  std::cout << std::endl;
  {
    std::shared_ptr<Mother> mother= std::shared_ptr<Mother>( new Mother);
    std::shared_ptr<Son> son= std::shared_ptr<Son>( new Son(mother) );
    std::shared_ptr<Daughter> daughter= std::shared_ptr<Daughter>( new Daughter(mother) );
    mother->setSon(son);
    mother->setDaughter(daughter);
  }
  std::cout << std::endl;
}
```

## 6. 6.shared_ptr_implmentation.cpp

**Q:** How would you implement a basic reference-counted shared pointer from scratch?

**A:** By allocating a shared integer counter on the heap.

**Explanation:**
- Uses a heap-allocated `int` to track the number of owners.
- Copy constructor and assignment operator increment the count.
- Destructor decrements the count and deletes both the resource and the counter when it hits zero.

**Cost / notes:** 2 allocations (object + counter), unlike `std::make_shared` which merges them.

```cpp
#include <iostream>

template <typename T>
class SimpleSharedPtr {
private:
    T* ptr;
    int* ref_count; // Pointer to the shared counter

    // Helper to decrement count and delete if necessary
    void release() {
        if (ref_count) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
                std::cout << "Resource destroyed.\n";
            }
        }
    }

public:
    // 1. Constructor
    explicit SimpleSharedPtr(T* p = nullptr) : ptr(p), ref_count(new int(1)) {
        if (p == nullptr) {
            // Handle null pointer case specifically if needed, 
            // but usually we still need a ref_count for safety or set it to null.
            // For simplicity here, we create a count of 1 even for null to avoid complex logic.
        } 
    }

    // 2. Copy Constructor
    SimpleSharedPtr(const SimpleSharedPtr<T>& other) {
        ptr = other.ptr;
        ref_count = other.ref_count;
        (*ref_count)++; // Increment shared count
    }

    // 3. Copy Assignment Operator
    SimpleSharedPtr<T>& operator=(const SimpleSharedPtr<T>& other) {
        if (this != &other) { // Prevent self-assignment
            release();        // Clean up current resource
            
            // Copy new resource
            ptr = other.ptr;
            ref_count = other.ref_count;
            (*ref_count)++;
        }
        return *this;
    }

    // 4. Destructor
    ~SimpleSharedPtr() {
        release();
    }

    // Accessors
    T& operator*() { return *ptr; }
    T* operator->() { return ptr; }
    
    int get_count() const { return *ref_count; }
};

// --- Usage Example ---

class Test {
public:
    void say_hello() { std::cout << "Hello from Test Object!\n"; }
};

int main() {
    // Create first pointer
    SimpleSharedPtr<Test> ptr1(new Test());
    std::cout << "Ref Count: " << ptr1.get_count() << "\n"; // Output: 1

    {
        // Create second pointer (copy)
        SimpleSharedPtr<Test> ptr2 = ptr1;
        std::cout << "Ref Count: " << ptr1.get_count() << "\n"; // Output: 2
        ptr2->say_hello();
    } // ptr2 goes out of scope here

    std::cout << "Ref Count: " << ptr1.get_count() << "\n"; // Output: 1

    return 0;
} // ptr1 goes out of scope -> Count becomes 0 -> Resource destroyed
```

