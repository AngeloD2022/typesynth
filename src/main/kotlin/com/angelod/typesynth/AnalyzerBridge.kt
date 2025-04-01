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