/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/16 14:01:54 by bkaras-g         ###   ########.fr       */
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

static void binary_insert(std::vector<unsigned int> &chain,
	unsigned int value, size_t search_end)
{
	size_t left = 0;
	size_t right = search_end;

	while (left < right)
	{
		size_t mid = left + (right - left) / 2;
		if (chain[mid] < value)
			left = mid + 1;
		else
			right = mid;
	}
	chain.insert(chain.begin() + left, value);
}

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

    //insert losers following the Jacobsthal suite order
    int endpoint = 3;
    int previous_endpoint = 1;
    for (int k = 2; previous_endpoint <= pairs.size(); k++) // k-th batch of insertion (see page 7 pdf Merge)
    {
        for (int idx = endpoint; idx > previous_endpoint; idx--)
        {
            // skip the Jacobsthal indices that are beyond the number of pairs
            while (idx > pairs.size())
                idx--;

            // determine the position of the winner paired to the loser we want to insert
            // so that we insert the loser with binary search until the paired winner's index
            unsigned int pairWinner = 0;
            for (size_t pairIndex = 0; pairIndex < pairs.size(); pairIndex++)
            {
                if (pairs[pairIndex].second == losers[idx])
                {
                    pairWinner = pairs[pairIndex].first;
                    break;
                }
            }
            size_t pairWinnerPos = 0;
            for (size_t winnerIndex = 0; winnerIndex < winners.size(); winnerIndex++)
            {
                if (winners[winnerIndex] == pairWinner)
                {
                    pairWinnerPos = winnerIndex;
                    break;
                }
            }
            binary_insert(winners, losers[idx], pairWinnerPos);
        }
        //update boundaries for the next batch
        previous_endpoint = endpoint;
        endpoint = (2^(k + 1) + (-1)^k) / 3; //endpoint == t_k (page 7 pdf Merge)
    }

    //insert the unpaired value if it exists (straggler)
    if (has_unpaired_value)
        binary_insert(winners, unpaired_value, winners.size());

    return winners;
}