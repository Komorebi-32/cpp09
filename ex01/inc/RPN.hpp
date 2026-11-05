/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:43:03 by komorebi          #+#    #+#             */
/*   Updated: 2026/11/05 13:51:05 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <list>
#include <limits>
#include <stdexcept>
#pragma once

void perform_operation(char token, std::list<int> &polishList);
static void check_overflow(char token, int lhs, int rhs);