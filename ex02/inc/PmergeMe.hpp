/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bkaras-g <bkaras-g@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 14:51:38 by bkaras-g          #+#    #+#             */
/*   Updated: 2026/10/09 15:03:37 by bkaras-g         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>

/**
 * @brief Parses command-line values and appends them to input.
 * @param input Vector that receives the parsed values.
 * @param av Command-line argument array.
 * @param ac Number of command-line arguments.
 * @return Nothing.
 * @throws std::overflow_error if a value exceeds unsigned int's range.
 */
void fill_input(std::vector<unsigned int> &input, char **av, int ac);

