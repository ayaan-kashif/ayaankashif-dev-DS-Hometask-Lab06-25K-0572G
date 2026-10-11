#include <iostream>
#include <string>

using namespace std;

int n, targetSum;
string target, code;
bool used[10] = {false};
bool found = false;
long long totalCodes = 0;

void generate(int pos, int sum);

void tryDigit(int pos, int sum, int digit)
{
    if (digit == 10)
    {
        return;
    }

    if (!used[digit] && (pos != 0 || digit != 0) &&
        sum + digit <= targetSum)
    {
        used[digit] = true;
        code += char('0' + digit);

        generate(pos + 1, sum + digit);

        code.erase(code.size() - 1);
        used[digit] = false;
    }

    tryDigit(pos, sum, digit + 1);
}

void generate(int pos, int sum)
{
    if (pos == n)
    {
        if (sum == targetSum)
        {
            cout << code << '\n';
            ++totalCodes;
            if (code == target)
            {
                found = true;
            }
        }
        return;
    }

    if (sum > targetSum ||
        sum + 9 * (n - pos) < targetSum)
    {
        return;
    }

    tryDigit(pos, sum, 0);
}

int main()
{
    cout << "Enter number of positions (1-10): ";
    if (!(cin >> n) || n < 1 || n > 10)
    {
        cout << "Invalid number of positions.\n";
        return 0;
    }

    cout << "Enter required digit sum (0-45): ";
    if (!(cin >> targetSum) || targetSum < 0 || targetSum > 45)
    {
        cout << "Invalid target sum.\n";
        return 0;
    }

    cout << "Enter target code: ";
    if (!(cin >> target))
    {
        cout << "Invalid input.\n";
        return 0;
    }

    cout << "Valid lock codes:\n";
    generate(0, 0);

    if (totalCodes == 0)
    {
        cout << "No valid codes exist.\n";
    }
    cout << "Total valid codes: " << totalCodes << '\n';
    if (found)
    {
        cout << "Target code can be formed.\n";
    }
    else
    {
        cout << "Target code cannot be formed.\n";
    }

    return 0;
}
