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
