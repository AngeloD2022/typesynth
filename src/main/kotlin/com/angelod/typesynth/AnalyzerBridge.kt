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

package com.angelod.typesynth

sealed class TSType {
    abstract val name: String

    data class PrimitiveType(
        override val name: String, // int, char, float, etc.
        val sizeInBits: Int,
        val signed: Boolean
    ) : TSType()

    data class PointerType(
        override val name: String,
        val pointeeTypeName: String // Reference by name to avoid complex object recursion over JNI
    ) : TSType()

    data class StructType(
        override val name: String,
        val fields: List<StructField>,
        val sizeInBytes: Int,
    ) : TSType()

    data class EnumType(
        override val name: String,
        val enumerators: List<EnumConstant>,
        val sizeInBytes: Int,
    ) : TSType()

    data class FunctionPrototype(
        override val name: String,
        val returnType: String,
        val parameterTypes: List<String>,
        val isVariadic: Boolean,
    ) : TSType()

    data class TypedefType(
        override val name: String,
        val underlyingType: String
    ) : TSType()
}

data class StructField(
    val name: String,
    val typeName: String, // reference type by name
    val offsetInBits: Int
)

data class EnumConstant(
    val name: String,
    val value: Long
)

data class TypeAnalysisResult(
    val mainFile: String,
    val files: List<String>,
    val clangFlags: List<String>,
    val types: Map<String, TSType>,
)

class AnalyzerBridge {

    companion object {
        init {
            System.loadLibrary("tsAnalysis")
        }
    }

    private external fun jniAnalyzeSourceFile(mainFile: String, clangFlags: List<String>): String

    /**
     * Performs analysis on the given source file and associated clang compilation flags.
     *
     * @param mainFile The path to the main source file to be analyzed.
     * @param clangFlags A list of clang compiler flags used during the analysis.
     * @return A `TypeAnalysisResult` containing the analysis details, including associated files,
     *         clang flags, and detected types.
     */
    fun analyzeSourceFile(mainFile: String, clangFlags: List<String>): TypeAnalysisResult? {
        return null
    }
}