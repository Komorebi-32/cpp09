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