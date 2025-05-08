#include <iostream>

#include <clang/AST/ASTContext.h>
#include <clang/Basic/TargetInfo.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Basic/FileManager.h>
#include <llvm/TargetParser/Host.h>

// #include <map>

static const std::string path = "./jsxer/src/jsxer/jsxer.cpp";

// void ParseSourceFile(const char* source_file) {
//   CXIndex index = clang_createIndex(0, 0);
//
//   const char* args[] = {"-I.", "-std=c++20"};
//
//   CXTranslationUnit unit = clang_parseTranslationUnit(
//       index, source_file, args, 2, nullptr, 0, CXTranslationUnit_None);
//
//   if (unit == nullptr) {
//     std::cerr << "Couldn't parse source file" << std::endl;
//     return;
//   }
//
//   CXCursor cursor = clang_getTranslationUnitCursor(unit);
//
//   std::map<std::string, CXCursor> symbol_table;
//   auto visitor = [](CXCursor c, CXCursor parent, CXClientData client_data) {
//     const auto table = static_cast<std::map<std::string, CXCursor>*>(client_data);
//
//     if (clang_getCursorKind(c) == CXCursor_FunctionDecl) {
//       const CXString name = clang_getCursorSpelling(c);
//       table->insert_or_assign(std::string(clang_getCString(name)), c);
//       clang_disposeString(name);
//     }
//
//     return CXChildVisit_Recurse;
//   };
//   clang_visitChildren(cursor, visitor, &symbol_table);
//
//   for (const auto& [key, cursor] : symbol_table) {
//
//     auto location = clang_getCursorLocation(cursor);
//
//     CXFile file;
//     unsigned line, column, offset;
//     clang_getSpellingLocation(location, &file, &line, &column, &offset);
//
//     auto file_name = std::string(clang_getCString(clang_getFileName(file)));
//
//     std::cout << " * Function: " << key << ", Origin: " << file_name << std::endl;
//   }
// }

void ParseSourceFile(const std::string& fpath) {
  const char* args[] = {"-x", "c++", "-std=c++20"};

  clang::CompilerInstance compiler;
  compiler.createDiagnostics(compiler.getVirtualFileSystem());

  // Create compiler invocation info
  auto invocation = std::make_shared<clang::CompilerInvocation>();
  clang::CompilerInvocation::CreateFromArgs(*invocation, args,
                                            compiler.getDiagnostics());

  compiler.setInvocation(invocation);

  // Configure target...
  std::shared_ptr<clang::TargetOptions> target_opts(new clang::TargetOptions);
  target_opts->Triple = llvm::sys::getDefaultTargetTriple();

  compiler.setTarget(clang::TargetInfo::CreateTargetInfo(
      compiler.getDiagnostics(), target_opts));

  compiler.createFileManager();
  compiler.createSourceManager(compiler.getFileManager());
  compiler.createPreprocessor(clang::TU_Complete);
  compiler.createASTContext();


  const clang::FileEntry* file = compiler.getFileManager().getFile(fpath).get();
  if (!file) {
    // error
    return;
  }

}

int main() {

  ParseSourceFile(path);

  return 0;
}
