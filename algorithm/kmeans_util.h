#include <vector>

# ifndef KMEANS_H
# define KMEANS_H

double distance(std::vector<double> point_a, std::vector<double> point_b);

std::vector<int> closest_cluster(std::vector<std::vector<double>> vec, std::vector<std::vector<double>> center);

std::vector<std::vector<double>> new_center_vectors(std::vector<std::vector<double>> vec,
                                                    std::vector<std::vector<double>> old_center, std::vector<int> labels);

std::vector<int> kmeans(std::vector<std::vector<double>> vec, int k, int max_iter);

# endif // KMEANS_H