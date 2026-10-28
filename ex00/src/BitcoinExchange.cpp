/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 11:40:09 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/28 11:51:20 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

BitcoinExchange::BitcoinExchange()
{
	loadDatabase();
}

std::string BitcoinExchange::trim(const std::string &text)
{
	std::string::size_type first = 0;
	std::string::size_type last = text.size();

	while (first < last && std::isspace(static_cast<unsigned char>(text[first])))
		++first;
	while (last > first
		&& std::isspace(static_cast<unsigned char>(text[last - 1])))
		--last;
	return text.substr(first, last - first);
}

bool BitcoinExchange::isValidDate(const std::string &date)
{
	if (date.size() != 10 || date[4] != '-' || date[7] != '-')
		return false;
	for (std::string::size_type i = 0; i < date.size(); ++i)
	{
		if (i != 4 && i != 7
			&& !std::isdigit(static_cast<unsigned char>(date[i])))
			return false;
	}

	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());
	if (month < 1 || month > 12)
		return false;

	const int daysInMonth[] = {31, 28, 31, 30, 31, 30,
		31, 31, 30, 31, 30, 31};
	int maxDay = daysInMonth[month - 1];
	if (month == 2 && (year % 400 == 0
		|| (year % 4 == 0 && year % 100 != 0)))
		maxDay = 29;
	return day >= 1 && day <= maxDay;
}

bool BitcoinExchange::parseNumber(const std::string &text, double &number)
{
	if (text.empty())
		return false;

	char *end = NULL;
	errno = 0;
	number = std::strtod(text.c_str(), &end);

    //check if "number" is a number
	if (end == text.c_str() || *end != '\0' || errno == ERANGE)
		return false;

    //check if number is respectively NaN, inf or -inf
    //NaN is the unusual floating-point value that is unequal to itself.
	if (number != number || number > std::numeric_limits<double>::max()
		|| number < -std::numeric_limits<double>::max())
		return false;
	return true;
}

void BitcoinExchange::loadDatabase()
{
	std::ifstream database("data.csv");
	if (!database.is_open())
		throw std::runtime_error("could not open data.csv");

	std::string line;
	if (!std::getline(database, line) || trim(line) != "date,exchange_rate")
		throw std::runtime_error("data.csv has a missing or invalid header");

	while (std::getline(database, line))
	{
		std::string::size_type comma = line.find(',');
		if (comma == std::string::npos
			|| line.find(',', comma + 1) != std::string::npos)
			throw std::runtime_error("invalid row in data.csv");

		std::string date = trim(line.substr(0, comma));
		std::string rateText = trim(line.substr(comma + 1));
		double rate = 0.0;
		if (!isValidDate(date) || !parseNumber(rateText, rate) || rate < 0.0)
			throw std::runtime_error("invalid row in data.csv");
		if (!_rates.insert(std::make_pair(date, rate)).second)
			throw std::runtime_error("duplicate date in data.csv");
	}
	if (database.bad())
		throw std::runtime_error("error reading data.csv");
	if (_rates.empty())
		throw std::runtime_error("data.csv contains no exchange rates");
}

void BitcoinExchange::processInput(const std::string &filename) const
{
	std::ifstream input(filename.c_str());
	if (!input.is_open())
		throw std::runtime_error("could not open input file");

	std::string line;
	if (!std::getline(input, line) || trim(line) != "date | value")
		throw std::runtime_error("input file has a missing or invalid header");

	while (std::getline(input, line))
	{
		std::string::size_type separator = line.find('|');
		if (separator == std::string::npos
			|| line.find('|', separator + 1) != std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}

		std::string date = trim(line.substr(0, separator));
		std::string valueText = trim(line.substr(separator + 1));
		double value = 0.0;
		if (!isValidDate(date) || !parseNumber(valueText, value))
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}
		if (value < 0.0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}
		if (value > 1000.0)
		{
			std::cerr << "Error: too large a number." << std::endl;
			continue;
		}

		std::map<std::string, double>::const_iterator rate = _rates.upper_bound(date);
		if (rate == _rates.begin())
		{
			std::cerr << "Error: no exchange rate available for " << date
				<< std::endl;
			continue;
		}
		--rate;
		std::cout << date << " => " << value << " = "
			<< value * rate->second << std::endl;
	}
	if (input.bad())
		throw std::runtime_error("error reading input file");
}
