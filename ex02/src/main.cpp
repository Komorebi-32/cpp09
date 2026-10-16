/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:31 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/16 15:22:40 by bkaras-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <cctype>
#include <ctime>
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
        if (av[i][0] == '\0')
        {
            std::cout << "Provide only positive integers" << std::endl;
            return (1);
        }
        for (int j = 0; av[i][j]; j++)
        {
            if (!std::isdigit(static_cast<unsigned char>(av[i][j])) || av[i][0] == '0')
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

    std::clock_t start = std::clock();
    std::vector<unsigned int> sorted_vector = merge_insert_sort(input);
    double vector_time_us = static_cast<double>(std::clock() - start)
        * 1000000.0 / CLOCKS_PER_SEC;

    std::deque<unsigned int> deque_input(input.begin(), input.end());
    start = std::clock();
    merge_insert_sort(deque_input);
    double deque_time_us = static_cast<double>(std::clock() - start)
        * 1000000.0 / CLOCKS_PER_SEC;

    print_results(input, sorted_vector, vector_time_us, deque_time_us);
    return (0);
}