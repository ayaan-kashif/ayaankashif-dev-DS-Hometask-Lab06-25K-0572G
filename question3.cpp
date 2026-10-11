#include <iostream>
#include <string>
using namespace std;

// Meals: 1 = Breakfast, 2 = Lunch, 3 = Dinner. Priority: 1 = high, 0 = low.
// Each meal has its own stock, reserve, and total allocation limits.
// A rule means: if A gets at least T tokens, B can get at most L tokens.
// The high-priority total must lead by the threshold, or by 1 when it is zero.

const int MAX_STUDENTS = 20;
const int MAX_RULES = 50;
const int MAX_TOKENS = 100;
const int MAX_TOTAL = MAX_STUDENTS * MAX_TOKENS;

struct Student {
    int meal;
    int minimum;
    int maximum;
    int priority;
};

struct Rule {
    int firstStudent;
    int trigger;
    int secondStudent;
    int limit;
};

Student students[MAX_STUDENTS];
Rule rules[MAX_RULES];
int studentCount, ruleCount, requiredGap;
int stock[3], reserve[3], mealMinimum[3], mealMaximum[3];
int allocation[MAX_STUDENTS], bestAllocation[MAX_STUDENTS];
int mealTotal[3] = {0, 0, 0};
int bestMealTotal[3] = {0, 0, 0};
int bestEmergency = 3 * MAX_TOTAL + 1;
int bestHighTotal = 0, bestLowTotal = 0;
bool found = false;

string mealName(int meal) {
    if (meal == 0) return "Breakfast";
    if (meal == 1) return "Lunch";
    return "Dinner";
}

bool readMeals(int meal) {
    if (meal == 3) return true;

    cout << mealName(meal)
         << " (regular stock, emergency reserve, minimum total, maximum total): ";
    if (!(cin >> stock[meal] >> reserve[meal]
              >> mealMinimum[meal] >> mealMaximum[meal])) {
        return false;
    }

    if (stock[meal] < 0 || stock[meal] > MAX_TOTAL ||
        reserve[meal] < 0 || reserve[meal] > MAX_TOTAL ||
        mealMinimum[meal] < 0 ||
        mealMaximum[meal] < mealMinimum[meal] ||
        mealMaximum[meal] > MAX_TOTAL) {
        return false;
    }
    return readMeals(meal + 1);
}

bool readStudents(int index) {
    if (index == studentCount) return true;

    cout << "Student " << index + 1
         << " (category 1..3, minimum, maximum, priority 0/1): ";
    if (!(cin >> students[index].meal >> students[index].minimum
              >> students[index].maximum >> students[index].priority)) {
        return false;
    }

    if (students[index].meal < 1 || students[index].meal > 3 ||
        students[index].minimum < 0 ||
        students[index].maximum < students[index].minimum ||
        students[index].maximum > MAX_TOKENS ||
        students[index].priority < 0 || students[index].priority > 1) {
        return false;
    }
    --students[index].meal;
    return readStudents(index + 1);
}

bool readRules(int index) {
    if (index == ruleCount) return true;

    cout << "Rule " << index + 1 << " (student A, trigger T, student B, limit L): ";
    if (!(cin >> rules[index].firstStudent >> rules[index].trigger
              >> rules[index].secondStudent >> rules[index].limit)) {
        return false;
    }

    if (rules[index].firstStudent < 1 ||
        rules[index].firstStudent > studentCount ||
        rules[index].secondStudent < 1 ||
        rules[index].secondStudent > studentCount ||
        rules[index].trigger < 0 || rules[index].trigger > MAX_TOKENS ||
        rules[index].limit < 0 || rules[index].limit > MAX_TOKENS) {
        return false;
    }
    --rules[index].firstStudent;
    --rules[index].secondStudent;
    return readRules(index + 1);
}

