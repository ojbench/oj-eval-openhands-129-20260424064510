
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <cctype>
#include <algorithm>

// Structure to represent a variable
struct Variable {
    std::string type;
    std::string value;
    
    // Default constructor
    Variable() : type(""), value("") {}
    
    // Constructor with parameters
    Variable(const std::string& t, const std::string& v) : type(t), value(v) {}
};

// Scope manager class
class ScopeManager {
private:
    std::vector<std::map<std::string, Variable>> scopes;
    
public:
    ScopeManager() {
        // Create global scope
        scopes.push_back(std::map<std::string, Variable>());
    }
    
    // Enter a new scope
    void indent() {
        scopes.push_back(std::map<std::string, Variable>());
    }
    
    // Exit current scope
    void dedent() {
        if (scopes.size() > 1) {  // Always keep at least one scope (global)
            scopes.pop_back();
        }
    }
    
    // Declare a variable in current scope
    bool declare(const std::string& type, const std::string& name, const std::string& value) {
        // Check if type is valid
        if (type != "int" && type != "string") {
            return false;
        }
        
        // Check if variable already exists in current scope
        if (scopes.back().find(name) != scopes.back().end()) {
            return false;
        }
        
        // For int type, validate value is a valid integer
        if (type == "int") {
            // Check if value is a valid integer
            try {
                std::size_t pos;
                std::stoi(value, &pos);
                if (pos != value.length()) {
                    return false;  // Extra characters after number
                }
            } catch (...) {
                return false;  // Not a valid integer
            }
        } 
        // For string type, validate it's enclosed in quotes
        else if (type == "string") {
            if (value.length() < 2 || value.front() != '"' || value.back() != '"') {
                return false;
            }
        }
        
        scopes.back()[name] = Variable(type, value);
        return true;
    }
    
    // Find variable in current or outer scopes
    Variable* findVariable(const std::string& name) {
        // Search from current scope (last in vector) outwards (to beginning of vector)
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                // Return pointer to the variable in the original scopes vector
                auto& scope = *it;
                return &scope[name];
            }
        }
        return nullptr;
    }
    
    // Add two values and store in result
    bool add(const std::string& result, const std::string& val1, const std::string& val2) {
        Variable* resultVar = findVariable(result);
        Variable* var1 = findVariable(val1);
        Variable* var2 = findVariable(val2);
        
        if (!resultVar || !var1 || !var2) {
            return false;
        }
        
        // Check types match
        if (resultVar->type != var1->type || resultVar->type != var2->type) {
            return false;
        }
        
        if (resultVar->type == "int") {
            try {
                int v1 = std::stoi(var1->value);
                int v2 = std::stoi(var2->value);
                resultVar->value = std::to_string(v1 + v2);
            } catch (...) {
                return false;
            }
        } else if (resultVar->type == "string") {
            // Remove quotes from values, add them, then put back quotes
            std::string v1 = var1->value.substr(1, var1->value.length() - 2);
            std::string v2 = var2->value.substr(1, var2->value.length() - 2);
            resultVar->value = "\"" + v1 + v2 + "\"";
        }
        
        return true;
    }
    
    // Self add (+= operation)
    bool selfAdd(const std::string& name, const std::string& value) {
        Variable* var = findVariable(name);
        if (!var) {
            return false;
        }
        
        if (var->type == "int") {
            // Check if value is a valid integer
            try {
                std::size_t pos;
                int val = std::stoi(value, &pos);
                if (pos != value.length()) {
                    return false;  // Extra characters after number
                }
                
                int current = std::stoi(var->value);
                var->value = std::to_string(current + val);
            } catch (...) {
                return false;
            }
        } else if (var->type == "string") {
            // Check if value is a valid string (enclosed in quotes)
            if (value.length() < 2 || value.front() != '"' || value.back() != '"') {
                return false;
            }
            
            // Remove quotes from both strings, concatenate, then add quotes back
            std::string current = var->value.substr(1, var->value.length() - 2);
            std::string addStr = value.substr(1, value.length() - 2);
            var->value = "\"" + current + addStr + "\"";
        }
        
        return true;
    }
    
    // Print variable value
    bool print(const std::string& name) {
        Variable* var = findVariable(name);
        if (!var) {
            return false;
        }
        
        if (var->type == "int") {
            std::cout << name << ":" << var->value << "\n";
        } else if (var->type == "string") {
            // Remove quotes for output
            std::string output = var->value.substr(1, var->value.length() - 2);
            std::cout << name << ":" << output << "\n";
        }
        
        return true;
    }
};

int main() {
    // Speed up I/O
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    ScopeManager manager;
    
    int n;
    std::cin >> n;
    std::cin.ignore(); // Ignore newline after n
    
    for (int i = 0; i < n; i++) {
        std::string line;
        std::getline(std::cin, line);
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        // Parse the command
        std::vector<std::string> tokens;
        std::string token;
        for (char c : line) {
            if (c == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        
        if (tokens.empty()) {
            continue;
        }
        
        std::string command = tokens[0];
        
        if (command == "Indent") {
            manager.indent();
        } 
        else if (command == "Dedent") {
            manager.dedent();
        } 
        else if (command == "Declare" && tokens.size() == 4) {
            std::string type = tokens[1];
            std::string name = tokens[2];
            std::string value = tokens[3];
            
            if (!manager.declare(type, name, value)) {
                std::cout << "Invalid operation\n";
            }
        } 
        else if (command == "Add" && tokens.size() == 4) {
            std::string result = tokens[1];
            std::string val1 = tokens[2];
            std::string val2 = tokens[3];
            
            if (!manager.add(result, val1, val2)) {
                std::cout << "Invalid operation\n";
            }
        } 
        else if (command == "SelfAdd" && tokens.size() == 3) {
            std::string name = tokens[1];
            std::string value = tokens[2];
            
            if (!manager.selfAdd(name, value)) {
                std::cout << "Invalid operation\n";
            }
        } 
        else if (command == "Print" && tokens.size() == 2) {
            std::string name = tokens[1];
            
            if (!manager.print(name)) {
                std::cout << "Invalid operation\n";
            }
        } 
        else {
            std::cout << "Invalid operation\n";
        }
    }
    
    return 0;
}
