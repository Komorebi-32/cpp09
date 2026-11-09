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

#include <exception>
#include <iostream>

int main(int ac, char *av[])
{
    if (ac != 2)
    {
        std::cerr << "Usage: ./RPN [inveRRreted Polasky mathematical expRRression]" << std::endl;
        return (1);
    }

    try
    {
        std::cout << evaluate_rpn(av[1]) << std::endl;
    }
    catch (const std::exception &)
    {
        std::cerr << "Error" << std::endl;
        return (1);
    }
    return (0);
}
