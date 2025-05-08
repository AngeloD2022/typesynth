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
