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

#include "mysql.h"  // IWYU pragma: keep
#include "mysql/udf_registration_types.h"

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
  KMEANS(int group_num, int max_iter, double features...)
*/

/*
double cal_dis(double* x, double* y, int dim) {
    int i;
    double t, sum = 0.0;
    for (i = 0; i < dim; ++i)
        t = x[i] - y[i], sum += t * t;
    return sum;
}

void   kmeans_init() {
    int i, j, k, rnd;
    int pick[K];
    // 隨機找K 個不同資料點
    for (k = 0; k < K; ++k) {
        rnd = rand() % DCNT; // 隨機取一筆
        for (i = 0; i < k && pick[i] != rnd; ++i);
        if (i == k) pick[k] = rnd; // 沒重覆
        else --k; // 有重覆, 再找一次
    }
    // 將K 個資料點內容複制到重心cent
    for (k = 0; k < K; ++k)
        for (j = 0; j < DIM; ++j)
            cent[k][j] = data[pick[k]][j];
}
*/
struct KMEANS_data{
    std::vector<double> vec;
    char* str_result;
    int k;
    int dim;
    int max_iter;
};

extern "C" bool KMEANS_init(UDF_INIT * initid, UDF_ARGS* args, char* message) {
    KMEANS_data* data = new (std::nothrow) KMEANS_data;
    if (args->arg_count < 3) {
        strcpy(message,
            "wrong argument number: KMEANS() requires at least a group number, a max_iter number and a feature");
        return true;
    }

    if (args->arg_type[0] != INT_RESULT || args->arg_type[1] != INT_RESULT) {
        strcpy(message,
            "wrong argument type: KMEANS() group and max_iter arguments require INT");
        return true;
    }

    data->dim = args->arg_count - 2;
    for (unsigned i=2; i<args->arg_count; i++) {
        if (args->arg_type[i] != INT_RESULT && args->arg_type[i] != REAL_RESULT) {
            strcpy(message,
                "wrong argument type: KMEANS() feature arguments require INT or REAL");
            return true;
        }
        args->arg_type[i] = REAL_RESULT;
    }

    if (!data) {
        strcpy(message, "Could not allocate memory");
        return true;
    }

    int group = *((int*)args->args[0]);
    int iter = *((int*)args->args[1]);
    data->k = group;
    data->max_iter = iter;
    
    data->str_result = (char*)malloc(4096);
    memset(data->str_result, 0, 4096);
    initid->ptr = static_cast<char*>(static_cast<void*>(data));
    initid->maybe_null = true;
    return false;
}

extern "C" void KMEANS_deinit(UDF_INIT * initid) {
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    delete data;
}

extern "C" void KMEANS_add(UDF_INIT * initid, UDF_ARGS *args, unsigned char*,
    unsigned char*) {
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    for (unsigned i=2; i<args->arg_count; i++) {
        void* arg = args->args[i];
        double number = *(static_cast<double*>(arg));
        data->vec.push_back(number);
    }
}

extern "C" void KMEANS_clear(UDF_INIT * initid, unsigned char*,
    unsigned char*) {
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    data->vec.clear();
}

extern "C" char *KMEANS(UDF_INIT * initid, UDF_ARGS * args,
    char* result, unsigned long* length,
    char* is_null, char* error) {
    KMEANS_data* data =
        static_cast<KMEANS_data*>(static_cast<void*>(initid->ptr));
    if (data->vec.size() == 0) {
        *is_null = 1;
        return NULL;
    }
    *length = 7;
    data->str_result = "success";
    return data->str_result;
}
