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

/**
 * @brief Loads the exchange-rate database when an object is created.
 * @throws std::runtime_error If the database cannot be opened, read, or
 *         contains invalid data.
 */
BitcoinExchange::BitcoinExchange()
{
	loadDatabase();
}

/**
 * @brief Returns a copy of text with leading and trailing whitespace removed.
 * @param text The string to trim.
 * @return The trimmed string.
 */
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

/**
 * @brief Checks whether date is a real calendar date in YYYY-MM-DD format.
 * @param date The date string to validate.
 * @return true if date is valid; otherwise, false.
 */
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

/**
 * @brief Parses a finite double from text.
 * @param text The string to parse.
 * @param number [out] Receives the parsed value when parsing succeeds.
 * @return true if the entire string is a finite, representable double;
 *         otherwise, false.
 */
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

/**
 * @brief Reads data.csv, validates its header and rows, and stores rates by
 *        date.
 * @return Nothing.
 * @throws std::runtime_error If the file cannot be opened or read, its header
 *         or a row is invalid, a date is duplicated, or no rates are present.
 */
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

        //insert date & rate. std::map::insert() returns a pair, first being an
        //iterator to the entry in the map, second being a bool indicating
        //whether insertion happened.
		if (!_rates.insert(std::make_pair(date, rate)).second)
			throw std::runtime_error("duplicate date in data.csv");
	}

    //bad() checks the stream’s badbit status. It returns true if a serious
    //I/O error occurred while reading the file—for example, a device or
    //underlying read failure.
	if (database.bad())
		throw std::runtime_error("error reading data.csv");
	if (_rates.empty())
		throw std::runtime_error("data.csv contains no exchange rates");
}

/**
 * @brief Converts each valid dated value in the input file using the
 *        corresponding exchange rate and prints the result.
 * @param filename Path to the input file.
 * @return Nothing.
 * @throws std::runtime_error If the file cannot be opened, its header is
 *         invalid, or an I/O error occurs while reading it.
 * @note Invalid data rows and dates earlier than the first available rate are
 *       reported to standard error and skipped.
 */
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