bool checkStudents(int index, int previous, int amount) {
    if (previous == index) return true;

    if (students[previous].meal == students[index].meal &&
        allocation[previous] == amount) {
        return false;
    }
    if (students[index].priority > students[previous].priority &&
        amount < allocation[previous]) {
        return false;
    }
    if (students[index].priority < students[previous].priority &&
        amount > allocation[previous]) {
        return false;
    }
    return checkStudents(index, previous + 1, amount);
}

bool checkRules(int ruleIndex, int assignedCount) {
    if (ruleIndex == ruleCount) return true;

    int first = rules[ruleIndex].firstStudent;
    int second = rules[ruleIndex].secondStudent;
    if (first < assignedCount && second < assignedCount &&
        allocation[first] >= rules[ruleIndex].trigger &&
        allocation[second] > rules[ruleIndex].limit) {
        return false;
    }
    return checkRules(ruleIndex + 1, assignedCount);
}

int remainingMealMinimum(int index, int meal) {
    if (index == studentCount) return 0;
    int amount = students[index].meal == meal
                     ? students[index].minimum : 0;
    return amount + remainingMealMinimum(index + 1, meal);
}

int remainingMealMaximum(int index, int meal) {
    if (index == studentCount) return 0;
    int amount = students[index].meal == meal
                     ? students[index].maximum : 0;
    return amount + remainingMealMaximum(index + 1, meal);
}

bool checkMeals(int meal, int nextStudent) {
    if (meal == 3) return true;

    int smallestTotal = mealTotal[meal] +
                        remainingMealMinimum(nextStudent, meal);
    int largestTotal = mealTotal[meal] +
                       remainingMealMaximum(nextStudent, meal);

    // Stop if the remaining students cannot meet the meal limits.
    if (smallestTotal > mealMaximum[meal] ||
        smallestTotal > stock[meal] + reserve[meal] ||
        largestTotal < mealMinimum[meal]) {
        return false;
    }
    return checkMeals(meal + 1, nextStudent);
}

int remainingTokens(int index, int priority, bool takeMaximum) {
    if (index == studentCount) return 0;
    int amount = 0;
    if (students[index].priority == priority) {
        amount = takeMaximum ? students[index].maximum : students[index].minimum;
    }
    return amount + remainingTokens(index + 1, priority, takeMaximum);
}

int emergencyUsed(int meal) {
    if (meal == 3) return 0;
    int extra = mealTotal[meal] - stock[meal];
    if (extra < 0) extra = 0;
    return extra + emergencyUsed(meal + 1);
}

void saveAllocation(int index) {
    if (index == studentCount) return;
    bestAllocation[index] = allocation[index];
    saveAllocation(index + 1);
}

void saveMealTotals(int meal) {
    if (meal == 3) return;
    bestMealTotal[meal] = mealTotal[meal];
    saveMealTotals(meal + 1);
}

void findAllocation(int index, int highTotal, int lowTotal,
                       bool hasMinimum, bool hasMaximum);

void tryAmount(int index, int amount, int highTotal, int lowTotal,
               bool hasMinimum, bool hasMaximum) {
    if (amount > students[index].maximum || bestEmergency == 0) return;

    int meal = students[index].meal;
    int newTotal = mealTotal[meal] + amount;
    if (newTotal <= mealMaximum[meal] &&
        newTotal <= stock[meal] + reserve[meal] &&
        checkStudents(index, 0, amount)) {
        allocation[index] = amount;
        mealTotal[meal] += amount;

        if (checkRules(0, index + 1)) {
            int newHigh = highTotal;
            int newLow = lowTotal;
            if (students[index].priority == 1) newHigh += amount;
            else newLow += amount;

            findAllocation(index + 1, newHigh, newLow,
                              hasMinimum || amount == students[index].minimum,
                              hasMaximum || amount == students[index].maximum);
        }

        // Undo the choice before trying the next amount.
        mealTotal[meal] -= amount;
    }
    tryAmount(index, amount + 1, highTotal, lowTotal, hasMinimum, hasMaximum);
}

