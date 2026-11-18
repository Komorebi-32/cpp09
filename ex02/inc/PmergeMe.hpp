/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 14:51:38 by bkaras-g          #+#    #+#             */
/*   Updated: 2026/11/17 14:49:14 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <algorithm>
#include <cerrno>
#include <climits>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

// Value sorted by the recursion, with its tag: (value, tag). The tag is the
// index of the pair the value belongs to in the caller's level, so a level can
// find a sorted winner's pair in O(1) instead of searching by value.
// std::pair compares value first, so ordering is the same as on plain values.
typedef std::pair<unsigned int, size_t> tagged_value;

/**
 * @brief Throws if the same value appears more than once.
 * @param values Parsed input values.
 * @return Nothing.
 * @throws std::invalid_argument if a value appears more than once.
 */
void reject_duplicates(const std::vector<unsigned int> &values);

/**
 * @brief Appends command-line values to input, without any check.
 * Only call it on arguments already accepted by parse_input().
 * @param input Vector that receives the values.
 * @param av Command-line argument array.
 * @param ac Number of command-line arguments.
 * @return Nothing.
 */
void fill_input(std::vector<unsigned int> &input, char **av, int ac);

/**
 * @brief Appends command-line values to input, without any check.
 * Only call it on arguments already accepted by parse_input().
 * @param input Deque that receives the values.
 * @param av Command-line argument array.
 * @param ac Number of command-line arguments.
 * @return Nothing.
 */
void fill_input(std::deque<unsigned int> &input, char **av, int ac);

/**
 * @brief Sorts values using the Ford-Johnson merge-insertion algorithm.
 * @param values Values to sort.
 * @param comparison_count Receives the number of element-ordering comparisons.
 * @return A sorted copy of the values.
 */
std::vector<unsigned int> merge_insert_sort(
	std::vector<unsigned int> &values, size_t &comparison_count);

/**
 * @brief Sorts values using the Ford-Johnson merge-insertion algorithm.
 * @param values Values to sort.
 * @param comparison_count Receives the number of element-ordering comparisons.
 * @return A sorted copy of the values.
 */
std::deque<unsigned int> merge_insert_sort(
	std::deque<unsigned int> &values, size_t &comparison_count);

/**
 * @brief Prints the input, sorted results, and processing times for both containers.
 * @param input Unsorted values.
 * @param vector_sorted Sorted values from the vector implementation.
 * @param vector_time_us Vector processing time in microseconds.
 * @param deque_time_us Deque processing time in microseconds.
 * @return Nothing.
 */
void print_results(const std::vector<unsigned int> &input,
	const std::vector<unsigned int> &vector_sorted, double vector_time_us,
	double deque_time_us);

/* ---- template definitions ---- */

/**
 * @brief Parses command-line values, checks their range, and appends them to
 *        input. The characters themselves are checked in main().
 * @param input Container that receives the parsed values.
 * @param av Command-line argument array.
 * @param ac Number of command-line arguments.
 * @return Nothing.
 * @throws std::overflow_error if a value exceeds unsigned int's range.
 */
template <typename Container>
void parse_input(Container &input, char **av, int ac)
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

/**
 * @brief Appends command-line values to input without any check.
 * Only call it on arguments already accepted by parse_input().
 */
template <typename Container>
void fill_input_impl(Container &input, char **av, int ac)
{
	for (int i = 1; i < ac; i++)
	{
		unsigned long value = std::strtoul(av[i], NULL, 10);
		input.push_back(static_cast<unsigned int>(value));
	}
}

/**
 * @brief Splits each adjacent pair of values into its winner and its loser.
 * Pair i is stored as winners_by_pair[i] (the larger value) and
 * losers_by_pair[i] (the smaller one).
 * @param input Values to pair; an unpaired last value is ignored here.
 * @param winners_by_pair Receives the larger value of each pair.
 * @param losers_by_pair Receives the smaller value of each pair.
 * @param comparison_count Incremented once per pair.
 * @return Nothing.
 */
template <typename Container>
void split_pairs(const Container &input, Container &winners_by_pair,
	Container &losers_by_pair, size_t &comparison_count)
{
	for (size_t i = 0; i + 1 < input.size(); i += 2)
    {
		++comparison_count;
		if (input[i] >= input[i + 1])
		{
			winners_by_pair.push_back(input[i]);
			losers_by_pair.push_back(input[i + 1]);
		}
		else
		{
			winners_by_pair.push_back(input[i + 1]);
			losers_by_pair.push_back(input[i]);
		}
    }
}

template <typename Container>
void binary_insert(Container &chain,
	const tagged_value &value, size_t search_end,
	size_t &comparison_count)
{
	size_t left = 0;
	size_t right = search_end;

	while (left < right)
	{
		size_t mid = left + (right - left) / 2;
		++comparison_count;
		if (chain[mid] < value)
			left = mid + 1;
		else
			right = mid;
	}
	chain.insert(chain.begin() + left, value);
}

/**
 * @brief Tags each winner with the index of its pair, for the recursion.
 * @param winners_by_pair Larger value of each pair.
 * @param tagged_winners Receives (winner value, pair index) for each pair.
 * @return Nothing.
 */
