/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/16 16:09:03 by bkaras-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <algorithm>

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

template <typename Container>
static std::vector<std::pair<unsigned int, unsigned int> > make_pairs_impl(
	const Container &input)
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

std::vector<std::pair<unsigned int, unsigned int> > make_pairs(
	const std::vector<unsigned int> &input)
{
	return make_pairs_impl(input);
}

template <typename Container>
static void binary_insert(Container &chain,
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

template <typename Container>
static Container merge_insert_sort_impl(Container &values)
{
    if (values.size() <= 1)
        return values;
    
    //pair adjacent values as (winner a_i, loser b_i)
    std::vector<std::pair<unsigned int, unsigned int> > pairs; // (12, 3)  (10, 2)  (8, 1)
    pairs = make_pairs_impl(values);

    //keep winners
    Container winners;
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
    
    winners = merge_insert_sort_impl(winners);

    //keep track of the losers in the same order as their sorted winners
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
    if (!losers.empty())
        winners.insert(winners.begin(), losers[0]);

    //insert losers following the Jacobsthal suite order
    size_t endpoint = 3;
    size_t previous_endpoint = 1;
    while (previous_endpoint < pairs.size())
    {
        size_t batch_end = std::min(endpoint, pairs.size());
        for (size_t idx = batch_end; idx > previous_endpoint; )
        {
            --idx; // decrementing to switch from Ford-Johnson pairs that are 1-based
            // to 0-based C++ style
             
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
        size_t next_endpoint = endpoint + 2 * previous_endpoint;
        previous_endpoint = endpoint;
        endpoint = next_endpoint;
    }

    //insert the unpaired value if it exists (straggler)
    if (has_unpaired_value)
        binary_insert(winners, unpaired_value, winners.size());

    return winners;
}

std::vector<unsigned int> merge_insert_sort(std::vector<unsigned int> &values)
{
    return merge_insert_sort_impl(values);
}

std::deque<unsigned int> merge_insert_sort(std::deque<unsigned int> &values)
{
    return merge_insert_sort_impl(values);
}

template <typename Container>
static void print_sequence(const Container &values)
{
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (i != 0)
            std::cout << ' ';
        std::cout << values[i];
    }
}

void print_results(const std::vector<unsigned int> &input,
    const std::vector<unsigned int> &vector_sorted, double vector_time_us,
    double deque_time_us)
{
    std::cout << "Before: ";
    print_sequence(input);
    std::cout << '\n' << "After: ";
    print_sequence(vector_sorted);
    std::cout << '\n'
        << "Time to process a range of " << input.size()
        << " elements with std::vector: " << std::fixed
        << std::setprecision(3) << vector_time_us << " us\n";
    std::cout << "Time to process a range of " << input.size()
        << " elements with std::deque: " << deque_time_us << " us\n";
}