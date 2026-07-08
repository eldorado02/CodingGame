/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafaram <arafaram@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/08 15:21:23 by arafaram          #+#    #+#             */
/*   Updated: 2026/07/08 15:21:32 by arafaram         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * Takuzu Solver (Easy mode)
 * No row or column may contain a sequence of three or more repeating digits
 * e.g. 1 1 0 is valid but 1 1 1 is invalid
 **/

pair<int, int> pop(const vector<string> & strs)
{
    for (int j = 0; j < strs.size(); j++)
    {
        for (int i = 0; i < strs[j].size(); i++)
        {
            if (strs[j][i] == '.')
                return make_pair(i, j);
        }
    }
    return make_pair(-1, -1);
}

bool    isFullRow(const string & str)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '.')
            return (false);
    }
    return (true);
}

bool    isFullCol(const vector<string> & strs, int i)
{
    for (size_t j = 0; j < strs.size(); j++)
    {
        if (strs[j][i] == '.')
            return (false);
    }
    return (true);
}

bool    duplicateRow(const string & str, const vector<string> & strs, int j)
{
    for (size_t i = 0; i < strs.size(); i++)
    {
        if (i == j)
            continue ;
        if (str == strs[i])
            return (true); // misy duplicate
    }
    return (false);
}

bool    compareCol(int i, int j, const vector<string> & strs)
{
    int n;

    n = 0;
    for (size_t y = 0; y < strs.size(); y++)
    {
        if (strs[y][i] == strs[y][j])
            n += 1;
    }
    if (n == strs.size())
        return (true);
    return (false);
}

bool    duplicateCol(int j, const vector<string> & strs)
{
    for (size_t i = 0; i < strs[0].size(); i++)
    {
        if (i == j)
            continue ;
        if (compareCol(i, j, strs) == true)
            return (true); // misy duplicate
    }
    return (false);
}

bool    isFinished(vector<string> & strs)
{
    for (size_t j = 0; j < strs.size(); j++)
    {
        if (isFullRow(strs[j]) == false)
            return (false);
    }
    return (true);
}

bool    isTriplet(const string & str)
{
    if (str.size() < 3)
        return (false);
    for (size_t i = 0; i < str.size() - 2; i++)
    {
        if ((str[i] == str[i + 1] && str[i] == str[i + 2]) && (str[i] != '.'))
            return (true);
    }
    return (false);
}

bool    is_equal_number(const string & str)
{
    int n_0;
    int n_1;

    n_0 = 0;
    n_1 = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '0')
            n_0 += 1;
        if (str[i] == '1')
            n_1 += 1;
        
    }
    if (n_0 == n_1)
        return (true);
    return (false);
}

bool    dont_exceed_n_to_have(const string & str, int j, char c)
{
    int n_tohave;
    int n;

    n_tohave = str.size() / 2;
    n = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == c)
            n += 1; 
    }
    if (n >= n_tohave)
        return (true);
    return (false);
}

bool    is_valid(char c, vector<string> & strs, pair<int, int> i_j)
{
    if (dont_exceed_n_to_have(strs[i_j.second], i_j.second, c) == false)
        return (false);
    strs[i_j.second][i_j.first] = c;
    if (isTriplet(strs[i_j.second]) == true)
        return (false);
    return (true);
}

bool    backtracking(vector<string> & strs)
{
    pair<int, int>  i_j;
    const   string  s = "01";

    i_j = pop(strs);
    if (i_j.first == -1 && i_j.second == -1)
        return (true);
    for (auto c : s)
    {
        if (is_valid(c, strs, i_j))
        {
            strs[i_j.second][i_j.first] = c;
            if (backtracking(strs) == true)
                return (true);
            strs[i_j.second][i_j.first] = '.';
        }
    }
}

int main()
{
    int n;
    vector<string>  strs;
    cin >> n; cin.ignore();
    for (int i = 0; i < n; i++) {
        string row;
        getline(cin, row);
        strs.push_back(row);
    }

    cout << "Completed board" << endl;
}