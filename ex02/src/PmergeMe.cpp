/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/09 17:11:45 by bkaras-g         ###   ########.fr       */
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

void group_winners(std::vector<std::pair<unsigned int, unsigned int> > &pairs)
{
    if (pairs.size() == 0)
        return ;
    std::vector<unsigned int> winners;

    for (size_t i = 0; i < pairs.size(); i++)
        winners.push_back(pairs[i][0]);
    
    std::vector<std::pair<unsigned int, unsigned int> > new_pairs;
    new_pairs = make_pairs(winners);

    bool has_unpaired_value;
    unsigned int unpaired_value;
    if (winners.size() % 2 == 1)
    {
        has_unpaired_value = true;
        unpaired_value = winners[winners.size() - 1];
    }

    group_winners(new_pairs);
    insert_losers(pairs, )
}