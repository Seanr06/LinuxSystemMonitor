#ifndef JSON_SERIALIZER_H
#define JSON_SERIALIZER_H

#include <string>

#include "data.h"

// Class for converting the data into a string with .json syntax
class JsonSerializer {
   private:
    std::string serialize_core_data(const CoreData& core);
    std::string serialize_cores(const std::vector<CoreData>& cores);

   public:
    std::string serialize_snapshot(const StatisticsData& data);
};

#endif