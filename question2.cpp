#include <iostream>
#include <string>
using namespace std;

const int MAX_RUNNERS = 10;
const long long MAX_VALUE = 1000000000LL;

int n;
long long stamina[MAX_RUNNERS];
long long speed[MAX_RUNNERS];
long long total = 0;
int order[MAX_RUNNERS];
bool used[MAX_RUNNERS] = {};
long long totalOrders = 0;

bool readRunners(int i)
{
    if (i == n)
    {
        return true;
    }

    cout << "Runner " << i + 1 << " stamina and speed: ";
    if (!(cin >> stamina[i] >> speed[i]))
    {
        return false;
    }
    if (stamina[i] < 0 || stamina[i] > MAX_VALUE ||
        speed[i] < 0 || speed[i] > MAX_VALUE)
    {
        cout << "Stamina and speed must be between 0 and " << MAX_VALUE
             << ".\n";
        return false;
    }

    total += stamina[i];
    return readRunners(i + 1);
}

bool canFollow(int current, int next)
{
    return speed[current] - speed[next] >=
           stamina[next] - stamina[current];
}

bool canPlace(int runner, int pos)
{
    if (pos < n - 2)
    {
        return true;
    }

    return stamina[runner] * n >= total;
}

bool canSwap(int pos, int runner)
{
    if (pos == 0)
    {
        return false;
    }

    int prevPos = pos - 1;
    int prev = order[prevPos];

    if (!canFollow(runner, prev))
    {
        return false;
    }
    if (!canPlace(runner, prevPos) ||
        !canPlace(prev, pos))
    {
        return false;
    }
    if (prevPos > 0 &&
        !canFollow(order[prevPos - 1], runner))
    {
        return false;
    }

    return true;
}

void printOrder(int pos)
{
    if (pos == n)
    {
        cout << '\n';
        return;
    }

    if (pos > 0)
    {
        cout << " -> ";
    }
    cout << "R" << order[pos] + 1;
    printOrder(pos + 1);
}

void buildOrder(int pos);

void tryRunner(int pos, int runner)
{
    if (runner == n)
    {
        return;
    }

    if (!used[runner] && canPlace(runner, pos) &&
        (pos == 0 ||
         canFollow(order[pos - 1], runner)) &&
        !canSwap(pos, runner))
    {
        used[runner] = true;
        order[pos] = runner;

        buildOrder(pos + 1);

        used[runner] = false;
    }

    tryRunner(pos, runner + 1);
}

void buildOrder(int pos)
{
    if (pos == n)
    {
        ++totalOrders;
        cout << "Order " << totalOrders << ": ";
        printOrder(0);
        return;
    }

    tryRunner(pos, 0);
}

int main()
{
    cout << "Number of runners (1-" << MAX_RUNNERS << "): ";
    if (!(cin >> n) || n < 1 || n > MAX_RUNNERS)
    {
        cout << "Invalid number of runners.\n";
        return 0;
    }
    if (!readRunners(0))
    {
        cout << "Invalid runner input.\n";
        return 0;
    }

    cout << "\nHandover condition: speed[current] - speed[next] >= "
            "stamina[next] - stamina[current]\n";
    cout << "Average stamina: "
         << static_cast<double>(total) / n << '\n';
    cout << "Valid running orders:\n";
    buildOrder(0);

    if (totalOrders == 0)
    {
        cout << "No valid running order exists.\n";
    }
    else
    {
        cout << "Total valid orders: " << totalOrders << '\n';
    }

    return 0;
}
