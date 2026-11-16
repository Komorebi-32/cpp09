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

template <typename Container>
struct matching_containers;

template <>
struct matching_containers<std::vector<unsigned int> >
{
	typedef std::vector<unsigned int> values_type;
	typedef std::vector<std::pair<unsigned int, unsigned int> > pairs_type;
};

template <>
struct matching_containers<std::deque<unsigned int> >
{
	typedef std::deque<unsigned int> values_type;
	typedef std::deque<std::pair<unsigned int, unsigned int> > pairs_type;
};

// Element sorted by the recursion: (value, tag). The tag is the index of the
// pair the element belongs to in the caller's level, so a level can find a
// sorted winner's pair in O(1) instead of searching by value.
// std::pair compares value first, so ordering is the same as on plain values.
typedef std::pair<unsigned int, size_t> element;

template <>
struct matching_containers<std::vector<element> >
{
	typedef std::vector<element> values_type;
	typedef std::vector<std::pair<element, element> > pairs_type;
};

template <>
struct matching_containers<std::deque<element> >
{
	typedef std::deque<element> values_type;
	typedef std::deque<std::pair<element, element> > pairs_type;
};

/**
 * @brief Throws if the same value appears more than once.
 * Uniqueness is a hard precondition of the sort: find_pair_winner_pos()
 * locates a winner in the main chain by its value.
 */
static void reject_duplicates(const std::vector<unsigned int> &values)
{
	std::vector<unsigned int> copy(values);

	std::sort(copy.begin(), copy.end());
	if (std::adjacent_find(copy.begin(), copy.end()) != copy.end())
		throw std::invalid_argument("duplicate values are not accepted");
}

template <typename Container>
static void fill_input_impl(Container &input, char **av, int ac)
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

void fill_input(std::vector<unsigned int> &input, char **av, int ac)
{
	fill_input_impl(input, av, ac);
	reject_duplicates(input);
}

void fill_input(std::deque<unsigned int> &input, char **av, int ac)
{
	fill_input_impl(input, av, ac);
}

