/*
 *  This file is part of Typesynth.
 *
 *  Copyright (C) 2025 Angelo DeLuca and contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "../tsanalyze/models.h"

using json = nlohmann::json;

void to_json(json& j, const SourceLocation& loc) {
  j = json{{"file", loc.file},
           {"start", loc.start},
           {"end", loc.end},
           {"snippet", loc.snippet}};
}

void to_json(json& j, const RecordField& field) {
  j = json{{"name", field.name},
           {"type", field.type_id},
           {"offsetInBits", field.offset_in_bits}};
}

void to_json(json& j, const EnumConstant& eConst) {
  j = json{{"name", eConst.name}, {"value", eConst.value}};
}

void to_json(json& j, const TSType& type) {
  j = json{{"kind", type.kind},
           {"name", type.name},
           {"sourceLocation", type.source_location}};

  if (type.kind == "PrimitiveType") {
    j["sizeInBits"] = type.primitive_type.size_in_bits;
    j["signed"] = type.primitive_type.is_signed;
  } else if (type.kind == "PointerType") {
    j["pointeeType"] = type.pointer_type.pointee_type_id;
  } else if (type.kind == "StructType") {
    j["fields"] = type.struct_type.fields;
    j["sizeInBytes"] = type.struct_type.size_in_bytes;
  } else if (type.kind == "EnumType") {
    j["enumerators"] = type.enum_type.enumerators;
    j["sizeInBytes"] = type.enum_type.size_in_bytes;
  } else if (type.kind == "FunctionPrototype") {
    j["returnType"] = type.function_prototype.return_type_id;
    j["parameterTypes"] = type.function_prototype.parameter_type_ids;
    j["isVariadic"] = type.function_prototype.is_variadic;
  } else if (type.kind == "TypedefType") {
    j["underlyingType"] = type.typedef_type.underlying_type_id;
  }
}

struct TypeAnalysisResultCPP {
  std::string mainFile;
  std::vector<std::string> files;
  std::vector<std::string> clangFlags;
  std::map<std::string, TSType> types;
};

void to_json(json& j, const TypeAnalysisResultCPP& result) {
  j = json{{"mainFile", result.mainFile},
           {"files", result.files},
           {"clangFlags", result.clangFlags},
           {"types", result.types}};
}

#endif  //SERIALIZATION_H