void findAllocation(int index, int highTotal, int lowTotal,
                       bool hasMinimum, bool hasMaximum) {
    // More students cannot reduce the reserve tokens already needed.
    if (bestEmergency == 0 || emergencyUsed(0) >= bestEmergency ||
        !checkMeals(0, index)) {
        return;
    }

    int highestHigh = highTotal + remainingTokens(index, 1, true);
    int lowestLow = lowTotal + remainingTokens(index, 0, false);
    if (highestHigh - lowestLow < requiredGap) return;

    if (index == studentCount) {
        if (!hasMinimum || !hasMaximum || highTotal - lowTotal < requiredGap) {
            return;
        }
        found = true;
        bestEmergency = emergencyUsed(0);
        bestHighTotal = highTotal;
        bestLowTotal = lowTotal;
        saveAllocation(0);
        saveMealTotals(0);
        return;
    }

    tryAmount(index, students[index].minimum, highTotal, lowTotal,
              hasMinimum, hasMaximum);
}

void printAllocation(int index) {
    if (index == studentCount) return;
    cout << "Student " << index + 1 << " ("
         << mealName(students[index].meal) << ", "
         << (students[index].priority == 1 ? "high" : "low")
         << " priority): " << bestAllocation[index] << " tokens\n";
    printAllocation(index + 1);
}

void printMealTotals(int meal, int unusedRegular, int unusedReserve) {
    if (meal == 3) {
        cout << "Total unused regular tokens: " << unusedRegular << '\n';
        cout << "Total unused emergency tokens: " << unusedReserve << '\n';
        cout << "Total unused tokens including reserves: "
             << unusedRegular + unusedReserve << '\n';
        return;
    }

    int emergency = bestMealTotal[meal] - stock[meal];
    if (emergency < 0) emergency = 0;
    int regularUsed = bestMealTotal[meal] - emergency;
    int regularLeft = stock[meal] - regularUsed;
    int reserveLeft = reserve[meal] - emergency;

    cout << mealName(meal) << ": allocated " << bestMealTotal[meal]
         << ", regular used " << regularUsed << ", emergency used " << emergency
         << ", unused regular " << regularLeft
         << ", unused reserve " << reserveLeft << '\n';
    printMealTotals(meal + 1, unusedRegular + regularLeft,
                        unusedReserve + reserveLeft);
}

int main() {
    cout << "Mess Token Management System\n"
         << "Supported limits: 1..20 students, 0..100 tokens per student,\n"
         << "0..2000 tokens per category field, and 0..50 conditional rules.\n"
         << "Number of students: ";
    if (!(cin >> studentCount) || studentCount < 1 || studentCount > MAX_STUDENTS) {
        cout << "Invalid number of students.\n";
        return 1;
    }

    cout << "Meal categories: 1 Breakfast, 2 Lunch, 3 Dinner.\n"
         << "Use category total range 0..2000 when no extra total limit applies.\n";
    if (!readMeals(0) || !readStudents(0)) {
        cout << "Invalid category or student input.\n";
        return 1;
    }

    cout << "Required high-priority total advantage (0..2000; 0 still requires > 0): ";
    if (!(cin >> requiredGap) || requiredGap < 0 || requiredGap > MAX_TOTAL) {
        cout << "Invalid threshold.\n";
        return 1;
    }
    if (requiredGap == 0) requiredGap = 1;

    cout << "Number of conditional rules (0 means none): ";
    if (!(cin >> ruleCount) || ruleCount < 0 || ruleCount > MAX_RULES ||
        !readRules(0)) {
        cout << "Invalid conditional rule input.\n";
        return 1;
    }

    findAllocation(0, 0, 0, false, false);
    if (!found) {
        cout << "No valid allocation exists, even with the emergency reserves.\n";
        return 0;
    }

    cout << "\nValid allocation using the minimum emergency reserve:\n";
    printAllocation(0);
    cout << "High-priority total: " << bestHighTotal
         << "\nLow-priority total: " << bestLowTotal
         << "\nMinimum emergency tokens used: " << bestEmergency << '\n';
    printMealTotals(0, 0, 0);
    return 0;
}
