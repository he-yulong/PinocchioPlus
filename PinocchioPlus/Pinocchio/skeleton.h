/*  This file is part of the Pinocchio automatic rigging library.
	Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#pragma once
#ifndef SKELETON_H
#define SKELETON_H

#include <map>
#include "graphutils.h"

class Skeleton {
public:
	Skeleton() {}
	
	const PtGraph &fGraph() const { return fGraphV; }
	const std::vector<int> &fPrev() const { return fPrevV; }
	const std::vector<int> &fSym() const { return fSymV; }
	
	const PtGraph &cGraph() const { return cGraphV; }
	const std::vector<int> &cPrev() const { return cPrevV; }
	const std::vector<int> &cSym() const { return cSymV; }
	const std::vector<bool> &cFeet() const { return cFeetV; }
	const std::vector<bool> &cFat() const { return cFatV; }
	
	const std::vector<int> &cfMap() const { return cfMapV; }
	const std::vector<int> &fcMap() const { return fcMapV; }
	const std::vector<double> &fcFraction() const { return fcFractionV; }
	const std::vector<double> &cLength() const { return cLengthV; }

	int getJointForName(const std::string &name) const { if(jointNames.count(name)) return jointNames.find(name)->second; return -1; }
	
	void scale(double factor);
	
protected:
	void initCompressed();
	
	//help for creation
	std::map<std::string, int> jointNames;
	void makeJoint(const std::string &name, const Vector3 &pos, const std::string &previous = std::string());
	void makeSymmetric(const std::string &name1, const std::string &name2);
	void setFoot(const std::string &name);
	void setFat(const std::string &name);
	
private:
	 //full
	PtGraph fGraphV;
	std::vector<int> fPrevV; //previous vertices
	std::vector<int> fSymV; //symmetry
	
	//compressed (no degree 2 vertices)
	PtGraph cGraphV; 
	std::vector<int> cPrevV; //previous vertices
	std::vector<int> cSymV; //symmetry
	std::vector<bool> cFeetV; //whether the vertex should be near the ground
	std::vector<bool> cFatV; //whether the vertex should be in a large region
	
	std::vector<int> cfMapV; //compressed to full map
	std::vector<int> fcMapV; //full to compressed map, -1 when vertex is not in compressed
	std::vector<double> fcFractionV; //maps full vertex number to ratio of its prev edge length to total length of
								//containing edge in the compressed graph
	std::vector<double> cLengthV; //lengths of the compressed bones
};

class HumanSkeleton : public Skeleton
{
public:
	HumanSkeleton();
};

class QuadSkeleton : public Skeleton
{
public:
	QuadSkeleton();
};

class HorseSkeleton : public Skeleton
{
public:
	HorseSkeleton();
};

class CentaurSkeleton : public Skeleton
{
public:
	CentaurSkeleton();
};

class FileSkeleton : public Skeleton
{
public:
	FileSkeleton(const std::string &filename);
}; 

#endif
