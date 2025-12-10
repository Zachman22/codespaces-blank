// Example C++ program to test the IDE
// This demonstrates basic features that the IDE can compile

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Simple class example
class Calculator {
private:
    double result;
    
public:
    Calculator() : result(0.0) {}
    
    double add(double a, double b) {
        result = a + b;
        return result;
    }
    
    double subtract(double a, double b) {
        result = a - b;
        return result;
    }
    
    double multiply(double a, double b) {
        result = a * b;
        return result;
    }
    
    double divide(double a, double b) {
        if (b == 0) {
            std::cerr << "Error: Division by zero!" << std::endl;
            return 0;
        }
        result = a / b;
        return result;
    }
    
    double getResult() const {
        return result;
    }
};

// Function to demonstrate STL usage
void demonstrateSTL() {
    std::cout << "\n=== STL Demonstration ===" << std::endl;
    
    std::vector<int> numbers = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    
    std::cout << "Original vector: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // Sort the vector
    std::sort(numbers.begin(), numbers.end());
    
    std::cout << "Sorted vector: ";
    for (int num : numbers) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // Find an element
    auto it = std::find(numbers.begin(), numbers.end(), 5);
    if (it != numbers.end()) {
        std::cout << "Found 5 at position: " << (it - numbers.begin()) << std::endl;
    }
}

// Function to demonstrate templates
template<typename T>
T getMax(T a, T b) {
    return (a > b) ? a : b;
}

// Main function
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  Hybrid IDE Test Program" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Test Calculator class
    std::cout << "\n=== Calculator Test ===" << std::endl;
    Calculator calc;
    
    std::cout << "10 + 5 = " << calc.add(10, 5) << std::endl;
    std::cout << "20 - 8 = " << calc.subtract(20, 8) << std::endl;
    std::cout << "7 * 6 = " << calc.multiply(7, 6) << std::endl;
    std::cout << "100 / 4 = " << calc.divide(100, 4) << std::endl;
    std::cout << "Last result: " << calc.getResult() << std::endl;
    
    // Test division by zero
    std::cout << "10 / 0 = " << calc.divide(10, 0) << std::endl;
    
    // Test STL
    demonstrateSTL();
    
    // Test templates
    std::cout << "\n=== Template Function Test ===" << std::endl;
    std::cout << "Max of 10 and 20: " << getMax(10, 20) << std::endl;
    std::cout << "Max of 3.14 and 2.71: " << getMax(3.14, 2.71) << std::endl;
    std::cout << "Max of 'a' and 'z': " << getMax('a', 'z') << std::endl;
    
    // Test string operations
    std::cout << "\n=== String Operations ===" << std::endl;
    std::string greeting = "Hello from Hybrid IDE!";
    std::cout << "String: " << greeting << std::endl;
    std::cout << "Length: " << greeting.length() << std::endl;
    std::cout << "Uppercase first char: " << static_cast<char>(toupper(greeting[0])) << std::endl;
    
    // Test loops and arrays
    std::cout << "\n=== Loop Test ===" << std::endl;
    std::cout << "Fibonacci sequence (first 10 numbers): ";
    int fib[10];
    fib[0] = 0;
    fib[1] = 1;
    for (int i = 2; i < 10; i++) {
        fib[i] = fib[i-1] + fib[i-2];
    }
    for (int i = 0; i < 10; i++) {
        std::cout << fib[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "  All tests completed successfully!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
