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
#include <list>
#include <stdexcept>

static void check_overflow(char token, int lhs, int rhs);

/**
 * @brief Pops the two top operands, applies token and pushes the result.
 * @throws std::domain_error On division by zero.
 * @throws std::overflow_error If the result does not fit in an int.
 */
static void perform_operation(char token, std::list<int> &polishList)
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

/**
 * @brief Evaluates a reverse Polish notation expression.
 * @param expression Single-digit numbers and the operators + - * / separated
 *        by single spaces.
 * @return The result of the expression.
 * @throws std::exception If the expression is malformed, divides by zero or
 *         overflows an int.
 */
int evaluate_rpn(const std::string &expression)
{
    std::list<int> polishList;

    for (std::string::size_type i = 0; i < expression.size(); ++i)
    {
        char c = expression[i];
        bool lastChar = (i + 1 == expression.size());

        if (c == ' ')
            continue;
        if (!lastChar && expression[i + 1] != ' ')
            throw std::invalid_argument("tokens must be separated by spaces");
        if (c >= '0' && c <= '9')
            polishList.push_back(c - '0');
        else if ((c == '*' || c == '/' || c == '+' || c == '-')
                && polishList.size() >= 2)
            perform_operation(c, polishList);
        else
            throw std::invalid_argument("invalid token or missing operand");
    }
    if (polishList.size() != 1)
        throw std::invalid_argument("invalid expression");
    return polishList.front();
}
