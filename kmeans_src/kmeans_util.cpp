#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <random>

double distance(std::vector<double> point_a, std::vector<double> point_b)
{
    // ssd is sum of square distances
    double ssd = 0.0;
    for (int i = 0; i < std::min(point_a.size(), point_b.size()); i++)
    {
        double delta = (point_a[i] - point_b[i]);
        ssd += delta * delta;
    }
    return std::sqrt(ssd);
}

std::vector<int> closest_cluster(std::vector<std::vector<double>> vec, std::vector<std::vector<double>> center)
{
    // category to the closest center point
    std::vector<int> labels(vec.size());
    for (int i = 0; i < vec.size(); i++)
    {
        int closest_point = 0;
        double closest_distance = distance(vec[i], center[0]);
        for (int k = 1; k < center.size(); k++)
        {
            double d = distance(vec[i], center[k]);
            if (d < closest_distance)
            {
                closest_point = k;
                closest_distance = d;
            }
        }
        labels[i] = closest_point;
    }
    return labels;
}

std::vector<std::vector<double>> new_center_vectors(std::vector<std::vector<double>> vec,
                                                    std::vector<std::vector<double>> old_center, std::vector<int> labels)
{
    // initial new center point
    int k = old_center.size();
    int column_number = old_center[0].size();
    std::vector<int> count(k, 0);
    std::vector<std::vector<double>> new_center(k, std::vector<double>(column_number, 0.0));
    // calculate each cluster point's summation and then divided to calculate average center
    for (int i = 0; i < vec.size(); i++)
    {
        for (int j = 0; j < column_number; j++)
        {
            new_center[labels[i]][j] += vec[i][j];
        }
        count[labels[i]]++;
    }
    for (int i = 0; i < k; i++)
    {
        if (count[i] == 0)
        {
            for (int j = 0; j < column_number; j++)
                new_center[i][j] = old_center[i][j];
        }
        else
        {
            for (int j = 0; j < column_number; j++)
                new_center[i][j] /= count[i];
        }
    }
    return new_center;
}

std::vector<int> kmeans(std::vector<std::vector<double>> vec, int k, int max_iter)
{
    // find initial center points
    std::vector<std::vector<double>> center(k);
    for (int j = 0; j < vec[0].size(); j++)
    {
        std::vector<double> column;
        for (int i = 0; i < vec.size(); i++)
        {
            column.push_back(vec[i][j]);
        }
        auto max_x = *std::max_element(column.begin(), column.end());
        auto min_x = *std::min_element(column.begin(), column.end());
        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<double> unif(min_x, max_x);
        for (int i = 0; i < k; i++)
        {
            double random_point = unif(eng);
            center[i].push_back(random_point);
        }
    }
    std::vector<int> labels = closest_cluster(vec, center);

    // where iteration is started
    center = new_center_vectors(vec, center, labels);
    for (int iter = 1; iter < max_iter; iter++)
    {
        std::vector<int> new_labels = closest_cluster(vec, center);
        center = new_center_vectors(vec, center, labels);
        if (new_labels == labels)
        {
            std::cout << "Early termination due to convergence after " << iter + 1 << " iteration." << std::endl;
            break;
        }
        labels = new_labels;
    }
    return labels;
}