template <typename Container>
void tag_winners(const Container &winners_by_pair, Container &tagged_winners)
{
	for (size_t i = 0; i < winners_by_pair.size(); i++)
		tagged_winners.push_back(tagged_value(winners_by_pair[i].first, i));
}

/**
 * @brief Builds the pend: the losers in the order of their sorted winners.
 * Each sorted winner is tagged with its pair index, so its loser is found in
 * O(1). Must run before restore_caller_tags(), which overwrites the tags.
 * @param main_chain Sorted winners, tagged with their pair index.
 * @param losers_by_pair Smaller value of each pair.
 * @return The pend: pend[j] is the loser of main_chain[j].
 */
template <typename Container>
Container build_pend(const Container &main_chain,
	const Container &losers_by_pair)
{
	Container pend;

	for (size_t j = 0; j < main_chain.size(); j++)
		pend.push_back(losers_by_pair[main_chain[j].second]);
	return pend;
}

/**
 * @brief Replaces each sorted winner by its entry in winners_by_pair, which
 *        carries the caller's tag instead of this level's pair index.
 * @param main_chain Sorted winners, tagged with their pair index.
 * @param winners_by_pair Larger value of each pair, with the caller's tags.
 * @return Nothing.
 */
template <typename Container>
void restore_caller_tags(Container &main_chain,
	const Container &winners_by_pair)
{
	for (size_t j = 0; j < main_chain.size(); j++)
		main_chain[j] = winners_by_pair[main_chain[j].second];
}

/**
 * @brief Finds the current position of a winner in the main chain.
 * The main chain is always sorted and its values are unique, so a binary
 * search finds the winner in O(log n). These lookups are bookkeeping, not
 * Ford-Johnson comparisons, so they are not counted.
 * @param value Winner whose position is needed.
 * @param chain Sorted main chain.
 * @return Position of the winner in the main chain.
 */
template <typename Container>
size_t find_position(const tagged_value &value, const Container &chain)
{
	size_t left = 0;
	size_t right = chain.size();

	while (left < right)
	{
		size_t mid = left + (right - left) / 2;
		if (chain[mid] < value)
			left = mid + 1;
		else
			right = mid;
	}
	return left;
}

/**
 * @brief Inserts the pend into the main chain in Jacobsthal sequence order.
 * @param main_chain Main chain receiving the pend.
 * @param pend Losers ordered to match the sorted winners.
 * @param sorted_winners Sorted winners; sorted_winners[i] is the partner of
 *        pend[i].
 * @return Nothing.
 */
template <typename Container>
void insert_pend(
	Container &main_chain,
	const Container &pend,
	const Container &sorted_winners,
	size_t &comparison_count)
{
	size_t jacobsthal = 3;
	size_t previous_jacobsthal = 1;
	while (previous_jacobsthal < sorted_winners.size())
	{
		size_t group_end = std::min(jacobsthal, sorted_winners.size());
		for (size_t idx = group_end; idx > previous_jacobsthal; )
		{
			--idx; // decrementing to switch from Ford-Johnson pairs that are 1-based
			// to 0-based C++ style

			// determine the position of the winner paired to the loser we want to insert
			// so that we insert the loser with binary search until the paired winner's index
			size_t winner_pos = find_position(
				sorted_winners[idx], main_chain);
			binary_insert(
				main_chain, pend[idx], winner_pos, comparison_count);
		}
		//update boundaries for the next group
		size_t next_jacobsthal = jacobsthal + 2 * previous_jacobsthal;
		previous_jacobsthal = jacobsthal;
		jacobsthal = next_jacobsthal;
	}
}

template <typename Container>
Container merge_insert_sort_impl(
	Container &values, size_t &comparison_count)
{
    //stop condition : values is already sorted if size <= 1
    if (values.size() <= 1)
        return values;
    
    //pair adjacent values as (winner a_i, loser b_i)
    Container winners_by_pair;
    Container losers_by_pair;
    split_pairs(values, winners_by_pair, losers_by_pair, comparison_count);

    //tag the winners and set the straggler aside
    Container tagged_winners;
    tag_winners(winners_by_pair, tagged_winners);
    bool has_straggler = (values.size() % 2 == 1);
    tagged_value straggler;
    if (has_straggler)
        straggler = values[values.size() - 1];

    Container main_chain =
        merge_insert_sort_impl(tagged_winners, comparison_count);

    //keep track of the losers in the same order as their sorted winners
    Container pend = build_pend(main_chain, losers_by_pair);
    restore_caller_tags(main_chain, winners_by_pair);
    //sorted winners, before any loser is inserted: sorted_winners[i] pairs with pend[i]
    const Container sorted_winners(main_chain);

    //include b1 at the beginning of the chain, since it's the loser of a1 
    //that is at the beginning of the chain
    if (!pend.empty())
        main_chain.insert(main_chain.begin(), pend[0]);

    //insert losers following the Jacobsthal suite order
    insert_pend(main_chain, pend, sorted_winners, comparison_count);

    //insert the straggler if it exists
    if (has_straggler)
        binary_insert(
            main_chain, straggler, main_chain.size(), comparison_count);

    return main_chain;
}

template <typename Container>
void print_sequence(const Container &values)
{
    for (size_t i = 0; i < values.size(); ++i)
    {
        if (i != 0)
            std::cout << ' ';
        std::cout << values[i];
    }
}

#endif
