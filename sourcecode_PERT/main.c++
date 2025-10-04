#include <iostream>
#include <sstream>
#include "pert.h"

int main() {
    // Welcome message
    std::cout << "========================================\n";
    std::cout << "    Welcome to PERT Model System\n";
    std::cout << "         Author: ChenHongtian\n";
    std::cout << "========================================\n\n";
    
    PERT pert;
    
    // Mode selection
    std::cout << "Select Mode:\n";
    std::cout << "1. Use Default Data (A-N)\n";
    std::cout << "2. Manual Input Data\n";
    std::cout << "Please choose (1 or 2): ";
    
    int choice;
    std::cin >> choice;
    
    if (choice == 1) {
        // Use default data
        pert.addActivity("A", 2, {});
        pert.addActivity("B", 4, {"A"});
        pert.addActivity("C", 10, {"B"});
        pert.addActivity("D", 6, {"C"});
        pert.addActivity("E", 4, {"C"});
        pert.addActivity("F", 5, {"E"});
        pert.addActivity("G", 7, {"D"});
        pert.addActivity("H", 9, {"E", "G"});
        pert.addActivity("I", 7, {"C"});
        pert.addActivity("J", 8, {"F", "I"});
        pert.addActivity("K", 4, {"J"});
        pert.addActivity("L", 5, {"J"});
        pert.addActivity("M", 2, {"H"});
        pert.addActivity("N", 6, {"K", "L"});

        if (!pert.calculate()) {
            std::cerr << "Calculation failed\n";
            return 1;
        }
        pert.printActivities();
    } else {
        // Manual input mode - directly enter user input
        std::cout << "\n=== Manual Input Mode ===\n";
        std::cout << "Format: Activity_ID Duration [Predecessor1] [Predecessor2] ...\n";
        std::cout << "Example: A 5 or B 3 A or C 7 A B\n";
        std::cout << "Type 'done' to finish input\n\n";
        
        std::string input;
        std::cin.ignore(); // Clear newline after choice
        
        while (true) {
            std::cout << "Activity " << (pert.find("A") == nullptr ? 1 : pert.getProjectDuration() + 1) << ": ";
            std::getline(std::cin, input);
            
            if (input == "done" || input == "q" || input == "quit") {
                break;
            }
            
            if (input.empty()) {
                std::cout << "Please enter activity info or type 'done' to finish\n";
                continue;
            }
            
            std::istringstream iss(input);
            std::vector<std::string> tokens;
            std::string token;
            
            while (iss >> token) {
                tokens.push_back(token);
            }
            
            if (tokens.size() < 2) {
                std::cout << "Format error! Need at least activity name and duration\n";
                continue;
            }
            
            std::string activityId = tokens[0];
            float duration;
            
            try {
                duration = std::stof(tokens[1]);
            } catch (...) {
                std::cout << "Duration must be a number!\n";
                continue;
            }
            
            std::vector<std::string> predecessors;
            for (size_t i = 2; i < tokens.size(); ++i) {
                predecessors.push_back(tokens[i]);
            }
            
            if (pert.find(activityId) != nullptr) {
                std::cout << "Activity '" << activityId << "' already exists!\n";
                continue;
            }
            
            if (duration < 0) {
                std::cout << "Duration cannot be negative!\n";
                continue;
            }
            
            if (pert.addActivity(activityId, duration, predecessors)) {
                std::cout << "✓ Successfully added: " << activityId << " (Duration: " << duration << ")";
                if (!predecessors.empty()) {
                    std::cout << " [Predecessors: ";
                    for (size_t i = 0; i < predecessors.size(); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << predecessors[i];
                    }
                    std::cout << "]";
                }
                std::cout << "\n";
            }
        }
        
        std::cout << "\n=== PERT Analysis Results ===\n";
        if (!pert.calculate()) {
            std::cerr << "Calculation failed\n";
            return 1;
        }
        pert.printActivities();
        return 0;
    }
    
    // If default data is selected, ask if user wants to add custom activities
    std::cout << "\nDo you want to add custom activities? (y/n): ";
    char addMore;
    std::cin >> addMore;
    
    if (addMore == 'y' || addMore == 'Y') {
        std::cout << "\n========================\n";
        std::cout << "Now you can input your own activity data\n";
        std::cout << "Format: Activity_ID Duration [Predecessor1] [Predecessor2] ...\n";
        std::cout << "Example: O 3 or P 5 A or Q 7 A B\n";
        std::cout << "Type 'done' to finish input\n";
        std::cout << "========================\n";
        
        std::string input;
        int userActivityCount = 0;
        std::cin.ignore(); // Clear newline after choice
        
        while (true) {
            std::cout << "\nActivity " << (userActivityCount + 1) << ": ";
            std::getline(std::cin, input);
            
            if (input == "done" || input == "q" || input == "quit") {
                break;
            }
            
            if (input.empty()) {
                std::cout << "Please enter activity info or type 'done' to finish\n";
                continue;
            }
            
            std::istringstream iss(input);
            std::vector<std::string> tokens;
            std::string token;
            
            while (iss >> token) {
                tokens.push_back(token);
            }
            
            if (tokens.size() < 2) {
                std::cout << "Format error! Need at least activity name and duration\n";
                continue;
            }
            
            std::string activityId = tokens[0];
            float duration;
            
            try {
                duration = std::stof(tokens[1]);
            } catch (...) {
                std::cout << "Duration must be a number!\n";
                continue;
            }
            
            std::vector<std::string> predecessors;
            for (size_t i = 2; i < tokens.size(); ++i) {
                predecessors.push_back(tokens[i]);
            }
            
            if (pert.find(activityId) != nullptr) {
                std::cout << "Activity '" << activityId << "' already exists!\n";
                continue;
            }
            
            if (duration < 0) {
                std::cout << "Duration cannot be negative!\n";
                continue;
            }
            
            if (pert.addActivity(activityId, duration, predecessors)) {
                std::cout << "✓ Successfully added: " << activityId << " (Duration: " << duration << ")";
                if (!predecessors.empty()) {
                    std::cout << " [Predecessors: ";
                    for (size_t i = 0; i < predecessors.size(); ++i) {
                        if (i > 0) std::cout << ", ";
                        std::cout << predecessors[i];
                    }
                    std::cout << "]";
                }
                std::cout << "\n";
                userActivityCount++;
            }
        }
        
        if (userActivityCount > 0) {
            std::cout << "\nAdded " << userActivityCount << " new activities\n";
            std::cout << "Recalculating PERT analysis...\n\n";
            
            if (!pert.calculate()) {
                std::cerr << "Recalculation failed: Please check input data\n\n";
                std::cerr << "Original data is still valid:\n";
            } else {
                std::cout << "=== Updated PERT Analysis Results ===\n";
            }
            pert.printActivities();
        }
    }
    
    return 0;
}