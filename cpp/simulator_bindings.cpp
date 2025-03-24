// This file creates the Emscripten bindings between C++ and JavaScript
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <vector>
#include <string>
#include <map>
#include <sstream>

// Include the simulator code
#include "simulator.cpp"

// Emscripten bindings for the simulator functions
EMSCRIPTEN_BINDINGS(simulator_module) {
    // Function to load assembly code
    emscripten::function("loadAssembly", [](const std::string& asm_code) -> std::string {
        try {
            // Clear previous state
            label_address_map.clear();
            input_file_instr.clear();
            input_data_file_instr.clear();
            memory.clear();
            machine_codes.clear();
            pc = 0;
            
            // Parse the assembly code line by line
            std::istringstream stream(asm_code);
            std::string line;
            std::vector<std::string> lines;
            
            while (std::getline(stream, line)) {
                lines.push_back(line);
            }
            
            // Process the assembly code similar to first_parse function
            bool data_flag = 0;
            int address = -4;
            
            for (const auto& line : lines) {
                if (line.empty()) continue;
                
                std::string instr = line;
                std::string first_word = "";
                bool quotes_flag = 0;
                int colon_end = 0;
                int instr_start = 0;
                
                for (int i = 0; i < line.size(); i++) {
                    if (line[i] == ' ' && first_word != "") break;
                    else if (line[i] != ' ') first_word += line[i];
                    colon_end = i + 1;
                }
                
                if (!first_word.empty() && first_word.front() == '#') continue;
                
                if (first_word == ".data") {
                    data_flag = 1;
                    continue;
                }
                if (first_word == ".text") {
                    data_flag = 0;
                    continue;
                }
                
                if (!data_flag) address += 4;
                
                for (int i = 0; i < line.size(); i++) {
                    if (line[i] == ':' && !quotes_flag) {
                        instr_start = i + 1;
                        std::string temp = line.substr(0, i);
                        temp.erase(0, temp.find_first_not_of(' '));
                        if (label_address_map.find(temp) != label_address_map.end()) {
                            return "Error: Label " + temp + " already used above";
                        }
                        label_address_map[temp] = address;
                        break;
                    }
                    if (line[i] == '"' || line[i] == '\'' || line[i] == '#') quotes_flag = 1;
                }
                
                instr = instr.substr(instr_start);
                instr.erase(0, instr.find_first_not_of(' '));
                
                if (instr.empty()) {
                    address -= 4;
                    continue;
                }
                
                if (data_flag) input_data_file_instr.push_back(instr);
                else input_file_instr.push_back({address, instr});
            }
            
            // Process data segment
            handle_data_segment();
            
            // Process text segment
            handle_text_segment();
            
            return "Assembly code loaded successfully";
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    });

    // Function to run the entire program
    emscripten::function("runSimulation", []() -> std::string {
        try {
            if (machine_codes.empty()) {
                return "Error: No code loaded";
            }
            
            return run();
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    });

    // Function to step through one instruction
    emscripten::function("stepSimulation", []() -> std::string {
        try {
            if (machine_codes.empty()) {
                return "Error: No code loaded";
            }
            
            return step();
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    });

    // Function to reset the simulator
    emscripten::function("resetSimulation", []() -> std::string {
        try {
            reset();
            return "Simulator reset";
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    });

    // Function to get register values
    emscripten::function("showRegisters", &show_registers);

    // Function to get memory values
    emscripten::function("showMemory", &show_memory);

    // Function to get current PC
    emscripten::function("getCurrentPC", &get_pc);

    // Function to get instruction count
    emscripten::function("getInstructionCount", &get_instruction_count);
}

