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
  Hierar_cluster: aggregate function
  Hierar_cluster(int group, int max_iter, double features...)
*/


struct Hierar_data {
    std::vector<double> vec;
    char* str_result;
    int k;
    int dim;
    int max_iter;
};

extern "C" bool Hierar_cluster_init(UDF_INIT * initid, UDF_ARGS * args, char* message) {
    Hierar_data* data = new (std::nothrow) Hierar_data;
    if (args->arg_count < 3) {
        strcpy(message,
            "wrong argument number: Hierar_cluster() requires at least a group number, a max_iter number and a feature");
        return true;
    }

    if (args->arg_type[0] != INT_RESULT || args->arg_type[1] != INT_RESULT) {
        strcpy(message,
            "wrong argument type: Hierar_cluster() group and max_iter arguments require INT");
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
    *length = 14;
    data->str_result = "success output";
    return data->str_result;
}
