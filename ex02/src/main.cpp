/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:31 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/09 15:00:18 by bkaras-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <vector>
#include <cctype>
#include <stdexcept>
#include "PmergeMe.hpp"

int main(int ac, char **av)
{
    if (ac == 1)
    {
        std::cout << "Usage: PmergeMe [list of positive ints separated by spaces]" << std::endl;
        return (1);
    }
    for (int i = 1; i < ac; i++)
    {
        for (int j = 0; av[i][j]; j++)
        {
            if (!std::isdigit(static_cast<unsigned char>(av[i][j])))
            {
                std::cout << "Provide only positive integers" << std::endl;
                return (1);
            }
        }
    }

    std::vector<unsigned int> input;
    try
    {
        fill_input(input, av, ac);
    }
    catch (const std::overflow_error &error)
    {
        std::cerr << error.what() << std::endl;
        return (1);
    }
}