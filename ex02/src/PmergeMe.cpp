/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/09 15:31:35 by bkaras-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <stdexcept>
#include <utility>

void fill_input(std::vector<unsigned int> &input, char **av, int ac)
{
	for (int i = 1; i < ac; i++)
	{
		char *end = NULL;
		errno = 0;
		unsigned long value = std::strtoul(av[i], &end, 10);

		if (errno == ERANGE || value > UINT_MAX)
			throw std::overflow_error("input number exceeds unsigned int max");
		input.push_back(static_cast<unsigned int>(value));
	}
}

std::vector<std::pair<unsigned int, unsigned int> > make_pairs(
	const std::vector<unsigned int> &input)
{
	std::vector<std::pair<unsigned int, unsigned int> > main_chain;

	for (size_t i = 0; i + 1 < input.size(); i += 2)
    {
		if (input[i] >= input[i + 1])
			main_chain.push_back(std::make_pair(input[i], input[i + 1]));
		else
			main_chain.push_back(std::make_pair(input[i + 1], input[i]));
    }
    return (main_chain);
}