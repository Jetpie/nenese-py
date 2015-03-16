// Optimized c++ general construction and searching functions for
// KD-Tree.
//
// @author: Bingqing Qu
//
// Copyright (C) 2014-2015  Bingqing Qu <sylar.qu@gmail.com>
//
// @license: See LICENSE at root directory
#ifndef SIREEN_NEAREST_NEIGHBOUR_H_
#define SIREEN_NEAREST_NEIGHBOUR_H_

#include <vector>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <assert.h>
#include <iomanip>
#include <math.h>
#include <stack>
#include <queue>
#include <limits>
#include "sireen/metrics.hpp"
//#define NDEBUG
using namespace std;

// nnse is short for "nearest neighbour search"
namespace nnse
{
    /// key value pair. Not use the std::pair because there is default
    /// "<" operator overloading for it and it is too heavy for operation.
    struct KeyValue
    {
        size_t key;
        float value;
        KeyValue(const size_t k, const float v) : key(k), value(v){}
    };
    // Operator overloading
    inline bool operator<(const KeyValue& lhs, const KeyValue& rhs)
    {return lhs.value < rhs.value; }

    // compute median position of a group of index
    inline size_t get_median_index(const size_t x)
    {return ( (x - 1) / 2); }

    // epic safe absolute operation on all types
    template<class T>
    inline T abs(const T x){return (x < 0 ? -x : x);}

    /// Define Feature structure
    struct Feature
    {
        /** feature data */
        float* data;
        /** feature dimension */
        size_t dimension;
        Feature(float* data, const size_t dim) : data(data) , dimension(dim) {}
        Feature() : data(),dimension(){}
        Feature& operator=(Feature other)
        {
            std::swap(data,other.data);
            std::swap(dimension,other.dimension);
            return *this;
        }
    };
    /// Node definitions for kd-tree
    struct KDTreeNode
    {
        /** feature dimension for partition */
        int pivot_dim;
        /** key value for partition */
        float pivot_val;
        /** leaf flag */
        bool leaf;
        /** all features at current node */
        Feature* features;
        /** number of features */
        size_t n;
        /** left child */
        KDTreeNode* left;
        /** right child */
        KDTreeNode* right;
        // DEBUG
        void print()
        {
            cout << "**********" << endl;
            cout << "pivot_dim:" << pivot_dim << endl;
            cout << "pivot_val:" << pivot_val << endl;
            cout << "leaf:" << leaf << endl;
            cout << "n:" << n << endl;
            cout << "feature:" <<endl;
            for(size_t i = 0;i<n;++i)
            {

            cout <<"(" <<features[i].data[0] <<","
                 << features[i].data[1] << ")"<<endl;
            }
        };
    };

    ///
    /// Todo: add descriptions for KDTree
    ///
    ///
    ///
    class KDTree
    {
    private:
        /** kd-tree root node */
        KDTreeNode* root_;
        /** kd-tree feature dimension */
        size_t dimension_;

        /**
         * initialization of a subtree
         *
         * @param features an array of features
         * @param n        number of features
         */
        KDTreeNode* init_node(Feature *, const size_t);
        /**
         * expand kd-tree after root node is initialized
         *
         * @param node current kd-tree node
         */
        void expand_subtree(KDTreeNode*);
        /**
         * Partition features on the current node. Two parts:
         *
         * 1.Determine pivot feature to split to patition the currrent
         * node's features. First, find the dimension with grestest
         * variance. Second, find the feature with the median of value on
         * that dimension.
         *
         * 2.Partition the features by the pivot. This is done by firstly,
         * get the order vector and secondly, re-order the features by that
         * order vector.
         *
         * The result of the partition is the Feature array is re-ordered
         * and a new node contains left child of  features[0:k] and the
         * right child contains the right child of features[k+1:k+1+n],
         * where n is the length of right child. The current root node is
         * features[k]
         *
         * @param node the current node
         *
         */
        void partition(KDTreeNode*);

    public:
        /** Constructor */
        KDTree(const size_t);
        /** Destructor */
        ~KDTree();
        /**
         * Release all the allocated memories
         *
         * @param a tree node
         */
        void release(KDTreeNode* );
        /**
         * build the kd-tree structure from input features
         *
         * @param features an array of features
         * @param n        number of features
         *
         */
        void build(Feature*, const size_t);
        /**
         *
         *
         */
        void knn_search_bbf(float* );
        void knn_search_brute_force(float*);

        /**
         * Basic k-nearest-neighbour search method use for kd-tree.
         * First, traverse from root node to a leaf node and. Second,
         * backtrack to search for better node
         *
         * @param feature query Feature
         *
         * @return
         */
        Feature knn_search_basic(Feature feature);

        /**
         * Traverse a kd-tree to a leaf node. Path decision are made
         * by comparision of values between the input feature and node
         * on the node's partition dimension.
         *
         * @param feature a input feature
         * @param node a start node
         *
         * @return a leaf node with node.leaf=true
         */
        KDTreeNode* traverse_to_leaf(Feature ,KDTreeNode*, stack<KDTreeNode*>&);



        // DEBUG
        void print_tree()
        {

            this->print_node(this->root_);

        };
        void print_node(KDTreeNode* node,int indent=0)
        {
            if(node != NULL)
            {
                if(node->left)
                    print_node(node->left,indent+8);

                if(indent)
                {
                    cout << setw(indent) << ' ';
                }
                // size_t k = get_median_index(node->n);
                // cout <<"(" << node->features[k].data[0] << ","
                //      << node->features[k].data[1] << ")\n";
                cout <<"(" << node->pivot_dim << ","
                     << node->pivot_val <<","
                     << node->n << ")\n";

                if(node->right)
                    print_node(node->right,indent+8);


            }
        }

    };

}
#endif //SIREEN_NEAREST_NEIGHBOUR_H_
