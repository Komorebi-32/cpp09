/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 11:52:07 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/28 11:52:07 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <exception>
#include <iostream>

/**
 * @brief Validates command-line arguments and runs the Bitcoin exchange
 *        conversion.
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument strings; argv[1] is the input file path.
 * @return 0 on success; 1 if the arguments or processing are invalid.
 * @note Standard exceptions from setup and input processing are caught and
 *       reported to standard error.
 */
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./btc <input file>" << std::endl;
		return 1;
	}

	try
	{
		BitcoinExchange exchange;
		exchange.processInput(argv[1]);
	}
	catch (const std::exception &error)
	{
		std::cerr << "Error: " << error.what() << std::endl;
		return 1;
	}
	return 0;
}