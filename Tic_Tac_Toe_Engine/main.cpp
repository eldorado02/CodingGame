/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arafaram <arafaram@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 18:40:03 by arafaram          #+#    #+#             */
/*   Updated: 2026/07/05 19:29:48 by arafaram         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <bits/stdc++.h>

using namespace std;

// X : +1
// O : -1
// "Tie" : 0 ' '

class GameEngine
{
	private:
		vector<string> 	_board;
		string			_role;

		int		score(char c);
		char	verifLine(void);
		char	verifColumn(void);
		char	verifDiagonal(void);
		bool	isGameEnded(void);
		pair<bool, int>	isEnding(void);

	public:
	
		GameEngine(void) {};
		GameEngine(const string & role, const vector<string> & board);
		GameEngine(const GameEngine & src);
		GameEngine & operator=(const GameEngine & rhs);
		~GameEngine(void) {};
};

GameEngine::GameEngine(const string & role, const vector<string> & board) :
	_role(role), _board(board)
{
}

GameEngine::GameEngine(const GameEngine & src)
{
	*this = src;
}

GameEngine & GameEngine::operator=(const GameEngine & rhs)
{
	if (this != &rhs)
	{
		this->_role = rhs._role;
		this->_board = rhs._board;
	}
	return (*this);
}

char GameEngine::verifLine(void)
{
	int	n;
	char prev;

	n = 0;
	for (size_t j = 0; j < this->_board.size(); j++)
	{
		prev = this->_board[j][0];
		n = 0;
		for (size_t i = 0; i < this->_board[j].size(); i++)
		{
			if (this->_board[j][i] == prev)
				n += 1;
		}
		if (n == 3 && prev != '.')
			return (prev);
	}
	return (' ');
}

char GameEngine::verifColumn(void)
{
	int	n;
	char prev;

	n = 0;
	for (size_t i = 0; i < this->_board[0].size(); i++)
	{
		prev = this->_board[i][0];
		n = 0;
		for (size_t j = 0; j < this->_board.size(); j++)
		{
			if (this->_board[j][i] == prev)
				n += 1;
		}
		if (n == 3 && prev != '.')
			return (prev);
	}
	return (' ');
}

char GameEngine::verifDiagonal(void)
{
	int	n;
	char	prev;

	n = 0;
	prev = this->_board[0][0];
	for (size_t i = 0; i < this->_board.size(); i++)
	{
		if (this->_board[i][i] == prev)
			n += 1;
	}
	if (n == 3 && prev != '.')
		return (prev);
	prev = this->_board[0][2];
	for (size_t i = 0; i < this->_board.size(); i++)
	{
		if (this->_board[i][2 - i] == prev)
			n += 1;
	}
	if (n == 3 && prev != '.')
		return (prev);
	return (' ');
}

bool	GameEngine::isGameEnded(void)
{
	for (size_t j = 0; j < this->_board.size(); j++)
	{
		for (size_t i = 0; i < this->_board[j].size(); i++)
		{
			if (this->_board[j][i] == '.')
				return (false);
		}
	}
	return (true);
}

int		GameEngine::score(char c)
{
	if (c == 'X')
		return (1);
	else if (c == 'O')
		return (-1);
	return (0);
}


pair<bool, int> GameEngine::isEnding()
{
	char	c;

	c = verifLine();
	if (c != ' ')
		return (make_pair(true, this->score(c)));
	c = verifColumn();
	if (c != ' ')
		return (make_pair(true, this->score(c)));
	c = verifDiagonal();
	if (c != ' ')
		return (make_pair(true, this->score(c)));
	if (isGameEnded() == true)
		return (make_pair(true, 0));
	return (make_pair(false, 42));
}



int main()
{
    string engine;
	vector<string>	strs;
    string row;

	getline(cin, engine);
    for (int i = 0; i < 3; i++) {
        getline(cin, row);
		strs.push_back(row);
    }

    cout << "XOXO" << endl;
}