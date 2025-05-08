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

#include <clang/Frontend/CompilerInstance.h>
#include <llvm/ADT/StringExtras.h>

#include "absl/strings/str_cat.h"
#include "tsanalyze.h"

namespace typesynth {

using models::NodeKind;

TypeAnalyzer::TypeAnalyzer(std::vector<std::string> flags)
    : compiler_flags_(std::move(flags)) {}

absl::Status TypeAnalyzer::AnalyzeSourceFile(const std::string& filepath) {
  auto compiler = CreateCompilerInstance();

  // Initialize the file and source management components for clang.
  compiler->createFileManager();
  clang::FileManager& file_manager = compiler->getFileManager();
  compiler->createSourceManager(file_manager);
  clang::SourceManager& source_manager = compiler->getSourceManager();

  // Set up the source file with clang.
  const clang::FileEntryRef file = file_manager.getFileRef(filepath).get();
  if (!file) {
    return absl::NotFoundError(absl::StrCat("File not found: ", filepath));
  }

  clang::FileID file_id = source_manager.createFileID(
      file, clang::SourceLocation(), clang::SrcMgr::C_User);
  if (file_id.isInvalid()) {
    return absl::InternalError("Failed to create the clang File ID");
  }

  source_manager.setMainFileID(file_id);

  compiler->createPreprocessor(clang::TU_Complete);
  clang::Preprocessor& preprocessor = compiler->getPreprocessor();

  compiler->createASTContext();
  clang::ASTContext& ast_context = compiler->getASTContext();

  // todo: possibly refactor this into a different reusable method.
}

void TypeAnalyzer::ProcessTranslationUnit(const clang::ASTContext& context) {
  for (const clang::Decl* decl : context.getTranslationUnitDecl()->decls()) {
    ProcessDeclaration(decl, context);
  }
}

void TypeAnalyzer::ProcessDeclaration(const clang::Decl* declaration,
                                      const clang::ASTContext& context) {
  // Applicable language constructs:
  //  * Unions
  //  * Functions
  //  * Structs
  //  * Typedefs
  //  * Objective-C Classes

  // This method will route AST declarations to their appropriate
  //  processing routine.

  if (!declaration)
    return;

  switch (declaration->getKind()) {
    case clang::Decl::Record: {
      // Includes: Structs and unions.
      auto record_decl = llvm::dyn_cast<clang::RecordDecl>(declaration);
      if (!record_decl)
        break;

      if (record_decl->isStruct() || record_decl->isUnion()) {
        ProcessRecordDecl(*record_decl, context);
      }
      break;
    }
    case clang::Decl::Function: {
      auto function_decl = llvm::dyn_cast<clang::FunctionDecl>(declaration);
      if (!function_decl)
        break;

      ProcessFunctionDecl(*function_decl, context);
      break;
    }
    case clang::Decl::Enum: {
      auto enum_decl = llvm::dyn_cast<clang::EnumDecl>(declaration);
      if (!enum_decl)
        break;

      ProcessEnumDecl(*enum_decl, context);
      break;
    }
    case clang::Decl::Typedef: {
      auto typedef_decl = llvm::dyn_cast<clang::TypedefDecl>(declaration);
      if (!typedef_decl)
        break;

      ProcessTypedefDecl(*typedef_decl, context);
      break;
    }
    case clang::Decl::ObjCInterface: {
      auto interface_decl =
          llvm::dyn_cast<clang::ObjCInterfaceDecl>(declaration);
      if (!interface_decl)
        break;

      ProcessObjCInterfaceDecl(*interface_decl, context);
      break;
    }
    case clang::Decl::Namespace: {
      auto namespace_decl = llvm::dyn_cast<clang::NamespaceDecl>(declaration);
      if (!namespace_decl)
        break;

      for (auto ns_decl : namespace_decl->decls()) {
        ProcessDeclaration(ns_decl, context);
      }
      break;
    }
    default:
      break;
  }
}

void TypeAnalyzer::ProcessRecordDecl(const clang::RecordDecl& record_decl,
                                     const clang::ASTContext& context) {

  bool is_packed = IsRecordPacked(record_decl);
  bool is_anon = record_decl.isAnonymousStructOrUnion();

  // when we encounter an inline/anonymous structure declaration, model it as
  // though it is both: a declaration and subsequent usage.

  // Record each field while incrementing the bit width counter to use for
  //  the offset_bits value.
  uint32_t off_bits_ctr = 0;
  std::vector<models::RecordField> fields;
  for (auto field : record_decl.fields()) {
    auto entry = models::RecordField{
        .name = field->getNameAsString(),
        .type = IDForQualType(field->getType(),
                              context),
        .offset_bits = off_bits_ctr};
    fields.push_back(entry);
    off_bits_ctr += field->getBitWidthValue();
  }

  if (record_decl.isStruct()) {
    auto node = models::StructDecl{
        next_type_id_,
        NodeKind::kStructDeclaration,
        .name = NameForRecordDecl(record_decl),
        .qualified_name = FullyQualifiedDeclName(record_decl, context),
        .fields = fields,
        .is_packed = is_packed,
        .is_anonymous = is_anon};
    // todo: push to type registry
  } else if (record_decl.isUnion()) {
    auto node = models::UnionDecl{
        next_type_id_,
        NodeKind::kUnionDeclaration,
        .name = NameForRecordDecl(record_decl),
        .qualified_name = FullyQualifiedDeclName(record_decl, context),
        .fields = fields,
        .is_packed = is_packed,
        .is_anonymous = is_anon};
    // todo: push to type registry
  }
}

TypeId TypeAnalyzer::IDForQualType(const clang::QualType& qual_type,
                                   const clang::ASTContext& context) {

  // Check if a matching node is in our cache.
  clang::PrintingPolicy policy(context.getLangOpts());
  policy.adjustForCPlusPlus();
  std::string type_as_string = qual_type.getAsString(policy);
  if (string_to_type_id_.contains(type_as_string)) {
    return string_to_type_id_[type_as_string];
  }

  TypeId next = next_type_id_++;

  if (qual_type->isBuiltinType()) {
    auto node = models::Primitive{
        next,
        NodeKind::kPrimitive,
        type_as_string,
    };
    // todo: push to type registry
  } else if (qual_type->isPointerType()) {
    clang::QualType inner = qual_type->getPointeeType();
    auto node = models::Pointer{next, NodeKind::kPointer,
                                .inner = IDForQualType(inner, context)};
    // todo: push to type registry
  } else if (qual_type->isReferenceType()) {
    clang::QualType inner = qual_type.getNonReferenceType();
    auto node = models::Reference{next, NodeKind::kReference,
                                  .inner = IDForQualType(inner, context)};
    // todo: push to type registry
  } else if (qual_type->isFunctionType()) {
    const auto* fn_proto = llvm::cast<clang::FunctionProtoType>(qual_type);

    TypeId ret_type_id = IDForQualType(fn_proto->getReturnType(), context);
    bool is_variadic = fn_proto->isVariadic();

    std::vector<models::FunctionArgument> args;

    for (const auto arg : fn_proto->getParamTypes()) {
      args.push_back(models::FunctionArgument {
        .name = "",
        .type = IDForQualType(arg, context)
      });
    }

    auto node = models::Function{
      next,
      NodeKind::kFunction,
      .args = args,
      .is_variadic = is_variadic
    };
    // todo: push to type registry
  } else if (IsSymbolicReference(qual_type)) {
    // Handle declared identifiers.

    // fixme: bad idea to do just this.
    TypeId referenced = string_to_type_id_[type_as_string];

    auto node = models::SymbolicReference {
      next,
      NodeKind::kSymbolicReference,
      .inner = referenced
    };
    // todo: push to type registry
  }
}

bool TypeAnalyzer::IsSymbolicReference(const clang::QualType& qual_type) {

  return qual_type->isTypedefNameType() ||
         qual_type->getAsRecordDecl() != nullptr ||
         qual_type->getAs<clang::EnumType>() != nullptr ||
         qual_type->getAs<clang::ElaboratedType>() != nullptr;
}

bool TypeAnalyzer::IsRecordPacked(const clang::RecordDecl& record_decl) {
  if (record_decl.hasAttr<clang::PackedAttr>()) {
    return true;
  }

  // if this is a forward-declaration, go to the original definition and check
  // for the packed attribute.
  if (!record_decl.isCompleteDefinition()) {
    const clang::RecordDecl* definition = record_decl.getDefinition();

    if (definition && definition != &record_decl) {
      return definition->hasAttr<clang::PackedAttr>();
    }
  }

  return false;
}

bool TypeAnalyzer::IsTypeProcessed(TypeId type_id) const {
  return type_registry_.contains(type_id);
}

std::string TypeAnalyzer::FullyQualifiedDeclName(
    const clang::Decl& declaration, const clang::ASTContext& context) {

  // todo: consider refactoring to return an error status if the declaration is
  //  anonymous.

  // To determine the fully-qualified identifier of a declaration,
  // we are going to recurse and record the identifiers of each of its
  // lexical contexts until we reach the translation unit.

  const clang::DeclContext* ctx = declaration.getDeclContext();
  std::vector<std::string> name_stack;

  while (ctx && !llvm::isa<clang::TranslationUnitDecl>(ctx)) {
    if (llvm::isa<clang::NamespaceDecl>(ctx)) {
      // Handle: namespaces
      const auto* ns = llvm::cast<clang::NamespaceDecl>(ctx);
      if (ns->isAnonymousNamespace()) {
        name_stack.emplace_back("(anonymous namespace)");
      } else {
        std::string ns_name = ns->getNameAsString();
        name_stack.push_back(ns_name);
      }
    } else if (llvm::isa<clang::RecordDecl>(ctx)) {
      // Handle: structs, unions
      const auto* record = llvm::cast<clang::RecordDecl>(ctx);
      name_stack.push_back(NameForRecordDecl(*record));
    }
    ctx = ctx->getLexicalParent();
  }

  std::ranges::reverse(name_stack);

  // Now, determine if this is a named declaration, and if so, append it to our
  // final result.

  if (const auto* named_decl = llvm::dyn_cast<clang::NamedDecl>(&declaration)) {
    std::string name = named_decl->getNameAsString();
    if (name.empty())
      return "";

    name_stack.push_back(name);
  }

  std::string fq_name = llvm::join(name_stack.begin(), name_stack.end(), "::");

  return fq_name;
}

std::string TypeAnalyzer::NameForRecordDecl(
    const clang::RecordDecl& record_decl) {

  if (record_decl.isAnonymousStructOrUnion()) {
    std::string anon_name = "(anonymous ";
    if (record_decl.isStruct())
      anon_name += "struct";
    else
      anon_name += "union";
    anon_name += ")";
    return anon_name;
  }

  std::string record_name = record_decl.getNameAsString();
  return record_name;
}

std::unique_ptr<clang::CompilerInstance> TypeAnalyzer::CreateCompilerInstance()
    const {

  auto compiler = std::make_unique<clang::CompilerInstance>();

  // Create diagnostics engine
  auto* diagnostics_engine = new clang::DiagnosticsEngine(
      clang::IntrusiveRefCntPtr<clang::DiagnosticIDs>(
          new clang::DiagnosticIDs()),
      new clang::DiagnosticOptions);
  compiler->setDiagnostics(diagnostics_engine);

  auto invocation = std::make_shared<clang::CompilerInvocation>();

  std::vector<const char*> cflags;
  cflags.reserve(compiler_flags_.size());
  for (const auto& flag : compiler_flags_) {
    cflags.push_back(flag.c_str());
  }
  clang::CompilerInvocation::CreateFromArgs(*invocation, cflags,
                                            compiler->getDiagnostics());

  compiler->setInvocation(invocation);

  return compiler;
}

}  // namespace typesynth
