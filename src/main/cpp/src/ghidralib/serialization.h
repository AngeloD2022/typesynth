/*
 * Typesynth
 *
 * Copyright (c) 2025 Angelo DeLuca
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
