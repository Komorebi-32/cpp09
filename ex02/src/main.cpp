/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:31 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/07 16:15:42 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>

int main(int ac, char **av)
{
    if (ac == 1)
    {
        std::cout << "Usage: PmergeMe [list of ints separated by spaces]" << std::endl;
        return (1);
    }
    for (size_t i = 1; i < ac; i++)
    {
        if (!std::isdigit(static_cast<unsigned char>(av[i][0])))
        {
            std::cout << "Provide only integers" << std::endl;
            return (1);
        }
    }
}