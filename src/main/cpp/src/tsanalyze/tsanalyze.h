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

#ifndef TSANALYZE_H
#define TSANALYZE_H

#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

#include "models.h"

// Forward declarations for clang to reduce compilation dependencies.
namespace clang {
class Decl;
class ASTContext;
class CompilerInstance;
class DiagnosticsEngine;
class SourceManager;
class QualType;
class Type;
class RecordDecl;
class EnumDecl;
class TypedefNameDecl;
class FunctionDecl;
class ObjCInterfaceDecl;
}  // namespace clang

namespace typesynth {

class TypeAnalyzer {
 public:
  explicit TypeAnalyzer(std::vector<std::string> flags);

  absl::Status AnalyzeSourceFile(const std::string& filepath);

 private:
  // Methods for processing clang type nodes.
  void ProcessTranslationUnit(const clang::ASTContext& context);
  void ProcessDeclaration(const clang::Decl* declaration,
                          const clang::ASTContext& context);

  void ProcessRecordDecl(const clang::RecordDecl& record_decl,
                         const clang::ASTContext& context);
  void ProcessEnumDecl(const clang::EnumDecl& enum_decl,
                       const clang::ASTContext& context);
  void ProcessPointer(const clang::QualType& pointer_type,
                      const clang::ASTContext& context);
  void ProcessFunctionDecl(const clang::FunctionDecl& function_type,
                           const clang::ASTContext& context);
  void ProcessTypedefDecl(const clang::TypedefNameDecl& typedef_decl,
                          const clang::ASTContext& context);
  void ProcessObjCInterfaceDecl(const clang::ObjCInterfaceDecl& interface_decl,
                                const clang::ASTContext& context);

  TypeId IDForQualType(const clang::QualType& qual_type,
                       const clang::ASTContext& context);
  bool IsSymbolicReference(const clang::QualType& qual_type);

  [[nodiscard]] static bool IsRecordPacked(
      const clang::RecordDecl& record_decl);
  [[nodiscard]] bool IsTypeProcessed(TypeId type_id) const;

  absl::StatusOr<models::SourceLocation> SourceLocationFromDecl(
      const clang::Decl* decl, const clang::SourceManager& source_manager);

  static std::string FullyQualifiedDeclName(const clang::Decl& declaration,
                                            const clang::ASTContext& context);

  static std::string NameForRecordDecl(const clang::RecordDecl& record_decl);

  TypeId GetOrCreateTypeId(const clang::QualType& type,
                           const clang::ASTContext& context);

  [[nodiscard]] std::unique_ptr<clang::CompilerInstance>
  CreateCompilerInstance() const;

  // Member variables
  std::vector<std::string> compiler_flags_;
  std::unordered_map<TypeId, models::TypeNode> type_registry_;
  std::unordered_map<std::string, size_t> string_to_type_id_;
  TypeId next_type_id_ = 1;
};

}  // namespace typesynth

#endif  //TSANALYZE_H
