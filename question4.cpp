#include <iostream>
#include <string>
using namespace std;

// A cell is high-risk if its people count reaches riskLimit.
// Each pair of selected high-risk cells needs one extra unit.
// Row and column numbers in the output start at 1.

const int MAX_ROWS = 10;
const int MAX_COLS = 10;
const long long MAX_INPUT = 1000000000LL;
int rows, cols;
long long people[MAX_ROWS][MAX_COLS];
long long unitsNeeded[MAX_ROWS][MAX_COLS];
bool selected[MAX_ROWS][MAX_COLS] = {};
bool bestPlan[MAX_ROWS][MAX_COLS] = {};
bool rowNeedsRescue[MAX_ROWS] = {};
int rowCount[MAX_ROWS] = {};
long long peopleLeft[MAX_ROWS * MAX_COLS + 1] = {};
long long totalUnits, riskLimit;
long long maxRescued = -1, unitsForBestPlan = 0;

bool readMatrix(int pos, bool isPeople) {
    if (pos == rows * cols) return true;
    int row = pos / cols, col = pos % cols;
    long long value;
    if (!(cin >> value) || value < -1 || value > MAX_INPUT) return false;
    if (isPeople) people[row][col] = value;
    else unitsNeeded[row][col] = value;
    return readMatrix(pos + 1, isPeople);
}

void loadSample(int pos) {
    const long long samplePeople[16] = {
        8, 12, -1, 6, 5, 10, 7, -1,
        -1, 9, 15, 4, 6, -1, 8, 11
    };
    const long long sampleUnits[16] = {
        2, 3, -1, 2, 1, 3, 2, -1,
        -1, 2, 4, 1, 2, -1, 2, 3
    };
    if (pos == 16) return;
    people[pos / cols][pos % cols] = samplePeople[pos];
    unitsNeeded[pos / cols][pos % cols] = sampleUnits[pos];
    loadSample(pos + 1);
}

bool validateMatrices(int pos) {
    if (pos == rows * cols) return true;
    int row = pos / cols, col = pos % cols;
    if ((people[row][col] == -1) != (unitsNeeded[row][col] == -1))
        return false;
    if (people[row][col] > 0) rowNeedsRescue[row] = true;
    return validateMatrices(pos + 1);
}

void countPeopleLeft(int pos) {
    if (pos == rows * cols) {
        peopleLeft[pos] = 0;
        return;
    }
    countPeopleLeft(pos + 1);
    long long value = people[pos / cols][pos % cols];
    peopleLeft[pos] = peopleLeft[pos + 1] + (value > 0 ? value : 0);
}

// Find the cheapest remaining area in this row.
long long minRowCost(int row, int col, int nextPos,
                     int riskCount, long long cheapest) {
    if (col == cols) return cheapest;
    if (row * cols + col >= nextPos && people[row][col] != -1) {
        long long cost = unitsNeeded[row][col];
        if (people[row][col] >= riskLimit) cost += riskCount;
        if (cost < cheapest) cheapest = cost;
    }
    return minRowCost(row, col + 1, nextPos, riskCount, cheapest);
}

bool canCoverRows(int row, int nextPos, int riskCount, long long unitsLeft) {
    if (row == rows) return true;
    if (rowNeedsRescue[row] && rowCount[row] == 0) {
        long long cheapest = minRowCost(row, 0, nextPos, riskCount, totalUnits + 1);
        if (cheapest > unitsLeft) return false;
        unitsLeft -= cheapest;
    }
    // Future high-risk pairs can only increase this minimum cost.
    return canCoverRows(row + 1, nextPos, riskCount, unitsLeft);
}

void savePlan(int pos) {
    if (pos == rows * cols) return;
    bestPlan[pos / cols][pos % cols] = selected[pos / cols][pos % cols];
    savePlan(pos + 1);
}