template <typename Container>
static typename matching_containers<Container>::pairs_type make_pairs_impl(
	const Container &input, size_t &comparison_count)
{
	typename matching_containers<Container>::pairs_type main_chain;

	for (size_t i = 0; i + 1 < input.size(); i += 2)
    {
		++comparison_count;
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
	size_t comparison_count = 0;
	return make_pairs_impl(input, comparison_count);
}

template <typename Container>
static void binary_insert(Container &chain,
	const typename Container::value_type &value, size_t search_end,
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
 * @brief Keeps the pair winners and any unpaired input value.
 * @param values Input values being sorted.
 * @param pairs Winner-first pairs from the input values.
 * @param winners Container that receives the pair winners, each tagged with
 *        the index of its pair.
 * @param has_unpaired_value Set to true when the input has an unpaired value.
 * @param unpaired_value Receives the unpaired input value when present.
 * @return Nothing.
 */
template <typename Container>
static void keep_winners_and_straggler(
	const Container &values,
	const typename matching_containers<Container>::pairs_type &pairs,
	Container &winners, bool &has_unpaired_value,
	typename Container::value_type &unpaired_value)
{
	for (size_t i = 0; i < pairs.size(); i++)
		winners.push_back(element(pairs[i].first.first, i));

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
 * The recursion returns the winners tagged with their pair index, so each
 * pair is found in O(1). The winners are then replaced by the original pair
 * elements, which carry the caller's tags again.
 * @param main_chain Sorted pair winners, tagged with their pair index.
 * @param pairs Winner-first pairs from the input values.
 * @return Losers ordered to match the sorted winners.
 */
template <typename Container>
static typename matching_containers<Container>::values_type keep_losers(
	Container &main_chain,
	const typename matching_containers<Container>::pairs_type &pairs)
{
	typename matching_containers<Container>::values_type losers;

	for (size_t j = 0; j < main_chain.size(); j++)
	{
		size_t pair_index = main_chain[j].second;
		losers.push_back(pairs[pair_index].second);
		main_chain[j] = pairs[pair_index].first;
	}
	return losers;
}

/**
 * @brief Finds the current position of a winner in the main chain.
 * The main chain is always sorted and its values are unique, so a binary
 * search finds the winner in O(log n). These lookups are bookkeeping, not
 * Ford-Johnson comparisons, so they are not counted.
 * @param winner Winner whose position is needed.
 * @param main_chain Sorted main chain.
 * @return Position of the winner in the main chain.
 */
template <typename Container>
static size_t find_pair_winner_pos(
	const typename Container::value_type &winner, const Container &main_chain)
{
	size_t left = 0;
	size_t right = main_chain.size();

	while (left < right)
	{
		size_t mid = left + (right - left) / 2;
		if (main_chain[mid] < winner)
			left = mid + 1;
		else
			right = mid;
	}
	return left;
}

/**
 * @brief Inserts losers into the main chain in Jacobsthal sequence order.
 * @param main_chain Main chain receiving the losers.
 * @param losers Losers ordered to match the sorted winners.
 * @param winners Sorted winners; winners[i] is the partner of losers[i].
 * @return Nothing.
 */
template <typename Container>
static void insert_losers(
	Container &main_chain,
	const typename matching_containers<Container>::values_type &losers,
	const Container &winners,
	size_t &comparison_count)
{
	size_t endpoint = 3;
	size_t previous_endpoint = 1;
	while (previous_endpoint < winners.size())
	{
		size_t batch_end = std::min(endpoint, winners.size());
		for (size_t idx = batch_end; idx > previous_endpoint; )
		{
			--idx; // decrementing to switch from Ford-Johnson pairs that are 1-based
			// to 0-based C++ style

			// determine the position of the winner paired to the loser we want to insert
			// so that we insert the loser with binary search until the paired winner's index
			size_t pairWinnerPos = find_pair_winner_pos(
				winners[idx], main_chain);
			binary_insert(
				main_chain, losers[idx], pairWinnerPos, comparison_count);
		}
		//update boundaries for the next batch
		size_t next_endpoint = endpoint + 2 * previous_endpoint;
		previous_endpoint = endpoint;
		endpoint = next_endpoint;
	}
}

template <typename Container>
static Container merge_insert_sort_impl(
	Container &values, size_t &comparison_count)
{
    //stop condition : values is already sorted if size <= 1
    if (values.size() <= 1)
        return values;
    
    //pair adjacent values as (winner a_i, loser b_i)
    typename matching_containers<Container>::pairs_type pairs;
    pairs = make_pairs_impl(values, comparison_count);

    //keep winners and unpaired value
    Container main_chain;
    bool has_unpaired_value = false;
    typename Container::value_type unpaired_value;
    keep_winners_and_straggler(
        values, pairs, main_chain, has_unpaired_value, unpaired_value);
    
    main_chain = merge_insert_sort_impl(main_chain, comparison_count);

    //keep track of the losers in the same order as their sorted winners
    typename matching_containers<Container>::values_type losers =
        keep_losers(main_chain, pairs);
    //sorted winners, before any loser is inserted: winners[i] pairs with losers[i]
    const Container winners(main_chain);

    //include b1 at the beginning of the chain, since it's the loser of a1 
    //that is at the beginning of the chain
    if (!losers.empty())
        main_chain.insert(main_chain.begin(), losers[0]);

    //insert losers following the Jacobsthal suite order
    insert_losers(main_chain, losers, winners, comparison_count);

    //insert the unpaired value if it exists (straggler)
    if (has_unpaired_value)
        binary_insert(
            main_chain, unpaired_value, main_chain.size(), comparison_count);

    return main_chain;
}

// The public functions take plain values: they tag each value with 0 (the top
// level has no caller to read the tag), sort, and strip the tags again.
std::vector<unsigned int> merge_insert_sort(
	std::vector<unsigned int> &values, size_t &comparison_count)
{
	comparison_count = 0;
	std::vector<element> tagged;
	for (size_t i = 0; i < values.size(); i++)
		tagged.push_back(element(values[i], 0));
	tagged = merge_insert_sort_impl(tagged, comparison_count);
	std::vector<unsigned int> sorted;
	for (size_t i = 0; i < tagged.size(); i++)
		sorted.push_back(tagged[i].first);
	return sorted;
}

std::deque<unsigned int> merge_insert_sort(
	std::deque<unsigned int> &values, size_t &comparison_count)
{
	comparison_count = 0;
	std::deque<element> tagged;
	for (size_t i = 0; i < values.size(); i++)
		tagged.push_back(element(values[i], 0));
	tagged = merge_insert_sort_impl(tagged, comparison_count);
	std::deque<unsigned int> sorted;
	for (size_t i = 0; i < tagged.size(); i++)
		sorted.push_back(tagged[i].first);
	return sorted;
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