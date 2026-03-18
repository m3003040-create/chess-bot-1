#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <atomic>

class EngineController {
public:
    std::unique_ptr<Position> mainPosition;
    std::unique_ptr<Searcher> engineSearch;
    std::atomic<bool> isSearching;
    std::string engineName = "ApexChess v1.0.0 (Neural-Hybrid)";

    EngineController() : isSearching(false) {
        mainPosition = std::make_unique<Position>();
        engineSearch = std::make_unique<Searcher>(*mainPosition);
        init_all_tables();
        reset_board(mainPosition->board);
    }

    void start_uci_loop() {
        std::cout << "id name " << engineName << std::endl;
        std::cout << "id author ApexTeam" << std::endl;
        std::cout << "option name Hash type spin default 64 min 1 max 1024" << std::endl;
        std::cout << "option name Threads type spin default 1 min 1 max 128" << std::endl;
        std::cout << "uciok" << std::endl;

        std::string inputLine;
        while (std::getline(std::cin, inputLine)) {
            if (inputLine == "isready") {
                std::cout << "readyok" << std::endl;
            } else if (inputLine == "quit") {
                break;
            } else if (inputLine.substr(0, 8) == "position") {
                handle_position_command(inputLine);
            } else if (inputLine.substr(0, 2) == "go") {
                handle_go_command(inputLine);
            } else if (inputLine == "stop") {
                isSearching = false;
            } else if (inputLine == "ucinewgame") {
                reset_board(mainPosition->board);
            }
        }
    }

private:
    void handle_position_command(const std::string& cmd) {
        if (cmd.find("startpos") != std::string::npos) {
            reset_board(mainPosition->board);
        }
        size_t movesIdx = cmd.find("moves");
        if (movesIdx != std::string::npos) {
            std::string movesStr = cmd.substr(movesIdx + 6);
            std::istringstream iss(movesStr);
            std::string moveToken;
            while (iss >> moveToken) {
                Move m = parse_uci_move(moveToken);
                if (m.data != 0) mainPosition->make_move(m);
            }
        }
    }

    void handle_go_command(const std::string& cmd) {
        int depth = 12;
        if (cmd.find("depth") != std::string::npos) {
            std::string dStr = cmd.substr(cmd.find("depth") + 6);
            depth = std::stoi(dStr);
        }
        isSearching = true;
        std::thread searchThread([this, depth]() {
            engineSearch->iterative_deepening(depth);
            isSearching = false;
        });
        searchThread.detach();
    }

    Move parse_uci_move(const std::string& mStr) {
        MoveList ml;
        generate_all_moves(mainPosition->board, ml);
        for (int i = 0; i < ml.count; i++) {
            std::string current = square_to_string(ml.moves[i].from()) + square_to_string(ml.moves[i].to());
            if (current == mStr.substr(0, 4)) return ml.moves[i];
        }
        return Move();
    }

    std::string square_to_string(int sq) {
        std::string s = "";
        s += (char)('a' + (sq % 8));
        s += (char)('1' + (sq / 8));
        return s;
    }
};

int main(int argc, char* argv[]) {
    auto start_time = std::chrono::high_resolution_clock::now();
    std::cout << "ApexChess: Initializing Core Subsystems..." << std::endl;

    EngineController controller;

    if (argc > 1 && std::string(argv[1]) == "--bench") {
        std::cout << "Running Performance Benchmark..." << std::endl;
        controller.engineSearch->iterative_deepening(10);
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "Benchmark Finished in " << duration.count() << "ms" << std::endl;
        return 0;
    }

    controller.start_uci_loop();
    return 0;
}

