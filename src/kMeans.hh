#pragma once
#ifndef KMEANS_HH
#define KMEANS_HH

#include <vector>


namespace KMeans
{
	template<typename Position>
	struct Element
	{
		Position position;
		virtual Position Distance( const Element<Position>& ) = 0;
	};



	template<typename Position>
	struct Cluster
	{
		Element<Position> *centroid;
		std::vector<long>  indices;

		void UpdateCentroid( const std::vector<Element<Position>*> &data )
		{
			Position mean;
			for( auto ind  = indices.begin();
			          ind != indices.end();
			          ind++ )
			{
				mean += data[ind];
			}
			mean /= indices.size();
			centroid->position = mean;
		}
	};



	template<typename Position>
	struct KMeans
	{
		std::vector<Cluster<Position>> clusters;
		std::vector<Element<Position>*> data;

		long Assign()
		{
			return 0;
		}


		long Update()
		{
			long changes = Assign();

			for( auto cluster  = clusters.begin();
			          cluster != clusters.end;
			          cluster++ )
			{
				(*cluster)->UpdateCentroid( data );
			}

			return changes;
		}
	};
}

#endif

