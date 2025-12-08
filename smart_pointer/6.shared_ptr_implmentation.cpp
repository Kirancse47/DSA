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
