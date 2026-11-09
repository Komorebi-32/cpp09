/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 16:33:26 by komorebi          #+#    #+#             */
/*   Updated: 2026/11/05 16:50:59 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

#include <iostream>
#include <list>

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
        if (av[1][i] != 32 && av[1][i + 1] && av[1][i + 1] != 32)
        {
            std::cerr << "Error" << std::endl;
            return (1);
        }
        if (av[1][i] == 32)
        {
            i++;
            continue;
        }
        if (av[1][i] >= '0' && av[1][i] <= '9')
        {
            polishList.push_back(av[1][i++] - 48);
            continue;
        }
        else if ((av[1][i] == '*' || av[1][i] == '/' || av[1][i] == '+' || av[1][i] == '-')
                && polishList.size() >= 2)
        {
            try
            {
                perform_operation(av[1][i], polishList);
            }
            catch (const std::exception &error)
            {
                std::cerr << "Error" << std::endl;
                return (1);
            }
        }
        else
        {
            std::cerr << "Error" << std::endl;
            return (1);
        }
        i++;
    }
    if (polishList.size() == 1)
        std::cout << polishList.front() << std::endl;
    else
    {
        std::cerr << "Error" << std::endl;
        return (1);
    }
    return (0);        
}
