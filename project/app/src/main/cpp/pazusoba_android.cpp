#include <jni.h>
#include <string>
#include <pazusoba/core.h>

/// Convert jstring to std::string
std::string convertJString(JNIEnv *env, jstring javaString) {
    auto length = env->GetStringUTFLength(javaString);
    auto charPointer = env->GetStringUTFChars(javaString, nullptr);
    std::string nativeString(charPointer, length);
    // Free up the pointer
    env->ReleaseStringUTFChars(javaString, charPointer);
    return nativeString;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_org_github_henryquan_pazusoba_1android_core_Pazusoba_solve(
        JNIEnv *env, jobject,
        jstring board, jint min_erase,
        jint max_step, jint beam_size
) {
    auto board_string = convertJString(env, board);
    auto parser = pazusoba::Parser(board_string, min_erase, max_step, beam_size);
    parser.parse();

    auto solver = pazusoba::BeamSearch(parser);
    auto state = solver.solve();
    auto route = state.route();

    // append combo at the end
    auto size = route.totalSteps() + 1;
    auto stepList = env->NewIntArray(size);
    if (stepList == nullptr) {
        return nullptr;
    }

    jint temp[size];
    auto routeList = route.list();
    int index;
    // exclude combo here
    for (index = 0; index < size - 1; index++) {
        temp[index] = routeList[index];
    }
    // add combo in the end
    temp[index] = state.combo();

    // fill temp data to return list
    env->SetIntArrayRegion(stepList, 0, size, temp);
    return stepList;
}

