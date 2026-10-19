/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/19 12:14:59 by komorebi         ###   ########.fr       */
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

/**
 * @brief Keeps the pair winners and any unpaired input value.
 * @param values Input values being sorted.
 * @param pairs Winner-first pairs from the input values.
 * @param winners Container that receives the pair winners.
 * @param has_unpaired_value Set to true when the input has an unpaired value.
 * @param unpaired_value Receives the unpaired input value when present.
 * @return Nothing.
 */
template <typename Container>
static void keep_winners_and_straggler(
	const Container &values,
	const std::vector<std::pair<unsigned int, unsigned int> > &pairs,
	Container &winners, bool &has_unpaired_value,
	unsigned int &unpaired_value)
{
	for (size_t i = 0; i < pairs.size(); i++)
		winners.push_back(pairs[i].first);

	if (values.size() % 2 == 1)
	{
		has_unpaired_value = true;
		unpaired_value = values[values.size() - 1];
	}
	else
		has_unpaired_value = false;
}

/**
 * @brief Keeps losers in the order of their sorted winners.
 * @param winners Sorted pair winners.
 * @param pairs Winner-first pairs from the input values.
 * @return Losers ordered to match the sorted winners.
 */
template <typename Container>
static std::vector<unsigned int> keep_losers(
	const Container &winners,
	const std::vector<std::pair<unsigned int, unsigned int> > &pairs)
{
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
	return losers;
}

/**
 * @brief Finds the position of the winner paired with a loser.
 * @param loser Loser whose paired winner position is needed.
 * @param winners Sorted main chain.
 * @param pairs Winner-first pairs from the input values.
 * @return Position of the paired winner in the main chain.
 */
template <typename Container>
static size_t find_pair_winner_pos(
	unsigned int loser, const Container &winners,
	const std::vector<std::pair<unsigned int, unsigned int> > &pairs)
{
	unsigned int pairWinner = 0;
	for (size_t pairIndex = 0; pairIndex < pairs.size(); pairIndex++)
	{
		if (pairs[pairIndex].second == loser)
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
	return pairWinnerPos;
}

/**
 * @brief Inserts losers into the main chain in Jacobsthal sequence order.
 * @param winners Main chain receiving the losers.
 * @param losers Losers ordered to match the sorted winners.
 * @param pairs Winner-first pairs from the input values.
 * @return Nothing.
 */
template <typename Container>
static void insert_losers(
	Container &winners, const std::vector<unsigned int> &losers,
	const std::vector<std::pair<unsigned int, unsigned int> > &pairs)
{
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
			size_t pairWinnerPos = find_pair_winner_pos(
				losers[idx], winners, pairs);
			binary_insert(winners, losers[idx], pairWinnerPos);
		}
		//update boundaries for the next batch
		size_t next_endpoint = endpoint + 2 * previous_endpoint;
		previous_endpoint = endpoint;
		endpoint = next_endpoint;
	}
}

template <typename Container>
static Container merge_insert_sort_impl(Container &values)
{
    //stop condition : values is already sorted if size <= 1
    if (values.size() <= 1)
        return values;
    
    //pair adjacent values as (winner a_i, loser b_i)
    std::vector<std::pair<unsigned int, unsigned int> > pairs; // (12, 3)  (10, 2)  (8, 1)
    pairs = make_pairs_impl(values);

    //keep winners and unpaired value
    Container winners;
    bool has_unpaired_value = false;
    unsigned int unpaired_value = 0;
    keep_winners_and_straggler(
        values, pairs, winners, has_unpaired_value, unpaired_value);
    
    winners = merge_insert_sort_impl(winners);

    //keep track of the losers in the same order as their sorted winners
    std::vector<unsigned int> losers = keep_losers(winners, pairs);
    
    //include b1 at the beginning of the chain, since it's the loser of a1 
    //that is at the beginning of the chain
    if (!losers.empty())
        winners.insert(winners.begin(), losers[0]);

    //insert losers following the Jacobsthal suite order
    insert_losers(winners, losers, pairs);

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