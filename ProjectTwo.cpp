//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Kashyab Maharjan Aug 15, 2024
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
//============================================================================

#define NOMINMAX // Prevent conflicts with the min and max macros

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits> // For input validation
#include <algorithm> // For transform to ensure alphanumeric sorting
#include <Windows.h> // Include Windows.h for Sleep function

using namespace std;

const int GLOBAL_SLEEP_TIME = 5000; // Default time for sleep

/*
Struct to hold the course information
*/
struct Course {
    string courseId;
    string courseName;
    vector<string> preList;
};

/*
Binary Search Tree class to manage the course objects
*/
class BinarySearchTree {

private:
    struct Node {
        Course course;
        Node* right;
        Node* left;

        // Default constructor
        Node() {
            left = nullptr;
            right = nullptr;
        }

        // Initialize with a course
        Node(Course aCourse) {
            course = aCourse;
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;
    void inOrder(Node* node, vector<Course>& courseList); // Changed to collect courses in a vector
    int size = 0;

public:
    BinarySearchTree();
    void InOrder(); // Now sorts and prints courses
    void Insert(Course aCourse);
    void Remove(string courseId);
    Course Search(string courseId);
    int Size();
};

/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    this->root = nullptr;
}

/**
 * Traverse the tree in order and collect courses
 */
void BinarySearchTree::inOrder(Node* node, vector<Course>& courseList) {
    if (node == nullptr) {
        return;
    }
    inOrder(node->left, courseList);
    courseList.push_back(node->course);
    inOrder(node->right, courseList);
}

/**
 * Insert a course into the tree
 */
void BinarySearchTree::Insert(Course aCourse) {
    if (root == nullptr) {
        root = new Node(aCourse);
    }
    else {
        Node* currentNode = root;
        while (true) {
            if (aCourse.courseId < currentNode->course.courseId) {
                if (currentNode->left == nullptr) {
                    currentNode->left = new Node(aCourse);
                    break;
                }
                else {
                    currentNode = currentNode->left;
                }
            }
            else {
                if (currentNode->right == nullptr) {
                    currentNode->right = new Node(aCourse);
                    break;
                }
                else {
                    currentNode = currentNode->right;
                }
            }
        }
    }
    size++;
}

/**
 * Remove a course from the tree
 */
void BinarySearchTree::Remove(string courseId) {
    Node* parent = nullptr;
    Node* current = root;

    while (current != nullptr && current->course.courseId != courseId) {
        parent = current;
        if (courseId < current->course.courseId) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    if (current == nullptr) {
        cout << "\nCourse ID " << courseId << " not found." << endl;
        return;
    }

    if (current->left == nullptr && current->right == nullptr) {
        if (current == root) {
            root = nullptr;
        }
        else if (parent->left == current) {
            parent->left = nullptr;
        }
        else {
            parent->right = nullptr;
        }
        delete current;
    }
    else if (current->left == nullptr || current->right == nullptr) {
        Node* child = (current->left != nullptr) ? current->left : current->right;
        if (current == root) {
            root = child;
        }
        else if (parent->left == current) {
            parent->left = child;
        }
        else {
            parent->right = child;
        }
        delete current;
    }
    else {
        Node* successorParent = current;
        Node* successor = current->right;
        while (successor->left != nullptr) {
            successorParent = successor;
            successor = successor->left;
        }

        if (successorParent != current) {
            successorParent->left = successor->right;
        }
        else {
            successorParent->right = successor->right;
        }

        current->course = successor->course;
        delete successor;
    }
    size--;
}

/**
 * Search for a course in the tree
 */
Course BinarySearchTree::Search(string courseId) {
    Node* currentNode = root;

    while (currentNode != nullptr) {
        if (currentNode->course.courseId == courseId) {
            return currentNode->course;
        }
        else if (courseId < currentNode->course.courseId) {
            currentNode = currentNode->left;
        }
        else {
            currentNode = currentNode->right;
        }
    }

    return Course(); // Return an empty course if not found
}

/**
 * Print courses in alphanumeric order
 */
void BinarySearchTree::InOrder() {
    vector<Course> courseList;
    inOrder(root, courseList);

    // Sort the courses alphanumerically
    sort(courseList.begin(), courseList.end(), [](const Course& a, const Course& b) {
        return a.courseId < b.courseId;
        });

    // Print the sorted list
    for (const Course& course : courseList) {
        cout << course.courseId << ", " << course.courseName << endl;
    }
}

int BinarySearchTree::Size() {
    return size;
}

/*
Helper function to split a string by a delimiter
*/
vector<string> Split(string lineFeed) {
    vector<string> tokens;
    string token;
    for (char ch : lineFeed) {
        if (ch == ',') {
            tokens.push_back(token);
            token.clear();
        }
        else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}

/*
Function to load courses from a CSV file
*/
void loadCourses(string csvPath, BinarySearchTree* courseList) {
    ifstream inFile(csvPath); // Input file stream

    if (!inFile.is_open()) { // Error handling if the file can't be opened
        cout << "Could not open file: " << csvPath << ". Please check the file path and try again." << endl;
        return;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue; // Skip empty lines

        vector<string> tokens = Split(line);

        if (tokens.size() < 2) { // Error handling for improperly formatted lines
            cout << "\nError: Skipping misformatted line: " << line << endl;
            continue;
        }

        Course aCourse;
        aCourse.courseId = tokens[0];
        aCourse.courseName = tokens[1];

        for (unsigned int i = 2; i < tokens.size(); i++) {
            aCourse.preList.push_back(tokens[i]);
        }

        // Insert the course into the binary search tree
        courseList->Insert(aCourse);
    }

    inFile.close(); // Close the file
}

/*
Function to display course information
*/
void displayCourse(const Course& aCourse) {
    cout << aCourse.courseId << ", " << aCourse.courseName << endl;
    cout << "Prerequisites: ";

    if (aCourse.preList.empty()) { // If the list is empty, then there are no prerequisites
        cout << "No prerequisites" << endl;
    }
    else {
        for (size_t i = 0; i < aCourse.preList.size(); i++) {
            cout << aCourse.preList[i];
            if (i < aCourse.preList.size() - 1) { // Put a comma for any elements greater than 1
                cout << ", ";
            }
        }
        cout << endl;
    }
}

/*
Function to force the case of any string to uppercase
*/
void convertCase(string& toConvert) {
    for (char& ch : toConvert) {
        ch = toupper(ch);
    }
}

int main(int argc, char* argv[]) {
    // Process command line arguments
    string csvPath, aCourseKey;

    if (argc >= 2) {
        csvPath = argv[1];
    }
    else {
        csvPath = "CS 300 ABCU_Advising_Program_Input.csv";
    }

    // Define a tree to hold all the courses
    BinarySearchTree* courseList = new BinarySearchTree();

    Course course;
    int choice = 0;

    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Courses" << endl;
        cout << "  2. Display All Courses" << endl;
        cout << "  3. Find Course" << endl;
        cout << "  4. Remove Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";

        cin >> choice;

        // Input validation
        if (cin.fail() || !(choice == 1 || choice == 2 || choice == 3 || choice == 4 || choice == 9)) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "Invalid choice. Please enter a number between 1 and 9." << endl;
            continue;
        }

        switch (choice) {
        case 1:
            // Load the courses into the tree
            loadCourses(csvPath, courseList);
            cout << courseList->Size() << " courses read" << endl;
            Sleep(GLOBAL_SLEEP_TIME);
            break;

        case 2:
            courseList->InOrder();
            break;

        case 3:
            cout << "\nEnter course ID: ";
            cin >> aCourseKey;
            convertCase(aCourseKey); // Convert the case of user input
            course = courseList->Search(aCourseKey);
            if (!course.courseId.empty()) {
                displayCourse(course);
            }
            else {
                cout << "\nCourse ID " << aCourseKey << " not found." << endl;
            }
            Sleep(GLOBAL_SLEEP_TIME);
            break;

        case 4:
            cout << "\nEnter course ID to delete: ";
            cin >> aCourseKey;
            convertCase(aCourseKey); // Convert the case of user input
            courseList->Remove(aCourseKey);
            Sleep(GLOBAL_SLEEP_TIME);
            break;

        case 9:
            cout << "Goodbye." << endl;
            Sleep(GLOBAL_SLEEP_TIME);
            break;
        }

        // Clear the input buffer for the next loop
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    delete courseList; // Clean up memory

    return 0;
}
