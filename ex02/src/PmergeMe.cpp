/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/15 18:26:27 by komorebi         ###   ########.fr       */
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

// void group_winners(std::vector<std::pair<unsigned int, unsigned int> > &pairs)
// {
//     if (pairs.size() == 0)
//         return ;
//     std::vector<unsigned int> winners;

//     for (size_t i = 0; i < pairs.size(); i++)
//         winners.push_back(pairs[i][0]);
    
//     std::vector<std::pair<unsigned int, unsigned int> > new_pairs;
//     new_pairs = make_pairs(winners);

//     bool has_unpaired_value;
//     unsigned int unpaired_value;
//     if (winners.size() % 2 == 1)
//     {
//         has_unpaired_value = true;
//         unpaired_value = winners[winners.size() - 1];
//     }

//     group_winners(new_pairs);
//     insert_losers(pairs, winners, unpaired value);
// }

/*
// void insert_losers(std::vector<std::pair<unsigned int, unsigned int> > &pairs, 
//     std::vector<unsigned int> main_chain, unsigned_int unpaired_value)
// {
//  if (winner's size == 2) switch 1st and 2nd
    for (all the pairs in pairs vector)
    {
        find the loser's index to insert from the Jacobsthal suite
        identify the corresponding winner's index in the main_chain
        insert the loser in the main_chain (binary seach until the winner's index)
    }
// }
*/

std::vector<unsigned int> merge_insert_sort(std::vector<unsigned int> &values)
{
    if (values.size() <= 1)
        return values;
    
    //pair adjacent values as (winner a_i, loser b_i)
    std::vector<std::pair<unsigned int, unsigned int> > pairs; // (12, 3)  (10, 2)  (8, 1)
    pairs = make_pairs(values);

    //keep winners
    std::vector<unsigned int> winners;
    for (size_t i = 0; i < pairs.size(); i++)
        winners.push_back(pairs[i].first);
    
    //keep unpaired value
    bool has_unpaired_value;
    unsigned int unpaired_value;
    if (values.size() % 2 == 1)
    {
        has_unpaired_value = true;
        unpaired_value = values[values.size() - 1];
    }
    else
        has_unpaired_value = false;
    
    winners = merge_insert_sort(winners); // [8, 10, 12]

    //keep track of the losers and their association to the winners
    //filling losers vector so that losers[i] is the loser of winners[i]
    std::vector<unsigned int> losers;
    for (size_t j = 0; j < winners.size(); j++)
    {
        for (size_t i = 0; i < pairs.size(); i++)
        {
            if (winners[j] == pairs[i].first)
            {
                losers.push_back(pairs[i].second);
                break;
            }
        }
    }
    
    //include b1 at the beginning of the chain, since it's the loser of a1 
    //that is at the beginning of the chain
    winners.insert(winners.begin(), losers[0]);
}

/*

    for each Jacobsthal batch endpoint t = 3, 5, 11, 21, ...:
        visit pair indices from t down to the previous endpoint + 1:
            skip indices beyond the number of pairs
            binary-insert b_i into the chain, respecting its paired a_i

    if a straggler exists:
        binary-insert it into the chain

    return chain
*/