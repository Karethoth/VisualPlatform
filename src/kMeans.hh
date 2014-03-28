#pragma once
#ifndef KMEANS_HH
#define KMEANS_HH

#include <vector>


namespace KMeans
{
	struct Element
	{
		int clusterId;

		virtual void  Add( const Element* )      = 0;
		virtual void  Divide( const float )      = 0;
		virtual float Distance( const Element* ) = 0;
	};



	template<typename ElementType>
	struct Cluster
	{
		int id;
		ElementType        centroid;
		std::vector<long>  indices;



		bool operator==( const Cluster &other )
		{
			return (this.id == other.id);
		}


		void UpdateCentroid( const std::vector<ElementType*> &data )
		{
			if( indices.size() <= 0 )
			{
				return;
			}
			ElementType mean;
			for( auto ind  = indices.begin();
			          ind != indices.end();
			          ind++ )
			{
				mean.Add( data[*ind] );
			}
			mean.Divide( indices.size() );
			centroid = mean;
		}
	};



	template<typename ElementType>
	struct KMeans
	{
		std::vector<Cluster<ElementType>*> clusters;
		std::vector<ElementType*> data;


		void Init( const std::vector<ElementType*> &elements, int clusterCount )
		{
			data = elements;
			clusters = std::vector<Cluster<ElementType>*>( clusterCount );

			int clusterId = 1;
			for( auto cluster  = clusters.begin();
			          cluster != clusters.end();
			          cluster++ )
			{
				(*cluster) = new Cluster<ElementType>();
				(*cluster)->id = clusterId++;

				// Not setting the centroid!
			}
		}



		template<typename ElementType>
		Cluster<ElementType>* GetCluster( int id )
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
				auto  best         = clusters.begin();
				float bestDistance = (*best)->centroid.Distance( *it );

				// Loop trough the clusters, we want to find the "closest" one.
				for( auto cluster  = clusters.begin();
						  cluster != clusters.end();
						  cluster++ )
				{
					float currDistance = (*cluster)->centroid.Distance( *it );
					if( currDistance >= bestDistance )
					{
						continue;
					}

					// This is the best cluster so far.
					best         = cluster;
					bestDistance = currDistance;
				}

				// Set the cluster id, for the element.
				// Also add the element id to the cluster.
				long oldClusterId = (*it)->clusterId;
				(*it)->clusterId = (*best)->id;
				(*best)->indices.push_back( it - data.begin() );

				// Check if the best cluster isn't the same as the old one
				if( (*best)->id != oldClusterId )
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

