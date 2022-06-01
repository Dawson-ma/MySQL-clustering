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
#include <iostream>
#include <random>
#include <charconv>

#include "mysql.h" // IWYU pragma: keep
#include "mysql/udf_registration_types.h"

#include "kmeans_util.h"

using namespace std;

#ifdef _WIN32
/* inet_aton needs winsock library */
#pragma comment(lib, "ws2_32")
#endif

/*
  Not all platforms have gethostbyaddr_r, so we use a global lock here instead.
  Production-quality code should use getaddrinfo where available.
*/
static std::mutex *LOCK_hostname{nullptr};

#ifndef _WIN32
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif

struct KMEANS_data
{
    size_t k;
    size_t max_iter;
    std::vector<int> labels;
    std::vector<std::vector<double>> vec;
    char *str_labels = "";
};

extern "C" bool KMEANS_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
    KMEANS_data *data = new (std::nothrow) KMEANS_data;
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

    // if (args->args[0] <= 0)
    // {
    //     strcpy(message,
    //            "wrong argument type: KMEANS() k should larger than 0");
    //     return true;
    // }

    // if (args->args[1] <= 1)
    // {
    //     strcpy(message,
    //            "wrong argument type: KMEANS() max_iter_number should larger than 1");
    //     return true;
    // }

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

    data->k = *((int *)args->args[0]);
    data->max_iter = *((int *)args->args[1]);

    data->str_labels = (char *)malloc(4096);
    memset(data->str_labels, 0, 4096);
    initid->ptr = static_cast<char *>(static_cast<void *>(data));
    initid->maybe_null = true;
    return false;
}

extern "C" void KMEANS_deinit(UDF_INIT *initid)
{
    KMEANS_data *data =
        static_cast<KMEANS_data *>(static_cast<void *>(initid->ptr));
    delete data;
}

extern "C" void KMEANS_add(UDF_INIT *initid, UDF_ARGS *args, unsigned char *,
                           unsigned char *)
{
    KMEANS_data *data =
        static_cast<KMEANS_data *>(static_cast<void *>(initid->ptr));
    std::vector<double> row;
    for (unsigned i = 2; i < args->arg_count; i++)
    {
        void *arg = args->args[i];
        double number = *(static_cast<double *>(arg));
        row.push_back(number);
    }
    data->vec.push_back(row);
}

extern "C" void KMEANS_clear(UDF_INIT *initid, unsigned char *,
                             unsigned char *)
{
    KMEANS_data *data =
        static_cast<KMEANS_data *>(static_cast<void *>(initid->ptr));
    data->vec.clear();
    data->labels.clear();
}

extern "C" char *KMEANS(UDF_INIT *initid, UDF_ARGS *args,
                        char *result, unsigned long *length,
                        char *is_null, char *error)
{
    KMEANS_data *data =
        static_cast<KMEANS_data *>(static_cast<void *>(initid->ptr));
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
    str.pop_back();
    *length = str.length();
    data->str_labels = strdup(str.c_str());
    
    return data->str_labels;
}
