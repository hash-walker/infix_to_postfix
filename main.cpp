//============================================================================
// Filename: main.cpp
// Author: Hamza
// Date: 06/10/2023
// Description: This C++ program converts an infix mathematical expression to
//              postfix notation and then evaluates the expression.
//============================================================================

#include <iostream>
#include <utility>
#include <variant>
#include <string>
#include <cmath>
using namespace std;

// Define a custom stack class
class stack {
private:
    struct node {
        variant<double,string,char> data;
        node* next;
        node* prev;

        // Node constructor initializes next and prev pointers to nullptr
        node() {
            next = nullptr;
            prev = nullptr;
        }
    };

    node* headPtr;
    node* tailPtr;

public:
    // Constructor initializes an empty stack
    stack() {
        headPtr = nullptr;
        tailPtr = nullptr;
    }

    // Getter for the head pointer
    node* getHeadPtr() {
        return headPtr;
    }

    // Check if the stack is empty
    bool isEmpty();

    // Push a value onto the stack
    void push(variant<double,string, char>);

    // Pop and return the top value from the stack
    variant<double,string, char> pop();

    // Pop and return the value from the tail of the stack
    variant<double,string, char> popFromTail();

    // Display all elements in the stack
    void displayAll();

    // clear the stack
    void clear();

    // Deallocate the memory
    ~stack();
};

// Check if the stack is empty
bool stack::isEmpty() {
    if (headPtr == nullptr) {
        return true;
    }
    return false;
}

// Push a value onto the stack
void stack::push(variant<double,string, char> val) {
    // Create a new node
    node* newNode = new node;

    // If the stack is empty, set both headPtr and tailPtr to the new node
    if (headPtr == nullptr) {
        headPtr = newNode;
        tailPtr = newNode;
    }

    // Assign the value to the new node and adjust pointers
    newNode->data = std::move(val);
    newNode->next = headPtr;
    newNode->prev = tailPtr;

    headPtr->prev = newNode;
    tailPtr->next = newNode;

    headPtr = newNode;
}

// Pop and return the top value from the stack
variant<double,string, char> stack::pop() {
    node* temp = headPtr;

    if (headPtr == nullptr) {
        cout << endl << "UnderFlow";
    }

    variant<double,string, char> data = temp->data;

    // If there's only one element, set both headPtr and tailPtr to nullptr
    if (headPtr == tailPtr) {
        headPtr = nullptr;
        tailPtr = nullptr;
    }
    else {
        headPtr = temp->next;
        headPtr->prev = tailPtr;
        tailPtr->next = headPtr;
    }

    delete temp;
    return data;
}

// Pop and return the value from the tail of the stack
variant<double,string, char> stack::popFromTail() {
    node* temp = tailPtr;

    if (headPtr == nullptr) {
        cout << endl << "UnderFlow";
    }

    variant<double,string, char> data = temp->data;

    // If there's only one element, set both headPtr and tailPtr to nullptr
    if (headPtr == tailPtr) {
        headPtr = nullptr;
        tailPtr = nullptr;
    }
    else {
        tailPtr = temp->prev;
        tailPtr->next = headPtr;
        headPtr->prev = tailPtr;
    }

    delete temp;
    return data;
}

// Display all elements in the stack
void stack::displayAll() {
    node* current = tailPtr;

    if (headPtr == nullptr) {
        cout << endl << "The stack is empty";
        return;
    }

    do {
        // Use variant visit to print either a double or char
        visit([](const auto& value) {
            cout << value << " ";
        }, current->data);

        current = current->prev;
    } while (current != tailPtr);
}

void stack::clear(){
    while (!isEmpty()) {
        pop();
    }
}


// Destructor for the stack class
stack::~stack() {
    while (!isEmpty()) {
        pop();
    }
}

// Class for converting infix expressions to postfix and evaluating them
class infixToPostfixConvertor : public stack {
public:
    // Convert infix expression to postfix notation
    void infixToPostfix(string expression);

