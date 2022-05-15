#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_map>
const size_t AllowableSize = 9000u;
const char FieldSize = 16;
const char RowSize = 4;
const std::array<char, FieldSize> finishField = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 0};

struct GameStateHasher;

class GameState {
public:
    explicit GameState(const std::array<char, FieldSize> &field)
            : field(field) {
        emptyPos = -1;
        for (int i = 0; i < FieldSize; i++) {
            if (field[i] == 0)
                emptyPos = i;
        }
        assert(emptyPos != -1);
    }

    //  для компоратора по умолчанию для пар (вообще сравнение по первым элементам, но в случае их равенства еще по вторым)
    //  из std::set<std::pair<int, GameState>> set из GetSolution()
    bool operator<(const GameState &) const {
        return true;
    }

    GameState(const GameState &other) = default;
    GameState(GameState &&other) = default;

    GameState &operator=(const GameState &source) = default;
    GameState &operator=(GameState &&source) = default;

    [[nodiscard]] bool IsComplete() const {
        return field == finishField;
    }

    [[nodiscard]] bool IsSolvable() const {
        return !((getInvCount() + emptyPos / RowSize + 1) % 2);
    }

    [[nodiscard]] bool CanMoveUp() const {
        return emptyPos < RowSize * (RowSize - 1);
    }

    [[nodiscard]] bool CanMoveDown() const {
        return emptyPos >= RowSize;
    }

    [[nodiscard]] bool CanMoveLeft() const {
        return emptyPos % RowSize != (RowSize - 1);
    }

    [[nodiscard]] bool CanMoveRight() const {
        return emptyPos % RowSize != 0;
    }

    [[nodiscard]] GameState MoveUp() const {
        assert(CanMoveUp());

        GameState newState(*this);
        std::swap(newState.field[emptyPos], newState.field[emptyPos + RowSize]);
        newState.emptyPos += RowSize;
        return newState;
    }

    [[nodiscard]] GameState MoveDown() const {
        assert(CanMoveDown());

        GameState newState(*this);
        std::swap(newState.field[emptyPos], newState.field[emptyPos - RowSize]);
        newState.emptyPos -= RowSize;
        return newState;
    }

    [[nodiscard]] GameState MoveLeft() const {
        assert(CanMoveLeft());

        GameState newState(*this);
        std::swap(newState.field[emptyPos], newState.field[emptyPos + 1]);
        ++newState.emptyPos;
        return newState;
    }

    [[nodiscard]] GameState MoveRight() const {
        assert(CanMoveRight());

        GameState newState(*this);
        std::swap(newState.field[emptyPos], newState.field[emptyPos - 1]);
        --newState.emptyPos;
        return newState;
    }

    bool operator==(const GameState &other) const {
        return field == other.field;
    }

private:
    [[nodiscard]] size_t getInvCount() const {
        size_t inv_count = 0;
        for (int i = 0; i < FieldSize - 1; i++) {
            for (int j = i + 1; j < FieldSize; j++) {
                if (field[i] && field[j] && field[i] > field[j])
                    inv_count++;
            }
        }
        return inv_count;
    }

    std::array<char, FieldSize> field;
    char emptyPos;

    friend struct GameStateHasher;
    friend int GetEvristika(const GameState &state);

    friend std::ostream &operator<<(std::ostream &out, const GameState &state);
};

// так как в бинарной записи чисел [0;15] значащие только последние 4 бита, можно взять хэш от них --
// последовательно записать последние 4 бита для каждой пятнашки,
// если size_of(size_t) == 8 байт, то коллизий не будет совсем
struct GameStateHasher {
public:
    size_t operator()(const GameState &state) const {
        size_t hash = 0;
        for (uint8_t i = 1u; i <= 16u; i += 2u) {
            char val = (state.field[i] << 4) + state.field[i + 1u];
            hash <<= 8;
            hash += val;
        }
        return hash;
    }
};

std::ostream &operator<<(std::ostream &out, const GameState &state) {
    for (int i = 0; i < RowSize; i++) {
        for (int j = 0; j < RowSize; j++) {
            out << static_cast<int>(state.field[i * RowSize + j]) << " ";
        }
        out << std::endl;
    }
    return out;
}

// сумма Манхеттенских расстояний каждой пятнашки до её целевой позиции
int GetEvristika(const GameState &state) {
    int countOutOfPlace = 0;
    for (int i = 0; i < FieldSize; ++i) {
        if (state.field[i] != finishField[i]) {
            ++countOutOfPlace;
        }
    }
    return countOutOfPlace;
}

std::string GetSolution(const std::array<char, FieldSize> &field) {
    GameState startState(field);

    if (!startState.IsSolvable())
        return "-1";

    std::unordered_map<GameState, char, GameStateHasher> visited;
    // first -- длина пути + эвристика, second--ситуация на поле
    std::set<std::pair<int, GameState>> set;
    set.emplace(GetEvristika(startState), startState);

    visited[startState] = 'S';

    while (true) {
        std::pair<int, GameState> currState = set.extract(set.begin()).value();

        if (currState.second.IsComplete())
            break;

        if (currState.second.CanMoveLeft()) {
            GameState newState = currState.second.MoveLeft();
            if (visited.find(newState) == visited.end())// not found
            {
                visited[newState] = 'L';
                set.emplace(GetEvristika(newState) + currState.first + 1, newState);// эвристика + путь до родителя + 1
            }
        }
        if (currState.second.CanMoveRight()) {
            GameState newState = currState.second.MoveRight();
            if (visited.find(newState) == visited.end())// not found
            {
                visited[newState] = 'R';
                set.emplace(GetEvristika(newState) + currState.first + 1, newState);// эвристика + путь до родителя + 1
            }
        }
        if (currState.second.CanMoveUp()) {
            GameState newState = currState.second.MoveUp();
            if (visited.find(newState) == visited.end())// not found
            {
                visited[newState] = 'U';
                set.emplace(GetEvristika(newState) + currState.first + 1, newState);// эвристика + путь до родителя + 1
            }
        }
        if (currState.second.CanMoveDown()) {
            GameState newState = currState.second.MoveDown();
            if (visited.find(newState) == visited.end())// not found
            {
                visited[newState] = 'D';
                set.emplace(GetEvristika(newState) + currState.first + 1, newState);// эвристика + путь до родителя + 1
            }
        }
        // если допустимый размер превыщен -- убрать половину set с наибольшими эвристиками
        if (set.size() > AllowableSize) {
            int averageEvristika = (set.begin()->first + set.rbegin()->first) / 2;
            set.erase(set.upper_bound(std::make_pair(averageEvristika, currState.second)), set.end());
        }
    }

    std::string path;
    GameState state(finishField);

    while (visited[state] != 'S') {
        char move = visited[state];
        switch (move) {
            case 'L': {
                // добавить мув-семантику
                state = state.MoveRight();
                path += 'L';
                break;
            }
            case 'R': {
                state = state.MoveLeft();
                path += 'R';
                break;
            }
            case 'U': {
                state = state.MoveDown();
                path += 'U';
                break;
            }
            case 'D': {
                state = state.MoveUp();
                path += 'D';
                break;
            }
        }
    }

    std::reverse(path.begin(), path.end());
    return path;
}

int main() {
    std::array<char, FieldSize> field{};
    for (int i = 0; i < FieldSize; ++i) {
        int value;
        std::cin >> value;
        field[i] = static_cast<char>(value);
    }

    std::string solution = GetSolution(field);
    std::cout << solution.size() << '\n'
              << solution << std::endl;
    return 0;
}
