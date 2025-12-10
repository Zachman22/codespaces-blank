#include <iostream>
#include <string>

class Calculator {
public:
    double add(double a, double b) {
        return a + b;
    }
    
    double subtract(double a, double b) {
        return a - b;
    }
    
    double multiply(double a, double b) {
        return a * b;
    }
    
    double divide(double a, double b) {
        if (b == 0) {
            std::cerr << "Error: Division by zero!" << std::endl;
            return 0;
        }
        return a / b;
    }
};

int main() {
    Calculator calc;
    
    std::cout << "=== C++ Calculator Demo ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "10 + 5 = " << calc.add(10, 5) << std::endl;
    std::cout << "10 - 5 = " << calc.subtract(10, 5) << std::endl;
    std::cout << "10 * 5 = " << calc.multiply(10, 5) << std::endl;
    std::cout << "10 / 5 = " << calc.divide(10, 5) << std::endl;
    
    std::cout << std::endl;
    std::cout << "Complex calculation: (20 + 10) * 3 / 2 = " 
              << calc.divide(calc.multiply(calc.add(20, 10), 3), 2) << std::endl;
    
    return 0;
}