    // Evaluate the postfix expression
    double calculate();

    // Display the postfix expression
    void displayInfixToPostfix(bool);
private:
    string postfix;
    stack stack1;

    // Function to check if a character is an operand (not an operator or parenthesis)
    static bool isOperand(char i);

    // Function to check if a character is an operator (+, -, *, /, ^)
    static bool isOperator(char i);

    // Get the precedence of an operator (+ and - have lower precedence than *, /, and ^)
    static int getOperatorPrecedence(char op);

    // Check if an operator has higher or equal precedence compared to another operator
    static bool checkPrecedence(char val, char i);

    // Perform addition operation
    static double add(double num1, double num2);

    // Perform subtraction operation
    static double subtract(double num1, double num2);

    // Perform multiplication operation
    static double multiply(double num1, double num2);

    // Perform division operation
    static double divide(double num1, double num2);

    // Perform exponentiation operation
    static double power(double num1, double num2);

    // Apply the specified operator to two operands
    static double applyOperation(double firstOperand, double secondOperand, char Operator);
};

// Convert infix expression to postfix notation
void infixToPostfixConvertor::infixToPostfix(string expression) {
    // Push an opening parenthesis onto the stack to facilitate processing
    push('(');

    // Append a closing parenthesis to the expression for proper termination
    expression += ')';

    string number;

    for (char i : expression) {
        if (isOperand(i)) {
            number += i;
        }
        else if (isOperator(i)) {
            try {
                double value = stod(number);
                number = "";
                stack1.push(value);
            }
            catch (const std::invalid_argument& e) {
                stack1.push(number);
                number = "";
            }

            variant<double,string, char> value1 = getHeadPtr()->data;

            // Keep pushing operators onto stack1 until their precedence is lower
            while (checkPrecedence(get<char>(value1), i)) {
                stack1.push(pop());
                value1 = getHeadPtr()->data;
            }

            push(i);
        }
        else {
            if (i == '(') {
                push(i);
            }
            else if (i == ')') {
                try {
                    double num = stod(number);
                    number = "";
                    stack1.push(num);
                }
                catch (const std::invalid_argument& e) {
                    stack1.push(number);
                    number = "";
                }

                variant<double,string, char> value1 = getHeadPtr()->data;

                // Pop and push operators until an opening parenthesis is encountered
                while (get<char>(value1) != '(') {
                    stack1.push(pop());
                    value1 = getHeadPtr()->data;
                }

                // Pop the opening parenthesis
                pop();
            }
        }

    }
}

// Evaluate the postfix expression
double infixToPostfixConvertor::calculate() {
    variant<double,string, char> i = stack1.popFromTail();

    do {
        if (holds_alternative<double>(i)) {
            push(get<double>(i));
        }
        else if (holds_alternative<char>(i)) {
            variant<double,string, char> value2 = pop();
            variant<double,string, char> value1 = pop();

            double afterOperation = applyOperation(get<double>(value1), get<double>(value2), get<char>(i));

            cout << endl << get<double>(value1) << " " << get<double>(value2) << " After Operation " << afterOperation << " operator " << get<char>(i);

            push(afterOperation);
            cout << endl;
            displayAll();
        }

        if (stack1.isEmpty()) {
            break;
        }
        i = stack1.popFromTail();

    } while (stack1.isEmpty() || holds_alternative<char>(i) || holds_alternative<double>(i));

    variant<double,string, char> result = pop();
    return get<double>(result);
}

// Display the postfix expression
void infixToPostfixConvertor::displayInfixToPostfix(bool flag) {
    stack1.displayAll();
    if(flag){
        stack1.clear();
    }
}

// Check if a character is an operand (not an operator or parenthesis)
bool infixToPostfixConvertor::isOperand(char i) {
    if (i == '(' || i == ')' || i == '+' || i == '-' || i == '*' || i == '/' || i == '^') {
        return false;
    }
    return true;
}

