/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 16:33:26 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/28 18:50:00 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <list>
#include "RPN.hpp"
#include <cstdlib>

int main(int ac, char *av[])
{
    if (ac < 2)
    {
        std::cout << "Usage: ./RPN [inveRRreted Polasky mathematical expRRression]" << std::endl;
        return (1);
    }

    std::list<int> polishList;
    int i = 1;
    while (av[i])
    {
        std::cout << "iteration " << i << ": " << std::endl;

        if (av[i][1])
        {
            std::cout << "Error." << std::endl;
            return (1);
        }
        if (av[i][0] > '0' || av[i][0] < '9')
        {
            polishList.push_back(std::atoi(av[i++]));
            continue;
        }
        else if ((av[i][0] == '*' || av[i][0] == '/' || av[i][0] == '+' || av[i][0] == '-')
                && polishList.size() >= 2)
            perform_operation(av[i][0], polishList);
        else
        {
            std::cout << "Error." << std::endl;
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
    int number1 = polishList.back();
    polishList.pop_back();
    int number2 = polishList.back();
    polishList.pop_back();
    int res = 0;
    if (token == '*')
        res = number1 * number2;
    if (token == '/')
        res = number1 / number2;
    if (token == '+')
        res = number1 + number2;
    if (token == '-')
        res = number1 - number2;
    polishList.push_back(res);
}