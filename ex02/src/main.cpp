/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:31 by komorebi          #+#    #+#             */
/*   Updated: 2026/11/10 12:22:17 by komorebi         ###   ########.fr       */
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
        std::cerr << "Error" << std::endl;
        return (1);
    }
    for (int i = 1; i < ac; i++)
    {
        if (av[i][0] == '\0' || av[i][0] == '0')
        {
            std::cerr << "Error" << std::endl;
            return (1);
        }
        for (int j = 0; av[i][j]; j++)
        {
            if (!std::isdigit(static_cast<unsigned char>(av[i][j])))
            {
                std::cerr << "Error" << std::endl;
                return (1);
            }
        }
    }

    std::vector<unsigned int> input;
    try
    {
        fill_input(input, av, ac);
    }
    catch (const std::exception &)
    {
        std::cerr << "Error" << std::endl;
        return (1);
    }

    // Each timer covers data management (parsing av into the container)
    // and the sort. 'input' above was parsed and validated outside the timers.
    std::clock_t start = std::clock();
    std::vector<unsigned int> vector_input;
    fill_input(vector_input, av, ac);
    size_t vector_comparison_count = 0;
    std::vector<unsigned int> sorted_vector = merge_insert_sort(
        vector_input, vector_comparison_count);
    double vector_time_us = static_cast<double>(std::clock() - start)
        * 1000000.0 / CLOCKS_PER_SEC;

    start = std::clock();
    std::deque<unsigned int> deque_input;
    fill_input(deque_input, av, ac);
    size_t deque_comparison_count = 0;
    merge_insert_sort(deque_input, deque_comparison_count);
    double deque_time_us = static_cast<double>(std::clock() - start)
        * 1000000.0 / CLOCKS_PER_SEC;

    print_results(input, sorted_vector, vector_time_us, deque_time_us);
    return (0);
}