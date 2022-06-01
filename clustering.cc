#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <mutex>
#include <new>
#include <regex>
#include <string>
#include <vector>
#include <math.h>
#include <numeric>

#include "mysql.h"  // IWYU pragma: keep
#include "mysql/udf_registration_types.h"

#include "algorithm/fastcluster.h"
#include "algorithm/kmeans_util.h"

using namespace std;

#ifdef _WIN32
/* inet_aton needs winsock library */
#pragma comment(lib, "ws2_32")
#endif

/*
  Not all platforms have gethostbyaddr_r, so we use a global lock here instead.
  Production-quality code should use getaddrinfo where available.
*/
static std::mutex* LOCK_hostname{ nullptr };

#ifndef _WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

/*
  KMEANS: aggregate function
  KMEANS(int k, int max_iter, double features...)
  Input arguments:
   k       = number of observables
   max_iter  = max iteration
*/

struct KMEANS_data
{
    size_t k;
    size_t max_iter;
    std::vector<int> labels;
    std::vector<std::vector<double>> vec;
    char* str_labels = "";
};

extern "C" bool KMEANS_init(UDF_INIT * initid, UDF_ARGS * args, char* message)
{
    KMEANS_data* data = new (std::nothrow) KMEANS_data;
    if (args->arg_count < 3)
    {
        strcpy(message,
            "wrong argument number: KMEANS() requires at least a k, a max_iter_number and a feature argument");
        return true;
    }

    if (args->arg_type[0] != INT_RESULT || args->arg_type[1] != INT_RESULT)
    {
        strcpy(message,
            "wrong argument type: KMEANS() k and max_iter_number arguments require INT");
        return true;
    }

    int k = *((int*)args->args[0]);
    int max_iter = *((int*)args->args[1]);

    if (k < 1)
    {
        strcpy(message,
            "wrong argument number: KMEANS() group number should be larger than 0");
        return true;
    }

    if (max_iter <= 1)
    {
        strcpy(message,
               "wrong argument number: KMEANS() max_iter_number should be larger than 1");
        return true;
    }

    for (unsigned i = 2; i < (args->arg_count); i++)
    {
        if (args->arg_type[i] != INT_RESULT && args->arg_type[i] != REAL_RESULT)
        {
            strcpy(message,
                "wrong argument type: KMEANS() feature arguments require INT or REAL");
            return true;
        }
        args->arg_type[i] = REAL_RESULT;
    }

    if (!data)
    {
        strcpy(message, "Could not allocate memory");
        return true;
    }
    data->k = k;
    data->max_iter = max_iter;
    data->str_labels = (char*)malloc(4096);
    memset(data->str_labels, 0, 4096);
    initid->ptr = static_cast<char*>(static_cast<void*>(data));
    initid->maybe_null = true;
    return false;
}

extern "C" void KMEANS_deinit(UDF_INIT * initid)
{
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    delete data;
}

extern "C" void KMEANS_add(UDF_INIT * initid, UDF_ARGS * args, unsigned char*,
    unsigned char*)
{
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    std::vector<double> row;
    for (unsigned i = 2; i < args->arg_count; i++)
    {
        void* arg = args->args[i];
        double number = *(static_cast<double*>(arg));
        row.push_back(number);
    }
    data->vec.push_back(row);
}

extern "C" void KMEANS_clear(UDF_INIT * initid, unsigned char*,
    unsigned char*)
{
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    data->vec.clear();
    data->labels.clear();
}

extern "C" char* KMEANS(UDF_INIT * initid, UDF_ARGS * args,
    char* result, unsigned long* length,
    char* is_null, char* error)
{
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    if (data->vec.size() == 0)
    {
        *is_null = 1;
        return NULL;
    }
    data->labels = kmeans(data->vec, data->k, data->max_iter);

    std::string str;
    for (int index : data->labels) {
        str += std::to_string(index);
        str += ",";
    }
    *length = str.length() - 1;
    data->str_labels = strdup(str.c_str());

    return data->str_labels;
}

/*
  Hierar_cluster: aggregate function
  Hierar_cluster(int n, string method, double features...)
  Input arguments:
   n       = number of observables
   method  = cluster metric: "single", "complete", "average", "median"
*/

// points distance
double cal_distance(const std::vector<double>& s1, const std::vector<double>& s2) {
    int size = sizeof(s1);
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += pow(s1[i] - s2[i], 2);
    }
    return sqrt(sum);
}

struct Hierar_data {
    std::vector<std::vector<double>> vec;
    char* str_result="";
    size_t cluster_num=0;
    int opt_method = HCLUST_METHOD_SINGLE;
};