// Check if a character is an operator (+, -, *, /, ^)
bool infixToPostfixConvertor::isOperator(char i) {
    if (i != '+' && i != '-' && i != '*' && i != '/' && i != '^') {
        return false;
    }
    return true;
}

// Get the precedence of an operator (+ and - have lower precedence than *, /, and ^)
int infixToPostfixConvertor::getOperatorPrecedence(char op) {
    if (op == '+' || op == '-') {
        return 1;
    }
    else if (op == '*' || op == '/') {
        return 2;
    }
    else if (op == '^') {
        return 3;
    }
    else {
        return 0;  // Default precedence for unknown operators
    }
}

// Check if an operator has higher or equal precedence compared to another operator
bool infixToPostfixConvertor::checkPrecedence(char val, char i) {
    int precedenceVal = getOperatorPrecedence(val);
    int precedenceI = getOperatorPrecedence(i);

    return precedenceVal >= precedenceI;
}

// Perform addition operation
double infixToPostfixConvertor::add(double num1, double num2) {
    return num1 + num2;
}

// Perform subtraction operation
double infixToPostfixConvertor::subtract(double num1, double num2) {
    return num1 - num2;
}

// Perform multiplication operation
double infixToPostfixConvertor::multiply(double num1, double num2) {
    return num1 * num2;
}

// Perform division operation
double infixToPostfixConvertor::divide(double num1, double num2) {
    return num1 / num2;
}

// Perform exponentiation operation
double infixToPostfixConvertor::power(double num1, double num2) {
    return pow(num1, num2);
}

// Apply the specified operator to two operands
double infixToPostfixConvertor::applyOperation(double firstOperand, double secondOperand, char Operator) {
    if (Operator == '+') {
        return add(firstOperand, secondOperand);
    }
    else if (Operator == '-') {
        return subtract(firstOperand, secondOperand);
    }
    else if (Operator == '*') {
        return multiply(firstOperand, secondOperand);
    }
    else if (Operator == '/') {
        return divide(firstOperand, secondOperand);
    }
    else {
        return power(firstOperand, secondOperand);
    }
}

int main() {
    // Create an instance of infixToPostfixConvertor
    infixToPostfixConvertor infixToPostfix;

    bool exit = false;
    string expression;
    double value;
    bool flag = false;

    while (!exit) {
        cout <<endl<< "Options:" << endl;
        cout << "1. Convert to Postfix" << endl;
        cout << "2. Convert to Postfix and Evaluate an Infix Expression" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        while(!(cin >> choice)){
            cout << endl << "Warning!! Only integer for Choice" << endl;
            cout <<endl<< "Options:" << endl;
            cout << "1. Convert to Postfix" << endl;
            cout << "2. Convert to Postfix and Evaluate an Infix Expression" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(); // Consume the newline character

        switch (choice) {
            case 1:
                cout <<endl<< "Enter an infix expression: ";

                getline(cin, expression);

                // Convert the infix expression to postfix
                infixToPostfix.infixToPostfix(expression);

                // Display the postfix expression
                cout <<endl<< "Postfix Expression: ";
                infixToPostfix.displayInfixToPostfix(true);
                break;

            case 2:

                while(!flag) {
                    flag = true;
                    cout << endl << "Enter an infix expression: ";

                    getline(cin, expression);


                    for (char c: expression) {
                        if (isalpha(c)) {
                            cout << endl << "Warning!! Your expression contains alphabets "<<endl;
                            flag = false;
                            break;
                        }
                    }
                }
                flag = false;
                cout << expression;


                // Convert the infix expression to postfix
                infixToPostfix.infixToPostfix(expression);

                // Display the postfix expression
                cout <<endl<< "Postfix Expression: ";
                infixToPostfix.displayInfixToPostfix(false);

                // Calculate and display the result
                value = infixToPostfix.calculate();
                cout << endl << "Result: " << value << endl;
                break;
            case 3:
                exit = true;
                break;
            default:
                cout <<endl<< "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
