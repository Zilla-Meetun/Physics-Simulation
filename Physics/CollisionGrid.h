#pragma once
#include "Stdafx.h"
#include "Collider.h"
#include <vector>
#include <algorithm>

template <typename T> class GridCell
{
	std::vector<T> colliders;
	int age = -1;
public:
	GridCell() {}
	void Clear() { colliders.clear(); }
	void AddCollider(T c, int gridAge)
	{
		if (age != gridAge)
		{
			age = gridAge;
			colliders.clear();
		}
		colliders.push_back(c);
	}
	const std::vector<T>& GetColliders() const { return colliders; }
};

template <typename T> class CollisionGrid
{
	int nx; // number of cells in x direction
	int nz; // number of cells in z direction
	int age = 0;
	float delta; // size of a cell in x,z directions
	GridCell<T>** grid;
	float x0, z0;
public:
	CollisionGrid(float xmin, float xmax, float zmin, float zmax, int nx, int nz) :
		nx(nx), nz(nz)
	{
		x0 = xmin;
		z0 = zmin;
		delta = std::max((zmax - zmin) / nz, (xmax - xmin) / nx);
		grid = new GridCell<T> * [nx];
		for (int i = 0; i < nx; i++)
			grid[i] = new GridCell<T>[nz];
	}
	void AddCollider(T c, float xmin, float xmax, float zmin, float zmax)
	{
		
		int i0 = (int)((xmin - x0) / delta);
		int i1 = (int)((xmax - x0) / delta);
		int j0 = (int)((zmin - z0) / delta);
		int j1 = (int)((zmax - z0) / delta);
		i0 = std::min(std::max(i0, 0), nx-1);
		j0 = std::min(std::max(j0, 0), nz-1);
		i1 = std::min(std::max(i1, 0), nx-1);
		j1 = std::min(std::max(j1, 0), nz-1);
		for (int i = i0; i <= i1; i++)
		{
			for (int j = j0; j <= j1; j++)
			{
				grid[i][j].AddCollider(c, age);
			}
		}
	}
	void GetColliderList(float xmin, float xmax, float zmin, float zmax, std::vector<T>& colliders)
	{
		int i0 = (int)((xmin - x0) / delta);
		int i1 = (int)((xmax - x0) / delta);
		int j0 = (int)((zmin - z0) / delta);
		int j1 = (int)((zmax - z0) / delta);
		i0 = std::min(std::max(i0, 0), nx - 1);
		j0 = std::min(std::max(j0, 0), nz - 1);
		i1 = std::min(std::max(i1, 0), nx - 1);
		j1 = std::min(std::max(j1, 0), nz - 1);
		colliders.clear();
		for (int i = i0; i <= i1; i++)
		{
			for (int j = j0; j <= j1; j++)
			{
				std::vector<T> cellColliders = grid[i][j].GetColliders();
				for (T c : cellColliders) 
				{
					if (std::find(colliders.begin(), colliders.end(), c) == colliders.end())
					{
						colliders.push_back(c);
					}
				}
			}
		}
	}
	void Clear()
	{
		++age;
	}

};