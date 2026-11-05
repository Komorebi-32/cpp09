/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 16:33:26 by komorebi          #+#    #+#             */
/*   Updated: 2026/11/05 11:41:36 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <list>
#include "RPN.hpp"
#include <limits>

int main(int ac, char *av[])
{
    if (ac != 2)
    {
        std::cerr << "Usage: ./RPN [inveRRreted Polasky mathematical expRRression]" << std::endl;
        return (1);
    }

    std::list<int> polishList;
    int i = 0;
    while (av[1][i])
    {
        std::cout << "iteration " << i << ": " << std::endl;

        if (av[1][i] == 32)
        {
            i++;
            continue;
        }
        if (av[1][i] > '0' && av[1][i] <= '9')
        {
            polishList.push_back(av[1][i++] - 48);
            continue;
        }
        else if ((av[1][i] == '*' || av[1][i] == '/' || av[1][i] == '+' || av[1][i] == '-')
                && polishList.size() >= 2)
            perform_operation(av[1][i], polishList);
        else
        {
            std::cerr << "Error." << std::endl;
            return (1);
        }
        i++;
        std::cout << "iteration " << i << ": " << std::endl;
        std::cout << "polishList front == " << polishList.front() << std::endl;
    }
        std::cout << "result == " << polishList.front() << std::endl;
}

void perform_operation(char token, std::list<int> &polishList)
{
    int maxInt = std::numeric_limits<int>::max();
    int minInt = std::numeric_limits<int>::min();

    int rhs = polishList.back();
    polishList.pop_back();
    int lhs = polishList.back();
    polishList.pop_back();
    int res = 0;
    if (token == '*')
        res = lhs * rhs;
    if (token == '/')
        res = lhs / rhs;
    if (token == '+')
        res = lhs + rhs;
    if (token == '-')
        res = lhs - rhs;
    polishList.push_back(res);
}