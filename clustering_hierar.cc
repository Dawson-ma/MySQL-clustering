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

#include "fastcluster.h"

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
  Hierar_cluster: aggregate function
  Hierar_cluster(int n, string method, double features...)
  Input arguments:
   n       = number of observables
   method  = cluster metric: "single", "complete", "average", "median"
*/


// points distance
double distance(double *s1, double *s2) {
    int size = sizeof(s1);
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += pow(s1[i] - s2[i], 2);
    }
    return sqrt(sum);
}

struct Hierar_data {
    std::vector<double> vec;
    char* str_result="";
    int cluster_num=0;
    int dim=0;
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
    

    data->dim = args->arg_count - 2;
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

    
    int cluster_num = *((int*)args->args[0]);
    data->cluster_num = cluster_num;

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
    for (unsigned i = 2; i < args->arg_count; i++) {
        void* arg = args->args[i];
        double number = *(static_cast<double*>(arg));
        data->vec.push_back(number);
    }
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
    int npoints = data->vec.size() / data->dim;
    double* distmat = new double[(npoints * (npoints - 1)) / 2];
    double* point1 = new double[data->dim];
    double* point2 = new double[data->dim];
    int k = 0;
    for (int i = 0; i < npoints; i++) {
        for (int x = 0; x < data->dim; x++) {
            point1[x] = data->vec.at((i * data->dim) + x);
        }
        for (int j = i + 1; j < npoints; j++) {
            for (int y = 0; y < data->dim; y++) {
                point2[y] = data->vec.at((j * data->dim) + y);
            }
            distmat[k] = distance(point1, point2);
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
    *length = str_result.length();
    
    // clean up
    delete[] distmat;
    delete[] merge;
    delete[] height;
    delete[] labels;
    

    /*
    string str_result = "";
    for (int i = 0; i < data->vec.size(); i++) {
        str_result += std::to_string(data->vec[i]);
        str_result += ",";
    }
    *length = str_result.length();*/
    strcpy(data->str_result, str_result.c_str());

    return data->str_result;
}
