//-----GPL----------------------------------------------------------------------
//
// This file is part of AdVersion
// Copyright (C) 2015  Zach Cobell
//
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------
//
//  File: AdVersion_comparison.cpp
//
//------------------------------------------------------------------------------
#include "AdVersion.h"


//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering node hashes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering node hashes
 *
 * @param[in] node1  first node for comparison
 * @param[in] node2  second node for comparison
 *
 * Comparison operator for ordering node hashes. Return is if node1 < node2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::nodeHashLessThan(const adcirc_node *node1, const adcirc_node *node2)
{
    if(node1->positionHash<node2->positionHash)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering element hashes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering element hashes
 *
 * @param[in] element1  first element for comparison
 * @param[in] element2  second element for comparison
 *
 * Comparison operator for ordering node hashes. Return is if element1 < element2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::elementHashLessThan(const adcirc_element *element1, const adcirc_element *element2)
{
    if(element1->hash<element2->hash)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering boundary hashes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering boundary hashes
 *
 * @param[in] boundary1  first boundary for comparison
 * @param[in] boundary2  second boundary for comparison
 *
 * Comparison operator for ordering boundary hashes. Return is if boundary1 < boundary2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::boundaryHashLessThan(const adcirc_boundary *boundary1, const adcirc_boundary *boundary2)
{
    if(boundary1->hash<boundary2->hash)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering rectangle areas
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering rectangle areas
 *
 * @param[in] rectangle1  first rectangle for comparison
 * @param[in] rectangle2  second rectangle for comparison
 *
 * Comparison operator for ordering node hashes. Return is if rectangle1 < rectangle2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::rectangeleAreaLessThan(const Rectangle rectangle1, const Rectangle rectangle2)
{
    if(rectangle1.area<rectangle2.area)
        return true;
    else
        return false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering node IDs
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering node IDs
 *
 * @param[in] node1  first node for comparison
 * @param[in] node2  second node for comparison
 *
 * Comparison operator for ordering node IDs. Return is if node1 < node2
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::nodeNumberLessThan(const adcirc_node *node1, const adcirc_node *node2)
{
    return node1->id<node2->id;
}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering element by the sum of their nodes
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering elements by the sum of their node IDs
 *
 * @param[in] node1  first node for comparison
 * @param[in] node2  second node for comparison
 *
 * Comparison operator for ordering elements by the sum of their node IDs. This does a decent
 * job of ordering the element IDs without doing something more complex
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::elementSumLessThan(const adcirc_element *element1, const adcirc_element *element2)
{
    int s1 = element1->connections[0]->id + element1->connections[1]->id + element1->connections[2]->id;
    int s2 = element2->connections[0]->id + element2->connections[1]->id + element1->connections[2]->id;
    return s1<s2;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Comparison operator for ordering boundary conditions
//-----------------------------------------------------------------------------------------//
/**
 * \brief Comparison operator for ordering boundaries by position geographicly
 *
 * @param[in] boundary1  first boundary for comparison
 * @param[in] boundary2  second boundary for comparison
 *
 * Comparison operator for ordering boundary conditions. Return is if boundary1 > boundary2
 * so that boundaries are positioned east-->west
 *
 **/
//-----------------------------------------------------------------------------------------//
bool AdVersion::boundaryPositionLessThan(const adcirc_boundary *boundary1,const adcirc_boundary *boundary2)
{
    return boundary1->averageLongitude > boundary2->averageLongitude;
}
//-----------------------------------------------------------------------------------------//
