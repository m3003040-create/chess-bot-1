CXX = g88
CXXFLAGS = -O3 -std=c++17 -march=native -flto -funroll-loops -ffast-math -DNDEBUG
LDFLAGS = -pthread -static-libstdc++ -static-libgcc

SOURCES = Bitboard.cpp MoveGen.cpp Position.cpp Search.cpp Evaluation.cpp NNUE_Architecture.cpp UCI_Interface.cpp Main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = apex_chess_v1

.PHONY: all clean profile bench

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

profile: CXXFLAGS += -pg
profile: all

bench: all
	./$(TARGET) --bench

optimize: CXXFLAGS += -fprofile-generate
optimize: all
	./$(TARGET) --bench
	$(CXX) $(CXXFLAGS) -fprofile-use $(OBJECTS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGET) gmon.out

# Специальные флаги для максимальной производительности на GitHub Actions
github-action: CXXFLAGS += -mavx2 -mbmi2 -mpopcnt
github-action: all

# Инструкция по сборке:
# 1. Поместите все .cpp файлы в одну папку.
# 2. Установите g++ (MinGW на Windows или build-essential на Linux).
# 3. Введите команду 'make all' в терминале.
# 4. Запустите готовый файл 'apex_chess_v1'.

test: all
	@echo "Running basic engine self-test..."
	@echo "isready\nquit" | ./$(TARGET)
