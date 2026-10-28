/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 11:39:21 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/28 11:40:07 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange
{
public:
	BitcoinExchange();
	void processInput(const std::string &filename) const;

private:
	std::map<std::string, double> _rates;

	void loadDatabase();
	static std::string trim(const std::string &text);
	static bool isValidDate(const std::string &date);
	static bool parseNumber(const std::string &text, double &number);
};

#endif
