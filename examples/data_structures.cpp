#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// Simple Student class
class Student {
private:
    std::string name;
    int id;
    double gpa;

public:
    Student(std::string n, int i, double g) : name(n), id(i), gpa(g) {}
    
    void display() const {
        std::cout << "ID: " << id << " | Name: " << name << " | GPA: " << gpa << std::endl;
    }
    
    double getGPA() const { return gpa; }
    std::string getName() const { return name; }
};

int main() {
    std::cout << "=== Student Management System ===" << std::endl;
    std::cout << std::endl;
    
    // Create a vector of students
    std::vector<Student> students;
    students.push_back(Student("Alice Johnson", 101, 3.8));
    students.push_back(Student("Bob Smith", 102, 3.5));
    students.push_back(Student("Charlie Brown", 103, 3.9));
    students.push_back(Student("Diana Prince", 104, 4.0));
    
    std::cout << "All Students:" << std::endl;
    for (const auto& student : students) {
        student.display();
    }
    
    // Find student with highest GPA
    auto maxStudent = std::max_element(students.begin(), students.end(),
        [](const Student& a, const Student& b) {
            return a.getGPA() < b.getGPA();
        });
    
    std::cout << std::endl;
    std::cout << "Top Student: " << maxStudent->getName() 
              << " with GPA: " << maxStudent->getGPA() << std::endl;
    
    return 0;
}
