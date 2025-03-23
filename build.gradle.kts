plugins {
    kotlin("jvm") version "2.1.10"
}

group = "com.angelod"
version = "1.0-SNAPSHOT"

val ghidraInstallDir: String? = System.getenv("GHIDRA_INSTALL_DIR") ?: project.findProperty("GHIDRA_INSTALL_DIR") as String?
if (ghidraInstallDir != null) {
    apply(from = File(ghidraInstallDir).canonicalPath + "/support/buildExtension.gradle")
} else {
    throw GradleException("GHIDRA_INSTALL_DIR is not defined!")
}

repositories {
    mavenCentral()
}

dependencies {
    testImplementation(kotlin("test"))
}

tasks.test {
    useJUnitPlatform()
}
kotlin {
    jvmToolchain(21)
}
