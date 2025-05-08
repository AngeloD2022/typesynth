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

#ifndef MODELS_H
#define MODELS_H

#include <map>
#include <string>
#include <vector>

namespace typesynth {
using TypeId = uint32_t;
namespace models {

struct SourceLocation {
  std::string file;
  uint64_t start;
  uint64_t end;
  std::string snippet;
};

enum class NodeKind {
  kStructDeclaration,
  kUnionDeclaration,
  kTypedefDeclaration,
  kFunctionDeclaration,
  kPointer,
  kReference,
  kPrimitive,
  kSymbolicReference,
  kFunction,
};

struct TypeNode {
  TypeId id;
  NodeKind kind;
};

struct Pointer : TypeNode {
  TypeId inner;
};

struct Reference : TypeNode {
  TypeId inner;
};

struct SymbolicReference : TypeNode {
  TypeId inner;
};

struct FunctionArgument {
  std::string name;
  TypeId type;
};

struct Function : TypeNode {
  TypeId ret_type;
  std::vector<FunctionArgument> args;
  bool is_variadic;
};

struct Primitive : TypeNode {
  std::string primitive;
};

struct RecordField {
  std::string name;
  TypeId type;
  uint32_t offset_bits;
};

struct StructDecl : TypeNode {
  std::string name;
  std::string qualified_name;
  std::vector<RecordField> fields;
  bool is_packed;
  bool is_anonymous;
};

struct UnionDecl : TypeNode {
  std::string name;
  std::string qualified_name;
  std::vector<RecordField> fields;
  bool is_packed;
  bool is_anonymous;
};

}  // namespace models
}  // namespace typesynth

#endif  //MODELS_H
