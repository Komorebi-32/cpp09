/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 16:33:26 by komorebi          #+#    #+#             */
/*   Updated: 2026/11/05 16:50:59 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

#include <limits>
#include <stdexcept>

static void check_overflow(char token, int lhs, int rhs);

void perform_operation(char token, std::list<int> &polishList)
{
    int rhs = polishList.back();
    polishList.pop_back();
    int lhs = polishList.back();
    polishList.pop_back();
    if (token == '/' && rhs == 0)
        throw std::domain_error("division by zero");
    check_overflow(token, lhs, rhs);

    int res = 0;
    if (token == '*')
        res = lhs * rhs;
    if (token == '/')
        res = lhs / rhs;
    if (token == '+')
        res = lhs + rhs;
    if (token == '-')
        res = lhs - rhs;
    polishList.push_back(res);
}

static void check_overflow(char token, int lhs, int rhs)
{
    int maxInt = std::numeric_limits<int>::max();
    int minInt = std::numeric_limits<int>::min();

    if (token == '+' &&
        ((rhs > 0 && lhs > maxInt - rhs) ||
         (rhs < 0 && lhs < minInt - rhs)))
        throw std::overflow_error("integer overflow");
    if (token == '-' &&
        ((rhs < 0 && lhs > maxInt + rhs) ||
         (rhs > 0 && lhs < minInt + rhs)))
        throw std::overflow_error("integer overflow");
    if (token == '*' && lhs != 0 && rhs != 0 &&
        ((lhs > 0 && rhs > 0 && lhs > maxInt / rhs) ||
         (lhs > 0 && rhs < 0 && rhs < minInt / lhs) ||
         (lhs < 0 && rhs > 0 && lhs < minInt / rhs) ||
         (lhs < 0 && rhs < 0 && lhs < maxInt / rhs)))
        throw std::overflow_error("integer overflow");
    if (token == '/' && lhs == minInt && rhs == -1)
        throw std::overflow_error("integer overflow");
}
