/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: komorebi <komorebi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/28 18:43:03 by komorebi          #+#    #+#             */
/*   Updated: 2026/10/28 18:43:30 by komorebi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <list>
#pragma once

void perform_operation(char token, std::list<int> &polishList);