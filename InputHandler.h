#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <conio.h>

class InputHandler {
private:
    vector<string> history;
    int history_index;
public:
    static std::string getUserInput() {
        std::cout << "Enter a command: ";
        std::string input;
        char ch;
        while (true) {
            if (_kbhit()) {
                ch = _getch();
                if (ch == 0 || ch == -32) {// arrow keys
                    ch = _getch();

                    if (ch == 72) {
                        //screen ls
                        for (int i = 0; i < input.size(); i++) {
                            std::cout << "\b \b";
                        }
                        std::cout << "screen -ls";
                        input = "screen -ls";
                    }
                    
                }
                else if (ch == 13) { // Enter key
                    std::cout << "\n";
                    break;
                }
                else if (ch == 8) { // Backspace
                    if (!input.empty()) {
                        input.pop_back();
                        std::cout << "\b \b";
                    }
                }
                else if (ch >= 32 && ch <= 126) { // Printable characters
                    input += ch;
                    std::cout << ch;
                }
            }
        }
        return input;
    }

    static std::vector<std::string> splitInput(const std::string& input) {
        std::stringstream ss(input);
        std::string word;
        std::vector<std::string> words;
        while (ss >> word) {
            words.push_back(word);
        }
        return words;
    }
};
