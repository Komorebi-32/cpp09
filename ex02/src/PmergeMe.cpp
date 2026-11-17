/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 15:59:43 by komorebi          #+#    #+#             */
/*   Updated: 2026/11/17 14:46:20 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <algorithm>

/**
 * @brief Throws if the same value appears more than once.
 * Uniqueness is a hard precondition of the sort: find_pair_winner_pos()
 * locates a winner in the main chain by its value.
 */
void reject_duplicates(const std::vector<unsigned int> &values)
{
	std::vector<unsigned int> copy(values);

	std::sort(copy.begin(), copy.end());
	if (std::adjacent_find(copy.begin(), copy.end()) != copy.end())
		throw std::invalid_argument("duplicate values are not accepted");
}

void fill_input(std::vector<unsigned int> &input, char **av, int ac)
{
	fill_input_impl(input, av, ac);
}

void fill_input(std::deque<unsigned int> &input, char **av, int ac)
{
	fill_input_impl(input, av, ac);
}

// The public functions take plain values: they tag each value with 0 (the top
// level has no caller to read the tag), sort, and strip the tags again.
std::vector<unsigned int> merge_insert_sort(
	std::vector<unsigned int> &values, size_t &comparison_count)
{
	comparison_count = 0;
	std::vector<tagged_value> tagged_values;
	for (size_t i = 0; i < values.size(); i++)
		tagged_values.push_back(tagged_value(values[i], 0));
	tagged_values = merge_insert_sort_impl(tagged_values, comparison_count);
	std::vector<unsigned int> sorted;
	for (size_t i = 0; i < tagged_values.size(); i++)
		sorted.push_back(tagged_values[i].first);
	return sorted;
}

std::deque<unsigned int> merge_insert_sort(
	std::deque<unsigned int> &values, size_t &comparison_count)
{
	comparison_count = 0;
	std::deque<tagged_value> tagged_values;
	for (size_t i = 0; i < values.size(); i++)
		tagged_values.push_back(tagged_value(values[i], 0));
	tagged_values = merge_insert_sort_impl(tagged_values, comparison_count);
	std::deque<unsigned int> sorted;
	for (size_t i = 0; i < tagged_values.size(); i++)
		sorted.push_back(tagged_values[i].first);
	return sorted;
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