extern "C" bool Hierar_cluster_init(UDF_INIT * initid, UDF_ARGS * args, char* message) {
    Hierar_data* data = new (std::nothrow) Hierar_data;
    if (args->arg_count < 3) {
        strcpy(message,
            "wrong argument number: Hierar_cluster() requires at least a group number, a method and a feature argument");
        return true;
    }

    if (args->arg_type[0] != INT_RESULT) {
        strcpy(message,
            "wrong argument type: Hierar_cluster() n requires INT");
        return true;
    }

    string method = args->args[1];
    if (method == "single" || method == "") {
        data->opt_method = HCLUST_METHOD_SINGLE;
    }
    else if (method == "complete") {
        data->opt_method = HCLUST_METHOD_COMPLETE;
    }
    else if (method == "average") {
        data->opt_method = HCLUST_METHOD_AVERAGE;
    }
    else if (method == "median") {
        data->opt_method = HCLUST_METHOD_MEDIAN;
    }
    else {
        strcpy(message,
            "wrong argument type: Hierar_cluster() method argument requires \"single\", \"complete\", \"average\"or \"median\"");
        return true;
    }
    
    for (unsigned i = 2; i < args->arg_count; i++) {
        if (args->arg_type[i] != INT_RESULT && args->arg_type[i] != REAL_RESULT) {
            strcpy(message,
                "wrong argument type: Hierar_cluster() feature arguments require INT or REAL");
            return true;
        }
        args->arg_type[i] = REAL_RESULT;
    }

    if (!data) {
        strcpy(message, "Could not allocate memory");
        return true;
    }

    int n = *((int*)args->args[0]);
    if (n < 1)
    {
        strcpy(message,
            "wrong argument number: Hierar_cluster() group number should be larger than 0");
        return true;
    }

    data->cluster_num = *((int*)args->args[0]);
    data->str_result = (char*)malloc(4096);
    memset(data->str_result, 0, 4096);
    initid->ptr = static_cast<char*>(static_cast<void*>(data));
    initid->maybe_null = true;
    return false;
}

extern "C" void Hierar_cluster_deinit(UDF_INIT * initid) {
    Hierar_data* data =
        static_cast<Hierar_data*>(static_cast<void*>(initid->ptr));
    delete data;
}

extern "C" void Hierar_cluster_add(UDF_INIT * initid, UDF_ARGS * args, unsigned char*,
    unsigned char*) {
    Hierar_data* data =
        static_cast<Hierar_data*>(static_cast<void*>(initid->ptr));
    std::vector<double> tmp;
    for (unsigned i = 2; i < args->arg_count; i++) {
        void* arg = args->args[i];
        double number = *(static_cast<double*>(arg));
        tmp.push_back(number);
    }
    data->vec.push_back(tmp);
}

extern "C" void Hierar_cluster_clear(UDF_INIT * initid, unsigned char*,
    unsigned char*) {
    Hierar_data* data =
        static_cast<Hierar_data*>(static_cast<void*>(initid->ptr));
    data->vec.clear();
}

extern "C" char* Hierar_cluster(UDF_INIT * initid, UDF_ARGS * args,
    char* result, unsigned long* length,
    char* is_null, char* error) {
    Hierar_data* data =
        static_cast<Hierar_data*>(static_cast<void*>(initid->ptr));
    if (data->vec.size() == 0) {
        *is_null = 1;
        return NULL;
    }

    // computation of condensed distance matrix
    int npoints = data->vec.size();
    double* distmat = new double[(npoints * (npoints - 1)) / 2];
    int k = 0;
    for (int i = 0; i < npoints; i++) {
        for (int j = i + 1; j < npoints; j++) {
            distmat[k] = cal_distance(data->vec[i], data->vec[j]);
            k++;
        }
    }
    
    // clustering call
    int* merge = new int[2 * (npoints - 1)];
    double* height = new double[npoints - 1];
    hclust_fast(npoints, distmat, data->opt_method, merge, height);
    int* labels = new int[npoints];
    cutree_k(npoints, merge, data->cluster_num, labels);
    string str_result = "";
    *length = 0;
    for (int i = 0; i < npoints; i++) {
        str_result += std::to_string(labels[i]);
        str_result += ",";
    }
    *length = str_result.length() - 1;
    
    // clean up
    delete[] distmat;
    delete[] merge;
    delete[] height;
    delete[] labels;

    strcpy(data->str_result, str_result.c_str());

    return data->str_result;
}