void rescueAreas(int pos, long long unitsUsed, long long rescued, int riskCount) {
    long long possibleTotal = rescued + peopleLeft[pos];
    if (possibleTotal < maxRescued) return;
    if (possibleTotal == maxRescued && unitsUsed >= unitsForBestPlan) return;
    if (!canCoverRows(0, pos, riskCount, totalUnits - unitsUsed))
        return;

    if (pos == rows * cols) {
        if (rescued > maxRescued ||
            (rescued == maxRescued && unitsUsed < unitsForBestPlan)) {
            maxRescued = rescued;
            unitsForBestPlan = unitsUsed;
            savePlan(0);
        }
        return;
    }

    int row = pos / cols, col = pos % cols;
    if (people[row][col] == -1) {
        rescueAreas(pos + 1, unitsUsed, rescued, riskCount);
        return;
    }

    bool highRisk = people[row][col] >= riskLimit;
    // This high-risk area makes one new pair with each earlier high-risk area.
    long long cost = unitsNeeded[row][col] + (highRisk ? riskCount : 0);
    if (cost <= totalUnits - unitsUsed) {
        selected[row][col] = true;
        ++rowCount[row];
        rescueAreas(pos + 1, unitsUsed + cost, rescued + people[row][col],
                    riskCount + (highRisk ? 1 : 0));
        --rowCount[row];
        selected[row][col] = false;
    }
    rescueAreas(pos + 1, unitsUsed, rescued, riskCount);
}

void printAreas(int pos) {
    if (pos == rows * cols) return;
    int row = pos / cols, col = pos % cols;
    if (bestPlan[row][col])
        cout << "(" << row + 1 << ", " << col + 1 << ") : "
             << people[row][col] << " people, " << unitsNeeded[row][col]
             << " base units\n";
    printAreas(pos + 1);
}

void printPlan(int pos) {
    if (pos == rows * cols) return;
    int row = pos / cols, col = pos % cols;
    if (people[row][col] == -1) cout << "-1";
    else cout << (bestPlan[row][col] ? 1 : 0);
    cout << (col == cols - 1 ? '\n' : ' ');
    printPlan(pos + 1);
}

int main() {
    int mode;
    cout << "Enter 1 for the sample matrices or 2 for custom matrices: ";
    if (!(cin >> mode) || (mode != 1 && mode != 2)) {
        cout << "Invalid matrix choice.\n";
        return 0;
    }

    if (mode == 1) {
        rows = cols = 4;
        loadSample(0);
    } else {
        cout << "Enter rows and columns (1 to 10 each): ";
        if (!(cin >> rows >> cols) || rows < 1 || rows > MAX_ROWS ||
            cols < 1 || cols > MAX_COLS) {
            cout << "Invalid matrix dimensions.\n";
            return 0;
        }
        cout << "Enter the people matrix row by row (-1 = inaccessible):\n";
        if (!readMatrix(0, true)) {
            cout << "Invalid people matrix; values must be -1 to 1000000000.\n";
            return 0;
        }
        cout << "Enter the required-units matrix row by row:\n";
        if (!readMatrix(0, false)) {
            cout << "Invalid units matrix; values must be -1 to 1000000000.\n";
            return 0;
        }
    }

    if (!validateMatrices(0)) {
        cout << "Inaccessible cells must be -1 in both matrices.\n";
        return 0;
    }
    cout << "Enter available rescue units (0 to 1000000000): ";
    if (!(cin >> totalUnits) || totalUnits < 0 || totalUnits > MAX_INPUT) {
        cout << "Invalid rescue capacity.\n";
        return 0;
    }
    cout << "Enter the minimum people count for a high-risk cell (1 to 1000000000): ";
    if (!(cin >> riskLimit) || riskLimit < 1 ||
        riskLimit > MAX_INPUT) {
        cout << "Invalid high-risk threshold.\n";
        return 0;
    }
    cout << "Each pair of selected high-risk cells costs one extra rescue unit.\n";
    countPeopleLeft(0);
    rescueAreas(0, 0, 0, 0);
    if (maxRescued == -1) {
        cout << "No valid rescue allocation exists.\n";
        return 0;
    }
    cout << "Selected positions (row, column):\n";
    printAreas(0);
    cout << "Selection matrix (1 = selected, 0 = skipped, -1 = inaccessible):\n";
    printPlan(0);
    cout << "Total people rescued: " << maxRescued << '\n';
    cout << "Total rescue units used (including high-risk extras): " << unitsForBestPlan << '\n';
    cout << "Remaining rescue units: " << totalUnits - unitsForBestPlan << '\n';
    return 0;
}
