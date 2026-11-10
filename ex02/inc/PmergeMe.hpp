/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 14:51:38 by bkaras-g          #+#    #+#             */
/*   Updated: 2026/10/16 15:21:56 by bkaras-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PMERGEME_HPP
# define PMERGEME_HPP

#include <cstddef>
#include <deque>
#include <utility>
#include <vector>

/**
 * @brief Parses command-line values and appends them to input.
 * @param input Vector that receives the parsed values.
 * @param av Command-line argument array.
 * @param ac Number of command-line arguments.
 * @return Nothing.
 * @throws std::overflow_error if a value exceeds unsigned int's range.
 * @throws std::invalid_argument if a value appears more than once.
 */
void fill_input(std::vector<unsigned int> &input, char **av, int ac);

/**
 * @brief Parses command-line values and appends them to input.
 * Does not check for duplicates: call the vector overload first.
 * @param input Deque that receives the parsed values.
 * @param av Command-line argument array.
 * @param ac Number of command-line arguments.
 * @return Nothing.
 * @throws std::overflow_error if a value exceeds unsigned int's range.
 */
void fill_input(std::deque<unsigned int> &input, char **av, int ac);

/**
 * @brief Groups adjacent input values into pairs with the larger value first.
 * @param input Values to group into pairs.
 * @return Vector of winner-first pairs.
 */
std::vector<std::pair<unsigned int, unsigned int> > make_pairs(
	const std::vector<unsigned int> &input);

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

#endif
