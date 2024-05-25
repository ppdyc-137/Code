/* 根据国际象棋的规则，
 * 皇后可以攻击与之处在同一行或同一列或同一斜线上的棋子。
 * 给定个皇后和一个大小的棋盘，寻找使得所有皇后之间无法相互攻击的摆放方案。
 */

#include <iostream>
#include <string>
#include <vector>

using namespace std;

void backtrack(vector<vector<string>>& state, int row, int n,
               vector<bool>& cols, vector<bool>& diags1, vector<bool>& diags2,
               vector<vector<vector<string>>>& res)
{
    if (row == n) {
        res.push_back(state);
        return;
    }

    for (int col = 0; col < n; col++) {
        int diag1 = row - col + n - 1;
        int diag2 = row + col;
        if (cols[col] || diags1[diag1] || diags2[diag2])
            continue;

        cols[col] = diags1[diag1] = diags2[diag2] = true;
        state[row][col] = "Q";
        backtrack(state, row + 1, n, cols, diags1, diags2, res);
        cols[col] = diags1[diag1] = diags2[diag2] = false;
        state[row][col] = "#";
    }
}

vector<vector<vector<string>>> nQueens(int n)
{
    vector<vector<string>> state(n, vector<string>(n, "#"));
    vector<vector<vector<string>>> res;
    vector<bool> cols(n, false);
    vector<bool> diags1(2 * n - 1, false);
    vector<bool> diags2(2 * n - 1, false);
    backtrack(state, 0, n, cols, diags1, diags2, res);
    return res;
}

int main()
{
    int n = 5;
    auto res = nQueens(n);
    for (auto r : res) {
        cout << "---------\n";
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << r[i][j];
            }
            cout << "\n";
        }
    }
}
