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
		int clusterId;
		virtual Position Distance( const Element<Position>& ) = 0;
	};



	template<typename Position>
	struct Cluster
	{
		int id;
		Element<Position> *centroid;
		std::vector<long>  indices;



		bool operator==( const Cluster &other )
		{
			return (this.id == other.id);
		}


		void UpdateCentroid( const std::vector<Element<Position>*> &data )
		{
			if( !centroid )
			{
				return;
			}


			Position mean;
			for( auto ind  = indices.begin();
			          ind != indices.end();
			          ind++ )
			{
				mean += (data[*ind])->position;
			}
			mean /= (float)indices.size();
			centroid->position = mean;
		}
	};



	template<typename Position>
	struct KMeans
	{
		std::vector<Cluster<Position>*> clusters;
		std::vector<Element<Position>*> data;


		template<typename Position>
		void Init( std::vector<Element<Position>*> elements, int clusterCount )
		{
			data = elements;
			clusters = std::vector<Cluster<Position>*>( clusterCount );

			int clusterId = 1;
			for( auto cluster  = clusters.begin();
			          cluster != clusters.end();
			          cluster++ )
			{
				(*cluster) = new Cluster<Position>();
				(*cluster)->id = clusterId++;

				// Not setting the centroid!
			}
		}



		template<typename Position>
		Cluster<Position>* GetCluster( int id )
		{
			for( auto cluster  = clusters.begin();
			          cluster != clusters.end();
			          cluster++ )
			{
				if( (*cluster)->id == id )
				{
					return *cluster;
				}
			}
			return nullptr;
		}



		long Assign()
		{
			long changes = 0;

			for( auto cluster  = clusters.begin();
			          cluster != clusters.end();
			          cluster++ )
			{
				(*cluster)->indices.clear();
			}

			// Loop trough every element
			for( auto it  = data.begin();
			          it != data.end();
			          it++ )
			{
				Cluster<Position> *best = (*clusters.begin());
				Position bestDistance   = best->centroid->Distance( **it );

				// Loop trough the clusters, we want to find the "closest" one.
				for( auto cluster  = clusters.begin();
						  cluster != clusters.end();
						  cluster++ )
				{
					Position currDistance = (*cluster)->centroid->Distance( **it );
					if( currDistance >= bestDistance )
					{
						continue;
					}

					// This is the best cluster so far.
					best         = *cluster;
					bestDistance = currDistance;
				}

				// Set the cluster id, for the element.
				// Also add the element id to the cluster.
				(*it)->clusterId = best->id;
				best->indices.push_back( it - data.begin() );

				// Check if the best cluster isn't the same as the old one
				if( best->id != (*it)->clusterId )
				{
					changes++;
				}
			}
			return changes;
		}



		long Update()
		{
			long changes = Assign();

			for( auto cluster  = clusters.begin();
			          cluster != clusters.end();
			          cluster++ )
			{
				(*cluster)->UpdateCentroid( data );
			}

			return changes;
		}
	};
}

#endif

